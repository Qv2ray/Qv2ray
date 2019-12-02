#include "QvSystemProxyConfigurator.hpp"
#ifdef Q_OS_WIN
#include "wininet.h"
#include <windows.h>
#endif

namespace Qv2ray
{
    namespace Components
    {

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

            if (!InternetQueryOption(nullptr, INTERNET_OPTION_PER_CONNECTION_OPTION, &List, &nSize))
                LOG(MODULE_PROXY, "InternetQueryOption failed, GLE=" + to_string(GetLastError()));

            LOG(MODULE_PROXY, "System default proxy info:")

            if (Option[0].Value.pszValue != nullptr)
                LOG(MODULE_PROXY, QString::fromWCharArray(Option[0].Value.pszValue).toStdString());

            if ((Option[2].Value.dwValue & PROXY_TYPE_AUTO_PROXY_URL) == PROXY_TYPE_AUTO_PROXY_URL)
                LOG(MODULE_PROXY, "PROXY_TYPE_AUTO_PROXY_URL");

            if ((Option[2].Value.dwValue & PROXY_TYPE_AUTO_DETECT) == PROXY_TYPE_AUTO_DETECT)
                LOG(MODULE_PROXY, "PROXY_TYPE_AUTO_DETECT");

            if ((Option[2].Value.dwValue & PROXY_TYPE_DIRECT) == PROXY_TYPE_DIRECT)
                LOG(MODULE_PROXY, "PROXY_TYPE_DIRECT");

            if ((Option[2].Value.dwValue & PROXY_TYPE_PROXY) == PROXY_TYPE_PROXY)
                LOG(MODULE_PROXY, "PROXY_TYPE_PROXY");

            if (!InternetQueryOption(nullptr, INTERNET_OPTION_PER_CONNECTION_OPTION, &List, &nSize))
                LOG(MODULE_PROXY, "InternetQueryOption failed,GLE=" + to_string(GetLastError()));

            if (Option[4].Value.pszValue != nullptr)
                LOG(MODULE_PROXY, Option[4].Value.pszValue);

            INTERNET_VERSION_INFO Version;
            nSize = sizeof(INTERNET_VERSION_INFO);
            InternetQueryOption(nullptr, INTERNET_OPTION_VERSION, &Version, &nSize);

            if (Option[0].Value.pszValue != nullptr)
                GlobalFree(Option[0].Value.pszValue);

            if (Option[3].Value.pszValue != nullptr)
                GlobalFree(Option[3].Value.pszValue);

            if (Option[4].Value.pszValue != nullptr)
                GlobalFree(Option[4].Value.pszValue);

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
                LOG(MODULE_PROXY, "Setting system proxy for PAC")
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
                LOG(MODULE_PROXY, "Setting system proxy for Global Proxy")
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

        bool SetSystemProxy(const QString &address, int port, bool usePAC)
        {
#ifdef Q_OS_WIN
            QString __a;

            if (usePAC) {
                __a = address;
            } else {
                __a = "http://" + address + ":" + QString::number(port);
            }

            auto proxyStrW = new WCHAR[__a.length() + 1];
            wcscpy(proxyStrW, __a.toStdWString().c_str());
            //
            __QueryProxyOptions();

            if (!__SetProxyOptions(proxyStrW, usePAC)) {
                LOG(MODULE_PROXY, "Failed to set proxy.")
                return false;
            }

            __QueryProxyOptions();
            return true;
#elif defined(Q_OS_LINUX)
            bool result = true;

            if (usePAC) {
                result = result && QProcess::execute("gsettings set org.gnome.system.proxy mode 'auto'") == QProcess::NormalExit;
                result = result && QProcess::execute("gsettings set org.gnome.system.proxy autoconfig-url '" + address + "'") == QProcess::NormalExit;
            } else {
                result = result && QProcess::execute("gsettings set org.gnome.system.proxy mode 'manual'") == QProcess::NormalExit;
                result = result && QProcess::execute("gsettings set org.gnome.system.proxy.http host '" + address + "'") == QProcess::NormalExit;
                result = result && QProcess::execute("gsettings set org.gnome.system.proxy.http port " + QString::number(port)) == QProcess::NormalExit;
            }

            if (!result) {
                LOG(MODULE_PROXY, "Something wrong happens when setting system proxy -> Gnome ONLY.")
                LOG(MODULE_PROXY, "If you are using KDE Plasma and receiving this message, just simply ignore this.")
            }

            return result;
#else
            Q_UNUSED(port)
            Q_UNUSED(address)
            Q_UNUSED(usePAC)
            return false;
#endif
        }

        bool ClearSystemProxy()
        {
#ifdef Q_OS_WIN
            LOG(MODULE_PROXY, "Cleaning system proxy settings.")
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
                LOG(MODULE_PROXY, "Failed to allocat memory in DisableConnectionProxy()");
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
            return false;
#endif
        }
    }
}
