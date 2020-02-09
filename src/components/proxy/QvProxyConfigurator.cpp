#include "QvProxyConfigurator.hpp"
#include "common/QvHelpers.hpp"
#ifdef Q_OS_WIN
#include "wininet.h"
#include <windows.h>
#endif

namespace Qv2ray::components::proxy
{

#ifdef Q_OS_MACOS
    QStringList macOSgetNetworkServices()
    {
        QProcess p;
        p.setProgram("/usr/sbin/networksetup");
        p.setArguments(QStringList() << "-listallnetworkservices");
        p.start();
        p.waitForStarted();
        p.waitForFinished();
        LOG(PROXY, p.errorString())
        auto str = p.readAllStandardOutput();
        auto lines = SplitLines(str);
        QStringList result;

        // Start from 1 since first line is unneeded.
        for (auto i = 1; i < lines.count(); i++) {
            // * means disabled.
            if (!lines[i].contains("*"))  {
                result << (lines[i].contains(" ") ? "\"" +  lines[i] + "\"" : lines[i]);
            }
        }

        LOG(PROXY, "Found " + QSTRN(result.size()) + " network services: " + result.join(";"))
        return result;
    }
#endif
#ifdef Q_OS_WIN
#define NO_CONST(expr) const_cast<wchar_t *>(expr)
    //static auto DEFAULT_CONNECTION_NAME = NO_CONST(L"DefaultConnectionSettings");
    ///
    /// INTERNAL FUNCTION
    bool __QueryProxyOptions()
    {
        INTERNET_PER_CONN_OPTION_LIST List;
        INTERNET_PER_CONN_OPTION Option[5];
        //
        unsigned long nSize = sizeof(INTERNET_PER_CONN_OPTION_LIST);
        Option[0].dwOption = INTERNET_PER_CONN_AUTOCONFIG_URL;
        Option[1].dwOption = INTERNET_PER_CONN_AUTODISCOVERY_FLAGS;
        Option[2].dwOption = INTERNET_PER_CONN_FLAGS;
        Option[3].dwOption = INTERNET_PER_CONN_PROXY_BYPASS;
        Option[4].dwOption = INTERNET_PER_CONN_PROXY_SERVER;
        //
        List.dwSize = sizeof(INTERNET_PER_CONN_OPTION_LIST);
        List.pszConnection = nullptr;// NO_CONST(DEFAULT_CONNECTION_NAME);
        List.dwOptionCount = 5;
        List.dwOptionError = 0;
        List.pOptions = Option;

        if (!InternetQueryOption(nullptr, INTERNET_OPTION_PER_CONNECTION_OPTION, &List, &nSize)) {
            LOG(PROXY, "InternetQueryOption failed, GLE=" + QSTRN(GetLastError()))
        }

        LOG(PROXY, "System default proxy info:")

        if (Option[0].Value.pszValue != nullptr) {
            LOG(PROXY, QString::fromWCharArray(Option[0].Value.pszValue))
        }

        if ((Option[2].Value.dwValue & PROXY_TYPE_AUTO_PROXY_URL) == PROXY_TYPE_AUTO_PROXY_URL) {
            LOG(PROXY, "PROXY_TYPE_AUTO_PROXY_URL")
        }

        if ((Option[2].Value.dwValue & PROXY_TYPE_AUTO_DETECT) == PROXY_TYPE_AUTO_DETECT) {
            LOG(PROXY, "PROXY_TYPE_AUTO_DETECT")
        }

        if ((Option[2].Value.dwValue & PROXY_TYPE_DIRECT) == PROXY_TYPE_DIRECT) {
            LOG(PROXY, "PROXY_TYPE_DIRECT")
        }

        if ((Option[2].Value.dwValue & PROXY_TYPE_PROXY) == PROXY_TYPE_PROXY) {
            LOG(PROXY, "PROXY_TYPE_PROXY")
        }

        if (!InternetQueryOption(nullptr, INTERNET_OPTION_PER_CONNECTION_OPTION, &List, &nSize)) {
            LOG(PROXY, "InternetQueryOption failed,GLE=" + QSTRN(GetLastError()))
        }

        if (Option[4].Value.pszValue != nullptr) {
            LOG(PROXY, QString::fromStdWString(Option[4].Value.pszValue))
        }

        INTERNET_VERSION_INFO Version;
        nSize = sizeof(INTERNET_VERSION_INFO);
        InternetQueryOption(nullptr, INTERNET_OPTION_VERSION, &Version, &nSize);

        if (Option[0].Value.pszValue != nullptr) {
            GlobalFree(Option[0].Value.pszValue);
        }

        if (Option[3].Value.pszValue != nullptr) {
            GlobalFree(Option[3].Value.pszValue);
        }

        if (Option[4].Value.pszValue != nullptr) {
            GlobalFree(Option[4].Value.pszValue);
        }

        return false;
    }
    bool __SetProxyOptions(LPWSTR proxy_full_addr, bool isPAC)
    {
        INTERNET_PER_CONN_OPTION_LIST list;
        BOOL    bReturn;
        DWORD   dwBufSize = sizeof(list);
        // Fill the list structure.
        list.dwSize = sizeof(list);
        // NULL == LAN, otherwise connectoid name.
        list.pszConnection = nullptr;

        if (isPAC) {
            LOG(PROXY, "Setting system proxy for PAC")
            //
            list.dwOptionCount = 2;
            list.pOptions = new INTERNET_PER_CONN_OPTION[2];

            // Ensure that the memory was allocated.
            if (nullptr == list.pOptions) {
                // Return FALSE if the memory wasn't allocated.
                return FALSE;
            }

            // Set flags.
            list.pOptions[0].dwOption = INTERNET_PER_CONN_FLAGS;
            list.pOptions[0].Value.dwValue = PROXY_TYPE_DIRECT | PROXY_TYPE_AUTO_PROXY_URL;
            // Set proxy name.
            list.pOptions[1].dwOption = INTERNET_PER_CONN_AUTOCONFIG_URL;
            list.pOptions[1].Value.pszValue = proxy_full_addr;
        } else {
            LOG(PROXY, "Setting system proxy for Global Proxy")
            //
            list.dwOptionCount = 3;
            list.pOptions = new INTERNET_PER_CONN_OPTION[3];

            if (nullptr == list.pOptions) {
                return false;
            }

            // Set flags.
            list.pOptions[0].dwOption = INTERNET_PER_CONN_FLAGS;
            list.pOptions[0].Value.dwValue = PROXY_TYPE_DIRECT | PROXY_TYPE_PROXY;
            // Set proxy name.
            list.pOptions[1].dwOption = INTERNET_PER_CONN_PROXY_SERVER;
            list.pOptions[1].Value.pszValue = proxy_full_addr;
            // Set proxy override.
            list.pOptions[2].dwOption = INTERNET_PER_CONN_PROXY_BYPASS;
            auto localhost = L"localhost";
            list.pOptions[2].Value.pszValue = NO_CONST(localhost);
        }

        // Set the options on the connection.
        bReturn = InternetSetOption(nullptr, INTERNET_OPTION_PER_CONNECTION_OPTION, &list, dwBufSize);
        delete [] list.pOptions;
        InternetSetOption(nullptr, INTERNET_OPTION_SETTINGS_CHANGED, nullptr, 0);
        InternetSetOption(nullptr, INTERNET_OPTION_REFRESH, nullptr, 0);
        return bReturn;
    }
#endif

    bool SetSystemProxy(const QString &address, int httpPort, int socksPort, bool usePAC)
    {
        bool hasHTTP = httpPort == 0;
        bool hasSOCKS = socksPort == 0;
#ifdef Q_OS_WIN
        QString __a;

        if (usePAC) {
            __a = address;
        } else {
            __a = (hasHTTP ? "http://" : "socks5://") + address + ":" + QSTRN(httpPort);
        }

        if (hasHTTP || hasSOCKS) {
            auto proxyStrW = new WCHAR[__a.length() + 1];
            wcscpy(proxyStrW, __a.toStdWString().c_str());
            //
            __QueryProxyOptions();

            if (!__SetProxyOptions(proxyStrW, usePAC)) {
                LOG(PROXY, "Failed to set proxy.")
                return false;
            }

            __QueryProxyOptions();
            return true;
        } else {
            return false;
        }

#elif defined(Q_OS_LINUX)
        QStringList actions;
        auto proxyMode = usePAC ? "auto" : "manual";
        actions << QString("gsettings set org.gnome.system.proxy mode '%1'").arg(proxyMode);

        if (usePAC) {
            actions << QString("gsettings set org.gnome.system.proxy autoconfig-url '%1'").arg(address);
        } else {
            if (hasHTTP) {
                actions << QString("gsettings set org.gnome.system.proxy.http host '%1'").arg(address);
                actions << QString("gsettings set org.gnome.system.proxy.http port %1").arg(httpPort);
                //
                actions << QString("gsettings set org.gnome.system.proxy.https host '%1'").arg(address);
                actions << QString("gsettings set org.gnome.system.proxy.https port %1").arg(httpPort);;
            }

            if (hasSOCKS) {
                actions << QString("gsettings set org.gnome.system.proxy.socks host '%1'").arg(address);
                actions << QString("gsettings set org.gnome.system.proxy.socks port %1").arg(socksPort);
            }
        }

        // note: do not use std::all_of / any_of / none_of,
        // because those are short-circuit and cannot guarantee atomicity.
        auto result = std::count_if(actions.cbegin(), actions.cend(), [](const QString & action) {
            DEBUG(PROXY, action)
            return QProcess::execute(action) == QProcess::NormalExit;
        }) == actions.size();

        if (!result) {
            LOG(PROXY, "Something wrong happens when setting system proxy -> Gnome ONLY.")
            LOG(PROXY, "If you are using KDE Plasma and receiving this message, just simply ignore this.")
        }

        return result;
#else
        bool result = true;

        for (auto service : macOSgetNetworkServices()) {
            LOG(PROXY, "Setting proxy for interface: " + service)

            if (usePAC) {
                result = result && QProcess::execute("/usr/sbin/networksetup -setautoproxystate " + service + " on") == QProcess::NormalExit;
                result = result && QProcess::execute("/usr/sbin/networksetup -setautoproxyurl " + service + " " + address) == QProcess::NormalExit;
            } else {
                if (hasHTTP) {
                    result = result && QProcess::execute("/usr/sbin/networksetup -setwebproxystate " + service + " on") == QProcess::NormalExit;
                    result = result && QProcess::execute("/usr/sbin/networksetup -setsecurewebproxystate " + service + " on") == QProcess::NormalExit;
                    result = result && QProcess::execute("/usr/sbin/networksetup -setwebproxy " + service + " " + address + " " + QSTRN(httpPort)) == QProcess::NormalExit;
                    result = result && QProcess::execute("/usr/sbin/networksetup -setsecurewebproxy " + service + " " + address + " " + QSTRN(httpPort)) == QProcess::NormalExit;
                }

                if (hasSOCKS) {
                    result = result && QProcess::execute("/usr/sbin/networksetup -setsocksfirewallproxystate " + service + " on") == QProcess::NormalExit;
                    result = result && QProcess::execute("/usr/sbin/networksetup -setsocksfirewallproxy " + service + " " + address + " " + QSTRN(socksPort)) == QProcess::NormalExit;
                }
            }
        }

        return result;
#endif
    }

    bool ClearSystemProxy()
    {
#ifdef Q_OS_WIN
        LOG(PROXY, "Cleaning system proxy settings.")
        INTERNET_PER_CONN_OPTION_LIST list;
        BOOL bReturn;
        DWORD dwBufSize = sizeof(list);
        // Fill out list struct.
        list.dwSize = sizeof(list);
        // nullptr == LAN, otherwise connectoid name.
        list.pszConnection = nullptr;
        // Set three options.
        list.dwOptionCount = 1;
        list.pOptions = new INTERNET_PER_CONN_OPTION[list.dwOptionCount];

        // Make sure the memory was allocated.
        if (nullptr == list.pOptions) {
            // Return FALSE if the memory wasn't allocated.
            LOG(PROXY, "Failed to allocat memory in DisableConnectionProxy()")
            return FALSE;
        }

        // Set flags.
        list.pOptions[0].dwOption = INTERNET_PER_CONN_FLAGS;
        list.pOptions[0].Value.dwValue = PROXY_TYPE_DIRECT;
        //
        // Set the options on the connection.
        bReturn = InternetSetOption(nullptr, INTERNET_OPTION_PER_CONNECTION_OPTION, &list, dwBufSize);
        delete [] list.pOptions;
        InternetSetOption(nullptr, INTERNET_OPTION_SETTINGS_CHANGED, nullptr, 0);
        InternetSetOption(nullptr, INTERNET_OPTION_REFRESH, nullptr, 0);
        return bReturn;
#elif defined(Q_OS_LINUX)
        return QProcess::execute("gsettings set org.gnome.system.proxy mode 'none'") == QProcess::ExitStatus::NormalExit;
#else
        bool result = true;

        for (auto service : macOSgetNetworkServices()) {
            result = result && QProcess::execute("/usr/sbin/networksetup -setautoproxystate " + service + " off") == QProcess::NormalExit;
            result = result && QProcess::execute("/usr/sbin/networksetup -setwebproxystate " + service + " off") == QProcess::NormalExit;
            result = result && QProcess::execute("/usr/sbin/networksetup -setsecurewebproxystate " + service + " off") == QProcess::NormalExit;
            result = result && QProcess::execute("/usr/sbin/networksetup -setsocksfirewallproxystate " + service + " off") == QProcess::NormalExit;
        }

        return result;
#endif
    }
}
