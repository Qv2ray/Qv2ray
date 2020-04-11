#include "common/QvHelpers.hpp"
#include "components/proxy/QvProxyConfigurator.hpp"
#include "w_MainWindow.hpp"

void MainWindow::MWSetSystemProxy()
{
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

        // Not using PAC
        if (httpEnabled || socksEnabled)
        {
            // Not use PAC, System proxy should use HTTP or SOCKS
            LOG(MODULE_PROXY, "Setting up system proxy.")
            // A 'proxy host' should be a host WITHOUT `http://` uri scheme
            proxyAddress = "127.0.0.1";
        }
        else
        {
            LOG(MODULE_PROXY, "Neither of HTTP nor SOCKS is enabled, cannot set system proxy.")
            QvMessageBoxWarn(this, tr("Cannot set system proxy"), tr("Both HTTP and SOCKS inbounds are not enabled"));
            canSetSystemProxy = false;
        }

        if (canSetSystemProxy)
        {
            LOG(MODULE_UI, "Setting system proxy for simple config.")
            auto httpPort = GlobalConfig.inboundConfig.useHTTP ? GlobalConfig.inboundConfig.http_port : 0;
            auto socksPort = GlobalConfig.inboundConfig.useSocks ? GlobalConfig.inboundConfig.socks_port : 0;
            //
            SetSystemProxy(proxyAddress, httpPort, socksPort);
            hTray.showMessage("Qv2ray", tr("System proxy configured."));
        }
    }
    else
    {
        hTray.showMessage("Qv2ray", tr("Didn't set proxy for complex config."), windowIcon());
    }
}

void MainWindow::MWClearSystemProxy()
{
    ClearSystemProxy();
    hTray.showMessage("Qv2ray", tr("System proxy removed."));
}

void MainWindow::CheckSubscriptionsUpdate()
{
    QStringList updateList;

    auto subscriptions = ConnectionManager->Subscriptions();
    for (auto entry : subscriptions)
    {
        const auto info = ConnectionManager->GetGroupMetaObject(entry);
        //
        // The update is ignored.
        if (info.updateInterval == 0)
            continue;
        //
        auto lastRenewDate = QDateTime::fromTime_t(info.lastUpdated);
        auto renewTime = lastRenewDate.addSecs(info.updateInterval * 86400);
        LOG(MODULE_SUBSCRIPTION,                                                  //
            "Subscription \"" + info.displayName + "\": " +                       //
                NEWLINE + " --> Last renewal time: " + lastRenewDate.toString() + //
                NEWLINE + " --> Renew interval: " + QSTRN(info.updateInterval) +  //
                NEWLINE + " --> Ideal renew time: " + renewTime.toString())       //

        if (renewTime <= QDateTime::currentDateTime())
        {
            LOG(MODULE_SUBSCRIPTION, "Subscription: " + info.displayName + " needs to be updated.")
            updateList.append(entry.toString());
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
