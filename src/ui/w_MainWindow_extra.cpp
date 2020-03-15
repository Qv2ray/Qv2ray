#include "common/QvHelpers.hpp"
#include "components/proxy/QvProxyConfigurator.hpp"
#include "w_MainWindow.hpp"

void MainWindow::MWSetSystemProxy()
{
    bool usePAC = GlobalConfig.inboundConfig.pacConfig.enablePAC;
    bool pacUseSocks = GlobalConfig.inboundConfig.pacConfig.useSocksProxy;
    bool httpEnabled = GlobalConfig.inboundConfig.useHTTP;
    bool socksEnabled = GlobalConfig.inboundConfig.useSocks;
    //
    bool isComplex = IsComplexConfig(ConnectionManager->CurrentConnection());

    if (!isComplex)
    {
        // Is simple config and we will try to set system proxy.
        LOG(MODULE_UI, "Preparing to set system proxy")
        //
        QString proxyAddress;
        bool canSetSystemProxy = true;

        if (usePAC)
        {
            if ((httpEnabled && !pacUseSocks) || (socksEnabled && pacUseSocks))
            {
                // If we use PAC and socks/http are properly configured for PAC
                LOG(MODULE_PROXY, "System proxy uses PAC")
                proxyAddress = "http://" + GlobalConfig.inboundConfig.listenip + ":" + QSTRN(GlobalConfig.inboundConfig.pacConfig.port) + "/pac";
            }
            else
            {
                // Not properly configured
                LOG(MODULE_PROXY, "Failed to process pac due to following reasons:")
                LOG(MODULE_PROXY, " --> PAC is configured to use socks but socks is not enabled.")
                LOG(MODULE_PROXY, " --> PAC is configuted to use http but http is not enabled.")
                QvMessageBoxWarn(this, tr("PAC Processing Failed"),
                                 tr("HTTP or SOCKS inbound is not properly configured for PAC") + NEWLINE +
                                     tr("Qv2ray will continue, but will not set system proxy."));
                canSetSystemProxy = false;
            }
        }
        else
        {
            // Not using PAC
            if (httpEnabled || socksEnabled)
            {
                // Not use PAC, System proxy should use HTTP or SOCKS
                LOG(MODULE_PROXY, "Setting up system proxy.")
                // A 'proxy host' should be a host WITHOUT `http://` uri scheme
                proxyAddress = "localhost";
            }
            else
            {
                LOG(MODULE_PROXY, "Neither of HTTP nor SOCKS is enabled, cannot set system proxy.")
                QvMessageBoxWarn(this, tr("Cannot set system proxy"), tr("Both HTTP and SOCKS inbounds are not enabled"));
                canSetSystemProxy = false;
            }
        }

        if (canSetSystemProxy)
        {
            LOG(MODULE_UI, "Setting system proxy for simple config.")
            auto httpPort = GlobalConfig.inboundConfig.useHTTP ? GlobalConfig.inboundConfig.http_port : 0;
            auto socksPort = GlobalConfig.inboundConfig.useSocks ? GlobalConfig.inboundConfig.socks_port : 0;
            //
            SetSystemProxy(proxyAddress, httpPort, socksPort, usePAC);
        }
    }
    else
    {
        hTray.showMessage("Qv2ray", tr("Didn't set proxy for complex config."), windowIcon());
    }
}

void MainWindow::CheckSubscriptionsUpdate()
{
    QStringList updateList;

    for (auto index = 0; index < GlobalConfig.subscriptions.count(); index++)
    {
        auto subs = GlobalConfig.subscriptions.values().at(index);
        auto key = GlobalConfig.subscriptions.keys().at(index);
        //
        auto lastRenewDate = QDateTime::fromTime_t(subs.lastUpdated);
        auto renewTime = lastRenewDate.addSecs(subs.updateInterval * 86400);
        LOG(MODULE_SUBSCRIPTION, "Subscription \"" + key + "\": " + NEWLINE + " --> Last renewal time: " + lastRenewDate.toString() + NEWLINE +
                                     " --> Renew interval: " + QSTRN(subs.updateInterval) + NEWLINE +
                                     " --> Ideal renew time: " + renewTime.toString())

        if (renewTime <= QDateTime::currentDateTime())
        {
            LOG(MODULE_SUBSCRIPTION, "Subscription: " + key + " needs to be updated.")
            updateList.append(key);
        }
    }

    if (!updateList.isEmpty())
    {
        QvMessageBoxWarn(this, tr("Update Subscriptions"),
                         tr("There are subscriptions need to be updated, please go to subscriptions window to update them.") + NEWLINE +
                             NEWLINE + tr("These subscriptions are out-of-date: ") + NEWLINE + updateList.join(";"));
        on_subsButton_clicked();
    }
}
