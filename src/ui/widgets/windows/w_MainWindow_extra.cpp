#include "components/proxy/QvProxyConfigurator.hpp"
#include "ui/widgets/Qv2rayWidgetApplication.hpp"
#include "ui/widgets/common/WidgetUIBase.hpp"
#include "utils/QvHelpers.hpp"
#include "w_MainWindow.hpp"

void MainWindow::MWSetSystemProxy()
{
    const auto inboundInfo = KernelInstance->GetCurrentConnectionInboundInfo();

    ProtocolSettingsInfoObject httpInboundInfo;
    ProtocolSettingsInfoObject socksInboundInfo;

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
        proxyAddress = httpEnabled ? httpInboundInfo.address : socksInboundInfo.address;
        if (proxyAddress == "0.0.0.0")
            proxyAddress = "127.0.0.1";

        if (proxyAddress == "::")
            proxyAddress = "::1";

        LOG(MODULE_UI, "ProxyAddress: " + proxyAddress);
        SetSystemProxy(proxyAddress, httpInboundInfo.port, socksInboundInfo.port);
        qvAppTrayIcon->setIcon(Q_TRAYICON("tray-systemproxy"));
        if (!GlobalConfig.uiConfig.quietMode)
        {
            QvWidgetApplication->showMessage(tr("System proxy configured."));
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
    qvAppTrayIcon->setIcon(KernelInstance->CurrentConnection().isEmpty() ? Q_TRAYICON("tray") : Q_TRAYICON("tray-connected"));
    if (!GlobalConfig.uiConfig.quietMode)
    {
        QvWidgetApplication->showMessage(tr("System proxy removed."));
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
        const auto result = GlobalConfig.uiConfig.quietMode ? Yes :
                                                              QvMessageBoxAsk(this, tr("Update Subscriptions"),                            //
                                                                              tr("Do you want to update these subscriptions?") + NEWLINE + //
                                                                                  updateNamesList.join(NEWLINE),                           //
                                                                              { Yes, No, Ignore });

        for (const auto &[name, id] : updateList)
        {
            if (result == Yes)
            {
                LOG(MODULE_UI, "Updating subscription: " + name)
                ConnectionManager->UpdateSubscriptionAsync(id);
            }
            else if (result == Ignore)
            {
                LOG(MODULE_UI, "Ignored subscription update: " + name)
                ConnectionManager->IgnoreSubscriptionUpdate(id);
            }
        }
    }
}

void MainWindow::updateColorScheme()
{
    qvAppTrayIcon->setIcon(KernelInstance->CurrentConnection().isEmpty() ? Q_TRAYICON("tray") : Q_TRAYICON("tray-connected"));
    //
    importConfigButton->setIcon(QICON_R("add"));
    updownImageBox->setStyleSheet("image: url(" + QV2RAY_ICON_RESOURCE("netspeed_arrow") + ")");
    updownImageBox_2->setStyleSheet("image: url(" + QV2RAY_ICON_RESOURCE("netspeed_arrow") + ")");
    //
    tray_action_ToggleVisibility->setIcon(this->windowIcon());

    action_RCM_Start->setIcon(QICON_R("start"));
    action_RCM_Edit->setIcon(QICON_R("edit"));
    action_RCM_EditJson->setIcon(QICON_R("code"));
    action_RCM_EditComplex->setIcon(QICON_R("edit"));
    action_RCM_DuplicateConnection->setIcon(QICON_R("copy"));
    action_RCM_DeleteConnection->setIcon(QICON_R("ashbin"));
    action_RCM_ResetStats->setIcon(QICON_R("ashbin"));
    action_RCM_TestLatency->setIcon(QICON_R("ping_gauge"));
    action_RCM_RealLatencyTest->setIcon(QICON_R("ping_gauge"));
    //
    clearChartBtn->setIcon(QICON_R("ashbin"));
    clearlogButton->setIcon(QICON_R("ashbin"));
    //
    locateBtn->setIcon(QICON_R("map"));
    sortBtn->setIcon(QICON_R("arrow-down-filling"));
    collapseGroupsBtn->setIcon(QICON_R("arrow-up"));
}

void MainWindow::UpdateActionTranslations()
{
    tray_SystemProxyMenu->setTitle(tr("System Proxy"));
    tray_RecentConnectionsMenu->setTitle(tr("Recent Connections"));
    tray_ClearRecentConnectionsAction->setText(tr("Clear Recent Connections"));
    //
    tray_action_ToggleVisibility->setText(tr("Hide"));
    tray_action_Preferences->setText(tr("Preferences"));
    tray_action_Quit->setText(tr("Quit"));
    tray_action_Start->setText(tr("Connect"));
    tray_action_Restart->setText(tr("Reconnect"));
    tray_action_Stop->setText(tr("Disconnect"));
    tray_action_SetSystemProxy->setText(tr("Enable System Proxy"));
    tray_action_ClearSystemProxy->setText(tr("Disable System Proxy"));
    //
    action_RCM_Start->setText(tr("Connect to this"));
    action_RCM_SetAutoConnection->setText(tr("Set as automatically connected"));
    action_RCM_EditJson->setText(tr("Edit as JSON"));
    action_RCM_UpdateSubscription->setText(tr("Update Subscription"));
    action_RCM_EditComplex->setText(tr("Edit as Complex Config"));
    action_RCM_RenameConnection->setText(tr("Rename"));
    action_RCM_Edit->setText(tr("Edit"));
    action_RCM_DuplicateConnection->setText(tr("Duplicate to the Same Group"));
    action_RCM_TestLatency->setText(tr("Test Latency"));
    action_RCM_RealLatencyTest->setText(tr("Test Real Latency"));
    action_RCM_ResetStats->setText(tr("Clear Usage Data"));
    action_RCM_DeleteConnection->setText(tr("Delete Connection"));
    //
    sortMenu->setTitle(tr("Sort connection list."));
    sortAction_SortByName_Asc->setText(tr("By connection name, A-Z"));
    sortAction_SortByName_Dsc->setText(tr("By connection name, Z-A"));
    sortAction_SortByPing_Asc->setText(tr("By latency, Ascending"));
    sortAction_SortByPing_Dsc->setText(tr("By latency, Descending"));
    sortAction_SortByData_Asc->setText(tr("By data, Ascending"));
    sortAction_SortByData_Dsc->setText(tr("By data, Descending"));
    //
    action_RCM_SwitchCoreLog->setText(tr("Switch to Core log"));
    action_RCM_SwitchQv2rayLog->setText(tr("Switch to Qv2ray log"));
    //
    action_RCM_CopyGraph->setText(tr("Copy graph as image."));
    action_RCM_CopyRecentLogs->setText(tr("Copy latest logs."));
}
