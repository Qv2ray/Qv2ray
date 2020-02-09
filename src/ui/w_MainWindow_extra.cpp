// Supplementary file for MainWindow -- Basically the handler for connectivity management
// and components interactions.
// We NEED to include the cpp file to define the macros.
#include "w_MainWindow.cpp"
#include "components/proxy/QvProxyConfigurator.hpp"
#include "core/connection/Generation.hpp"

QTreeWidgetItem *MainWindow::FindItemByIdentifier(ConnectionIdentifier identifier)
{
    // First filter out all items with our config name.
    auto items = connectionListWidget->findItems(identifier.connectionName, Qt::MatchExactly | Qt::MatchRecursive);

    for (auto item : items) {
        // This connectable prevents the an item with (which is the parent node of a subscription, having the same
        // -- name as our current connected name)
        if (!IsConnectableItem(item)) {
            LOG(UI, "Invalid Item found: " + item->text(0))
            continue;
        }

        auto thisIdentifier = ItemConnectionIdentifier(item);
        DEBUG(UI, "Item Identifier: " + thisIdentifier.IdentifierString())

        if (identifier == thisIdentifier) {
            return item;
        }
    }

    LOG(UI, "Warning: Failed to find an item named: " + identifier.IdentifierString())
    return nullptr;
}

void MainWindow::MWFindAndStartAutoConfig()
{
    if (!GlobalConfig.autoStartConfig.connectionName.isEmpty()) {
        // User has auto start configured, we try to find that connection item.
        auto name = GlobalConfig.autoStartConfig.subscriptionName.isEmpty()
                    ? GlobalConfig.autoStartConfig.connectionName
                    : GlobalConfig.autoStartConfig.connectionName + " (" + tr("Subscription:") + " " + GlobalConfig.autoStartConfig.subscriptionName + ")";
        //
        LOG(UI, "Found auto start config: " + name)
        auto item = FindItemByIdentifier(GlobalConfig.autoStartConfig);

        if (item != nullptr) {
            // We found the item required and start it.
            connectionListWidget->setCurrentItem(item);
            on_connectionListWidget_currentItemChanged(item, nullptr);
            connectionListWidget->scrollToItem(item);
            tray_RootMenu->actions()[0]->setText(tr("Show"));
            on_startButton_clicked();
        } else {
            QvMessageBoxWarn(this, tr("Autostarting a config"), tr("Could not find a specified config named: ") + NEWLINE +
                             name + NEWLINE + NEWLINE +
                             tr("Please reset the settings in Preference Window"));
        }
    } else if (connectionListWidget->topLevelItemCount() > 0) {
        // Make the first one our default selected item.
        connectionListWidget->setCurrentItem(connectionListWidget->topLevelItem(0));
        ShowAndSetConnection(ItemConnectionIdentifier(connectionListWidget->topLevelItem(0)), true, false);
    }
}

void MainWindow::MWClearSystemProxy(bool showMessage)
{
    ClearSystemProxy();
    LOG(UI, "Clearing System Proxy")
    systemProxyEnabled = false;

    if (showMessage) {
        hTray->showMessage("Qv2ray", tr("System proxy cleared."), windowIcon());
    }
}

void MainWindow::MWSetSystemProxy()
{
    bool usePAC = GlobalConfig.inboundConfig.pacConfig.enablePAC;
    bool pacUseSocks = GlobalConfig.inboundConfig.pacConfig.useSocksProxy;
    bool httpEnabled = GlobalConfig.inboundConfig.useHTTP;
    bool socksEnabled = GlobalConfig.inboundConfig.useSocks;
    //
    // Set system proxy if necessary
    bool isComplex = CheckIsComplexConfig(connections[CurrentConnectionIdentifier].config);

    if (!isComplex) {
        // Is simple config and we will try to set system proxy.
        LOG(UI, "Preparing to set system proxy")
        //
        QString proxyAddress;
        bool canSetSystemProxy = true;

        if (usePAC) {
            if ((httpEnabled && !pacUseSocks) || (socksEnabled && pacUseSocks)) {
                // If we use PAC and socks/http are properly configured for PAC
                LOG(PROXY, "System proxy uses PAC")
                proxyAddress = "http://" + GlobalConfig.inboundConfig.listenip + ":" + QSTRN(GlobalConfig.inboundConfig.pacConfig.port) +  "/pac";
            } else {
                // Not properly configured
                LOG(PROXY, "Failed to process pac due to following reasons:")
                LOG(PROXY, " --> PAC is configured to use socks but socks is not enabled.")
                LOG(PROXY, " --> PAC is configuted to use http but http is not enabled.")
                QvMessageBoxWarn(this, tr("PAC Processing Failed"), tr("HTTP or SOCKS inbound is not properly configured for PAC") +
                                 NEWLINE + tr("Qv2ray will continue, but will not set system proxy."));
                canSetSystemProxy = false;
            }
        } else {
            // Not using PAC
            if (!httpEnabled && !socksEnabled) {
                // Not use PAC, System proxy should use HTTP
                LOG(PROXY, "Setting up system proxy.")
                // A 'proxy host' should be a host WITHOUT `http://` uri scheme
                proxyAddress = "localhost";
            } else {
                LOG(PROXY, "Neither of HTTP nor SOCKS is enabled, cannot set system proxy.")
                QvMessageBoxWarn(this, tr("Cannot set system proxy"), tr("Both HTTP and SOCKS inbounds are not enabled"));
                canSetSystemProxy = false;
            }
        }

        if (canSetSystemProxy) {
            LOG(UI, "Setting system proxy for simple config, HTTP only")
            auto httpPort = GlobalConfig.inboundConfig.useHTTP ? GlobalConfig.inboundConfig.http_port : 0;
            auto socksPort = GlobalConfig.inboundConfig.useSocks ? GlobalConfig.inboundConfig.socks_port : 0;
            //
            // If usePAC is set
            SetSystemProxy(proxyAddress, httpPort, socksPort, usePAC);
            systemProxyEnabled = true;
            hTray->showMessage("Qv2ray", tr("System proxy settings applied."), windowIcon());
        }
    } else {
        hTray->showMessage("Qv2ray", tr("Cannot set proxy for complex config."), windowIcon());
    }
}

bool MainWindow::MWtryStartConnection()
{
    auto connectionRoot = connections[CurrentConnectionIdentifier].config;
    currentFullConfig = GenerateRuntimeConfig(connectionRoot);
    bool startFlag = this->vinstance->StartConnection(currentFullConfig);

    if (startFlag) {
        bool usePAC = GlobalConfig.inboundConfig.pacConfig.enablePAC;
        bool pacUseSocks = GlobalConfig.inboundConfig.pacConfig.useSocksProxy;
        bool httpEnabled = GlobalConfig.inboundConfig.useHTTP;
        bool socksEnabled = GlobalConfig.inboundConfig.useSocks;

        if (usePAC) {
            bool canStartPAC = true;
            QString pacProxyString;  // Something like this --> SOCKS5 127.0.0.1:1080; SOCKS 127.0.0.1:1080; DIRECT; http://proxy:8080
            auto pacIP = GlobalConfig.inboundConfig.pacConfig.localIP;

            if (pacIP.isEmpty()) {
                LOG(PROXY, "PAC Local IP is empty, default to 127.0.0.1")
                pacIP = "127.0.0.1";
            }

            if (pacUseSocks) {
                if (socksEnabled) {
                    pacProxyString = "SOCKS5 " + pacIP + ":" + QSTRN(GlobalConfig.inboundConfig.socks_port);
                } else {
                    LOG(UI, "PAC is using SOCKS, but it is not enabled")
                    QvMessageBoxWarn(this, tr("Configuring PAC"), tr("Could not start PAC server as it is configured to use SOCKS, but it is not enabled"));
                    canStartPAC = false;
                }
            } else {
                if (httpEnabled) {
                    pacProxyString = "PROXY " + pacIP + ":" + QSTRN(GlobalConfig.inboundConfig.http_port);
                } else {
                    LOG(UI, "PAC is using HTTP, but it is not enabled")
                    QvMessageBoxWarn(this, tr("Configuring PAC"), tr("Could not start PAC server as it is configured to use HTTP, but it is not enabled"));
                    canStartPAC = false;
                }
            }

            if (canStartPAC) {
                pacServer.SetProxyString(pacProxyString);
                pacServer.StartListen();
            } else {
                LOG(PROXY, "Not starting PAC due to previous error.")
            }
        }

        if (GlobalConfig.inboundConfig.setSystemProxy) {
            MWSetSystemProxy();
        }
    }

    return startFlag;
}

void MainWindow::MWStopConnection()
{
    if (systemProxyEnabled) {
        MWClearSystemProxy(false);
    }

    this->vinstance->StopConnection();
    QFile(QV2RAY_GENERATED_FILE_PATH).remove();

    if (GlobalConfig.inboundConfig.pacConfig.enablePAC) {
        pacServer.StopServer();
        LOG(UI, "Stopping PAC server")
    }
}

void MainWindow::MWTryPingConnection(const ConnectionIdentifier &alias)
{
    try {
        auto info  = MWGetConnectionInfo(alias);
        QString host = get<0>(info);
        int port = get<1>(info);
        tcpingHelper.StartPing(alias, host, port);
    }  catch (...) {
        QvMessageBoxWarn(this, tr("Latency Test"), tr("Failed to test latency for this connection."));
    }
}

tuple<QString, int, QString> MainWindow::MWGetConnectionInfo(const ConnectionIdentifier &alias)
{
    if (!connections.contains(alias))
        return make_tuple(tr("N/A"), 0, tr("N/A"));

    return GetConnectionInfo(connections[alias].config);
}

void MainWindow::CheckSubscriptionsUpdate()
{
    QStringList updateList;

    for (auto index = 0; index < GlobalConfig.subscriptions.count(); index++) {
        auto subs = GlobalConfig.subscriptions.values()[index];
        auto key = GlobalConfig.subscriptions.keys()[index];
        //
        auto lastRenewDate = QDateTime::fromTime_t(subs.lastUpdated);
        auto renewTime = lastRenewDate.addSecs(subs.updateInterval * 86400);
        LOG(SUBSCRIPTION, "Subscription \"" + key + "\": " + NEWLINE +
            " --> Last renewal time: "  + lastRenewDate.toString() + NEWLINE +
            " --> Renew interval: " + QSTRN(subs.updateInterval) + NEWLINE +
            " --> Ideal renew time: " + renewTime.toString())

        if (renewTime <= QDateTime::currentDateTime()) {
            LOG(SUBSCRIPTION, "Subscription: " + key + " needs to be updated.")
            updateList.append(key);
        }
    }

    if (!updateList.isEmpty()) {
        QvMessageBoxWarn(this, tr("Update Subscriptions"),
                         tr("There are subscriptions need to be updated, please go to subscriptions window to update them.") + NEWLINE + NEWLINE +
                         tr("These subscriptions are out-of-date: ") + NEWLINE + updateList.join(";"));
        on_subsButton_clicked();
    }
}

