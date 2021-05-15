#include "QvProxyConfigurator.hpp"

#include "base/Qv2rayBase.hpp"
#include "components/plugins/QvPluginHost.hpp"
#include "utils/QvHelpers.hpp"
#ifdef Q_OS_WIN
//
#include <Windows.h>
//
#include <WinInet.h>
#include <ras.h>
#include <raserror.h>
#endif

#define QV_MODULE_NAME "SystemProxy"

namespace Qv2ray::components::proxy
{

    using ProcessArgument = QPair<QString, QStringList>;
#ifdef Q_OS_MACOS
    QStringList macOSgetNetworkServices()
    {
        QProcess p;
        p.setProgram("/usr/sbin/networksetup");
        p.setArguments(QStringList{ "-listallnetworkservices" });
        p.start();
        p.waitForStarted();
        p.waitForFinished();
        LOG(p.errorString());
        auto str = p.readAllStandardOutput();
        auto lines = SplitLines(str);
        QStringList result;

        // Start from 1 since first line is unneeded.
        for (auto i = 1; i < lines.count(); i++)
        {
            // * means disabled.
            if (!lines[i].contains("*"))
            {
                result << lines[i];
            }
        }

        LOG("Found " + QSTRN(result.size()) + " network services: " + result.join(";"));
        return result;
    }
#endif
#ifdef Q_OS_WIN
    bool __SetProxyOptions(LPWSTR proxy_full_addr)
    {
        INTERNET_PER_CONN_OPTION_LIST list;
        DWORD dwBufSize = sizeof(list);
        // Fill the list structure.
        list.dwSize = sizeof(list);
        // NULL == LAN, otherwise connectoid name.
        list.pszConnection = nullptr;

        if (nullptr == proxy_full_addr)
        {
            list.dwOptionCount = 1;
            list.pOptions = new INTERNET_PER_CONN_OPTION[1];

            // Ensure that the memory was allocated.
            if (nullptr == list.pOptions)
            {
                // Return if the memory wasn't allocated.
                return false;
            }

            // Set flags.
            list.pOptions[0].dwOption = INTERNET_PER_CONN_FLAGS;
            list.pOptions[0].Value.dwValue = PROXY_TYPE_DIRECT;
        }
        else
        {
            list.dwOptionCount = 2;
            list.pOptions = new INTERNET_PER_CONN_OPTION[2];

            if (nullptr == list.pOptions)
            {
                return false;
            }

            // Set flags.
            list.pOptions[0].dwOption = INTERNET_PER_CONN_FLAGS;
            list.pOptions[0].Value.dwValue = PROXY_TYPE_DIRECT | PROXY_TYPE_PROXY;
            // Set proxy name.
            list.pOptions[1].dwOption = INTERNET_PER_CONN_PROXY_SERVER;
            list.pOptions[1].Value.pszValue = proxy_full_addr;
            // Set proxy override.
            // list.pOptions[2].dwOption = INTERNET_PER_CONN_PROXY_BYPASS;
            // auto localhost = L"localhost";
            // list.pOptions[2].Value.pszValue = NO_CONST(localhost);
        }

        DWORD dwCb = 0;
        DWORD dwRet = ERROR_SUCCESS;
        DWORD dwEntries = 0;
        LPRASENTRYNAME lpRasEntryName = nullptr;
        // Call RasEnumEntries with lpRasEntryName = NULL. dwCb is returned with the required buffer size and
        // a return code of ERROR_BUFFER_TOO_SMALL
        dwRet = RasEnumEntries(nullptr, nullptr, lpRasEntryName, &dwCb, &dwEntries);
        if (ERROR_BUFFER_TOO_SMALL == dwRet)
        {
            // Allocate the memory needed for the array of RAS entry names.
            lpRasEntryName = (LPRASENTRYNAME) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwCb);
            if (nullptr == lpRasEntryName)
            {
                LOG("HeapAlloc failed");
                delete[] list.pOptions;
                return false;
            }
            // The first RASENTRYNAME structure in the array must contain the structure size
            lpRasEntryName[0].dwSize = sizeof(RASENTRYNAME);
            // Call RasEnumEntries to enumerate all RAS entry names
            if (ERROR_SUCCESS != RasEnumEntries(nullptr, nullptr, lpRasEntryName, &dwCb, &dwEntries))
            {
                LOG("RasEnumEntries failed");
                HeapFree(GetProcessHeap(), 0, lpRasEntryName);
                lpRasEntryName = nullptr;
                delete[] list.pOptions;
                return false;
            }
            // Set proxy for each connectoid
            // help wanted: RAS entry names with non-ASCII characters
            for (DWORD i = 0; i < dwEntries; ++i)
            {
                list.pszConnection = lpRasEntryName[i].szEntryName;
                if (!InternetSetOption(nullptr, INTERNET_OPTION_PER_CONNECTION_OPTION, &list, dwBufSize))
                {
                    LOG("InternetSetOption failed for connectoid " + QString::fromWCharArray(list.pszConnection) + ", GLE=" + QSTRN(GetLastError()));
                }
            }
            // Deallocate memory for the connection buffer
            HeapFree(GetProcessHeap(), 0, lpRasEntryName);
            lpRasEntryName = nullptr;
        }

        // Set proxy for LAN.
        if (!InternetSetOption(nullptr, INTERNET_OPTION_PER_CONNECTION_OPTION, &list, dwBufSize))
        {
            LOG("InternetSetOption failed for LAN, GLE=" + QSTRN(GetLastError()));
        }

        delete[] list.pOptions;
        InternetSetOption(nullptr, INTERNET_OPTION_SETTINGS_CHANGED, nullptr, 0);
        InternetSetOption(nullptr, INTERNET_OPTION_REFRESH, nullptr, 0);
        return true;
    }
#endif

    void SetSystemProxy(const QString &address, int httpPort, int socksPort)
    {
        LOG("Setting up System Proxy");
        bool hasHTTP = !address.isEmpty() && httpPort > 0 && httpPort < 65536;
        bool hasSOCKS = !address.isEmpty() && socksPort > 0 && socksPort < 65536;

#ifdef Q_OS_WIN
        if (!hasHTTP)
        {
            LOG("Nothing?");
            return;
        }
        else
        {
            LOG("Qv2ray will set system proxy to use HTTP");
        }
#else
        if (!hasHTTP && !hasSOCKS)
        {
            LOG("Nothing?");
            return;
        }

        if (hasHTTP)
        {
            LOG("Qv2ray will set system proxy to use HTTP");
        }

        if (hasSOCKS)
        {
            LOG("Qv2ray will set system proxy to use SOCKS");
        }
#endif

#ifdef Q_OS_WIN
        QString __a;
        const QHostAddress ha(address);
        const auto type = ha.protocol();
        if (type == QAbstractSocket::IPv6Protocol)
        {
            __a = "[" + ha.toString() + "]:" + QSTRN(httpPort);
        }
        else
        {
             __a = address + ":" + QSTRN(httpPort);
        }

        LOG("Windows proxy string: " + __a);
        auto proxyStrW = new WCHAR[__a.length() + 1];
        wcscpy(proxyStrW, __a.toStdWString().c_str());
        //

        if (!__SetProxyOptions(proxyStrW))
        {
            LOG("Failed to set proxy.");
        }
        // help wanted: Administrator privileges required
        // else
        // {
        //     QProcess::execute("netsh", { "winhttp", "import", "proxy", "source=ie" });
        // }

#elif defined(Q_OS_LINUX)
        QList<ProcessArgument> actions;
        //
        bool isKDE = qEnvironmentVariable("XDG_SESSION_DESKTOP") == "KDE" || qEnvironmentVariable("XDG_SESSION_DESKTOP") == "plasma";
        const auto configPath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);

        //
        // Configure HTTP Proxies for HTTP, FTP and HTTPS
        if (hasHTTP)
        {
            // iterate over protocols...
            for (const auto &protocol : QStringList{ "http", "ftp", "https" })
            {
                // for GNOME:
                {
                    actions << ProcessArgument{ "gsettings", { "set", "org.gnome.system.proxy." + protocol, "host", address } };
                    actions << ProcessArgument{ "gsettings", { "set", "org.gnome.system.proxy." + protocol, "port", QSTRN(httpPort) } };
                }

                // for KDE:
                if (isKDE)
                {
                    actions << ProcessArgument{ "kwriteconfig5",
                                                { "--file", configPath + "/kioslaverc", //
                                                  "--group", "Proxy Settings",          //
                                                  "--key", protocol + "Proxy",          //
                                                  "http://" + address + " " + QSTRN(httpPort) } };
                }
            }
        }

        // Configure SOCKS5 Proxies
        if (hasSOCKS)
        {
            // for GNOME:
            {
                actions << ProcessArgument{ "gsettings", { "set", "org.gnome.system.proxy.socks", "host", address } };
                actions << ProcessArgument{ "gsettings", { "set", "org.gnome.system.proxy.socks", "port", QSTRN(socksPort) } };

                // for KDE:
                if (isKDE)
                {
                    actions << ProcessArgument{ "kwriteconfig5",
                                                { "--file", configPath + "/kioslaverc", //
                                                  "--group", "Proxy Settings",          //
                                                  "--key", "socksProxy",                //
                                                  "socks://" + address + " " + QSTRN(socksPort) } };
                }
            }
        }
        // Setting Proxy Mode to Manual
        {
            // for GNOME:
            {
                actions << ProcessArgument{ "gsettings", { "set", "org.gnome.system.proxy", "mode", "manual" } };
            }

            // for KDE:
            if (isKDE)
            {
                actions << ProcessArgument{ "kwriteconfig5",
                                            { "--file", configPath + "/kioslaverc", //
                                              "--group", "Proxy Settings",          //
                                              "--key", "ProxyType", "1" } };
            }
        }

        // Notify kioslaves to reload system proxy configuration.
        if (isKDE)
        {
            actions << ProcessArgument{ "dbus-send",
                                        { "--type=signal", "/KIO/Scheduler",                 //
                                          "org.kde.KIO.Scheduler.reparseSlaveConfiguration", //
                                          "string:''" } };
        }
        // Execute them all!
        //
        // note: do not use std::all_of / any_of / none_of,
        // because those are short-circuit and cannot guarantee atomicity.
        QList<bool> results;
        for (const auto &action : actions)
        {
            // execute and get the code
            const auto returnCode = QProcess::execute(action.first, action.second);
            // print out the commands and result codes
            DEBUG(QString("[%1] Program: %2, Args: %3").arg(returnCode).arg(action.first).arg(action.second.join(";")));
            // give the code back
            results << (returnCode == QProcess::NormalExit);
        }

        if (results.count(true) != actions.size())
        {
            LOG("Something wrong when setting proxies.");
        }
#elif defined(Q_OS_MACOS)

        for (const auto &service : macOSgetNetworkServices())
        {
            LOG("Setting proxy for interface: " + service);
            if (hasHTTP)
            {

                QProcess::execute("/usr/sbin/networksetup", { "-setwebproxy", service, address, QSTRN(httpPort) });
                QProcess::execute("/usr/sbin/networksetup", { "-setsecurewebproxy", service, address, QSTRN(httpPort) });
                QProcess::execute("/usr/sbin/networksetup", { "-setwebproxystate", service, "on" });
                QProcess::execute("/usr/sbin/networksetup", { "-setsecurewebproxystate", service, "on" });
            }

            if (hasSOCKS)
            {
                QProcess::execute("/usr/sbin/networksetup", { "-setsocksfirewallproxy", service, address, QSTRN(socksPort) });
                QProcess::execute("/usr/sbin/networksetup", { "-setsocksfirewallproxystate", service, "on" });
            }
        }

#endif
        //
        // Trigger plugin events
        QMap<Events::SystemProxy::SystemProxyType, int> portSettings;
        if (hasHTTP)
            portSettings.insert(Events::SystemProxy::SystemProxyType::SystemProxy_HTTP, httpPort);
        if (hasSOCKS)
            portSettings.insert(Events::SystemProxy::SystemProxyType::SystemProxy_SOCKS, socksPort);
        PluginHost->SendEvent({ portSettings, Events::SystemProxy::SystemProxyStateType::SetProxy });
    }

    void ClearSystemProxy()
    {
        LOG("Clearing System Proxy");

#ifdef Q_OS_WIN
        if (!__SetProxyOptions(nullptr))
        {
            LOG("Failed to clear proxy.");
        }
        // help wanted: Administrator privileges required
        // else
        // {
        //     QProcess::execute("netsh", { "winhttp", "reset", "proxy" });
        // }
#elif defined(Q_OS_LINUX)
        QList<ProcessArgument> actions;
        const bool isKDE = qEnvironmentVariable("XDG_SESSION_DESKTOP") == "KDE" || qEnvironmentVariable("XDG_SESSION_DESKTOP") == "plasma";
        const auto configRoot = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);

        // Setting System Proxy Mode to: None
        {
            // for GNOME:
            {
                actions << ProcessArgument{ "gsettings", { "set", "org.gnome.system.proxy", "mode", "none" } };
            }

            // for KDE:
            if (isKDE)
            {
                actions << ProcessArgument{ "kwriteconfig5",
                                            { "--file", configRoot + "/kioslaverc", //
                                              "--group", "Proxy Settings",          //
                                              "--key", "ProxyType", "0" } };
            }
        }

        // Notify kioslaves to reload system proxy configuration.
        if (isKDE)
        {
            actions << ProcessArgument{ "dbus-send",
                                        { "--type=signal", "/KIO/Scheduler",                 //
                                          "org.kde.KIO.Scheduler.reparseSlaveConfiguration", //
                                          "string:''" } };
        }

        // Execute the Actions
        for (const auto &action : actions)
        {
            // execute and get the code
            const auto returnCode = QProcess::execute(action.first, action.second);
            // print out the commands and result codes
            DEBUG(QString("[%1] Program: %2, Args: %3").arg(returnCode).arg(action.first).arg(action.second.join(";")));
        }

#elif defined(Q_OS_MACOS)
        for (const auto &service : macOSgetNetworkServices())
        {
            LOG("Clearing proxy for interface: " + service);
            QProcess::execute("/usr/sbin/networksetup", { "-setautoproxystate", service, "off" });
            QProcess::execute("/usr/sbin/networksetup", { "-setwebproxystate", service, "off" });
            QProcess::execute("/usr/sbin/networksetup", { "-setsecurewebproxystate", service, "off" });
            QProcess::execute("/usr/sbin/networksetup", { "-setsocksfirewallproxystate", service, "off" });
        }

#endif
        //
        // Trigger plugin events
        PluginHost->SendEvent(Events::SystemProxy::EventObject{ {}, Events::SystemProxy::SystemProxyStateType::ClearProxy });
    }
} // namespace Qv2ray::components::proxy
