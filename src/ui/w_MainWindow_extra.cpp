#include "common/QvHelpers.hpp"
#include "components/proxy/QvProxyConfigurator.hpp"
#include "w_MainWindow.hpp"

void MainWindow::MWSetSystemProxy()
{
    auto inboundPorts = KernelInstance->InboundPorts();
    bool httpEnabled = inboundPorts.contains("http");
    bool socksEnabled = inboundPorts.contains("socks");
    auto httpPort = inboundPorts["http"];
    auto socksPort = inboundPorts["socks"];

    QString proxyAddress;

    if (httpEnabled || socksEnabled)
    {
        proxyAddress = "localhost";
        SetSystemProxy(proxyAddress, httpPort, socksPort);
        hTray.setIcon(Q_TRAYICON("tray-systemproxy.png"));
        if (!GlobalConfig.uiConfig.quietMode)
        {
            hTray.showMessage("Qv2ray", tr("System proxy configured."));
        }
    }
    else
    {
        LOG(MODULE_PROXY, "Neither of HTTP nor SOCKS is enabled, cannot set system proxy.")
        QvMessageBoxWarn(this, tr("Cannot set system proxy"), tr("Both HTTP and SOCKS inbounds are not enabled"));
    }
}

void MainWindow::MWClearSystemProxy()
{
    ClearSystemProxy();
    hTray.setIcon(KernelInstance->CurrentConnection().isEmpty() ? Q_TRAYICON("tray.png") : Q_TRAYICON("tray-connected.png"));
    if (!GlobalConfig.uiConfig.quietMode)
    {
        hTray.showMessage("Qv2ray", tr("System proxy removed."));
    }
}

void MainWindow::CheckSubscriptionsUpdate()
{
    QStringList updateList;

    auto subscriptions = ConnectionManager->Subscriptions();
    for (const auto &entry : subscriptions)
    {
        const auto info = ConnectionManager->GetGroupMetaObject(entry);
        //
        // The update is ignored.
        if (info.subscriptionSettings.updateInterval == 0)
            continue;
        //
        const auto &lastRenewDate = QDateTime::fromTime_t(info.lastUpdatedDate);
        const auto &renewTime = lastRenewDate.addSecs(info.subscriptionSettings.updateInterval * 86400);
        LOG(MODULE_SUBSCRIPTION,                                                                      //
            "Subscription \"" + info.displayName + "\": " +                                           //
                NEWLINE + " --> Last renewal time: " + lastRenewDate.toString() +                     //
                NEWLINE + " --> Renew interval: " + QSTRN(info.subscriptionSettings.updateInterval) + //
                NEWLINE + " --> Ideal renew time: " + renewTime.toString())                           //

        if (renewTime <= QDateTime::currentDateTime())
        {
            LOG(MODULE_SUBSCRIPTION, "Subscription: " + info.displayName + " needs to be updated.")
            updateList.append(info.displayName);
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
