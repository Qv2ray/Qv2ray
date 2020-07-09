#include "common/QvHelpers.hpp"
#include "components/proxy/QvProxyConfigurator.hpp"
#include "src/Qv2rayApplication.hpp"
#include "w_MainWindow.hpp"

void MainWindow::MWSetSystemProxy()
{
    const auto inboundInfo = KernelInstance->GetInboundInfo();

    InboundInfoObject httpInboundInfo;
    InboundInfoObject socksInboundInfo;

    for (const auto &info : inboundInfo)
    {
        if (info.protocol == "http")
            httpInboundInfo = info;
        if (info.protocol == "socks")
            socksInboundInfo = info;
    }

    const bool httpEnabled = httpInboundInfo.port > 0;
    const bool socksEnabled = socksInboundInfo.port > 0;
    QString proxyAddress;

    if (httpEnabled || socksEnabled)
    {
        proxyAddress = httpEnabled ? httpInboundInfo.listenIp : socksInboundInfo.listenIp;
        if (proxyAddress == "0.0.0.0")
            proxyAddress = "127.0.0.1";

        if (proxyAddress == "::")
            proxyAddress = "::1";

        LOG(MODULE_UI, "ProxyAddress: " + proxyAddress);
        SetSystemProxy(proxyAddress, httpInboundInfo.port, socksInboundInfo.port);
        qvAppTrayIcon->setIcon(Q_TRAYICON("tray-systemproxy.png"));
        if (!GlobalConfig.uiConfig.quietMode)
        {
            qvApp->showMessage(tr("System proxy configured."));
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
    qvAppTrayIcon->setIcon(KernelInstance->CurrentConnection().isEmpty() ? Q_TRAYICON("tray.png") : Q_TRAYICON("tray-connected.png"));
    if (!GlobalConfig.uiConfig.quietMode)
    {
        qvApp->showMessage(tr("System proxy removed."));
    }
}

bool MainWindow::StartAutoConnectionEntry()
{
    if (StartupOption.noAutoConnection)
        return false;
    switch (GlobalConfig.autoStartBehavior)
    {
        case AUTO_CONNECTION_NONE: return false;
        case AUTO_CONNECTION_FIXED: return ConnectionManager->StartConnection(GlobalConfig.autoStartId);
        case AUTO_CONNECTION_LAST_CONNECTED: return ConnectionManager->StartConnection(GlobalConfig.lastConnectedId);
    }
    Q_UNREACHABLE();
}

void MainWindow::CheckSubscriptionsUpdate()
{
    QList<QPair<QString, GroupId>> updateList;
    QStringList updateNamesList;

    for (const auto &entry : ConnectionManager->Subscriptions())
    {
        const auto info = ConnectionManager->GetGroupMetaObject(entry);
        //
        // The update is ignored.
        if (info.subscriptionOption.updateInterval == 0)
            continue;
        //
        const auto lastRenewDate = QDateTime::fromTime_t(info.lastUpdatedDate);
        const auto renewTime = lastRenewDate.addSecs(info.subscriptionOption.updateInterval * 86400);

        if (renewTime <= QDateTime::currentDateTime())
        {
            updateList << QPair{ info.displayName, entry };
            updateNamesList << info.displayName;
            LOG(MODULE_SUBSCRIPTION, QString("Subscription update \"%1\": L=%2 R=%3 I=%4")
                                         .arg(info.displayName)
                                         .arg(lastRenewDate.toString())
                                         .arg(QSTRN(info.subscriptionOption.updateInterval))
                                         .arg(renewTime.toString()))
        }
    }

    if (!updateList.isEmpty())
    {
        const auto result = GlobalConfig.uiConfig.quietMode ? QMessageBox::Yes :
                                                              QvMessageBoxAsk(this, tr("Update Subscriptions"),                            //
                                                                              tr("Do you want to update these subscriptions?") + NEWLINE + //
                                                                                  updateNamesList.join(NEWLINE),                           //
                                                                              QMessageBox::Ignore);

        for (const auto &[name, id] : updateList)
        {
            if (result == QMessageBox::Yes)
            {
                LOG(MODULE_UI, "Updating subscription: " + name)
                ConnectionManager->UpdateSubscriptionAsync(id);
            }
            else if (result == QMessageBox::Ignore)
            {
                LOG(MODULE_UI, "Ignored subscription update: " + name)
                ConnectionManager->IgnoreSubscriptionUpdate(id);
            }
        }
    }
}
