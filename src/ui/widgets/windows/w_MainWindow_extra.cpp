#include "components/proxy/QvProxyConfigurator.hpp"
#include "ui/widgets/Qv2rayWidgetApplication.hpp"
#include "ui/widgets/common/WidgetUIBase.hpp"
#include "utils/QvHelpers.hpp"
#include "w_MainWindow.hpp"

#ifdef Q_OS_MAC
#include <ApplicationServices/ApplicationServices.h>
#endif

#define QV_MODULE_NAME "MainWindowExtra"

void MainWindow::MWToggleVisibilitySetText()
{
    if (isHidden() || isMinimized())
        tray_action_ToggleVisibility->setText(tr("Show"));
    else
        tray_action_ToggleVisibility->setText(tr("Hide"));
}

void MainWindow::MWToggleVisibility()
{
    if (isHidden() || isMinimized())
        MWShowWindow();
    else
        MWHideWindow();
}

void MainWindow::MWShowWindow()
{
#if QV2RAY_FEATURE(ui_has_store_state)
    RestoreState();
#endif
    this->show();
#ifdef Q_OS_WIN
    setWindowState(Qt::WindowNoState);
    SetWindowPos(HWND(this->winId()), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
    QThread::msleep(20);
    SetWindowPos(HWND(this->winId()), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
#endif
#ifdef Q_OS_MAC
    ProcessSerialNumber psn = { 0, kCurrentProcess };
    TransformProcessType(&psn, kProcessTransformToForegroundApplication);
#endif
    MWToggleVisibilitySetText();
}

void MainWindow::MWHideWindow()
{
#ifdef Q_OS_MAC
    ProcessSerialNumber psn = { 0, kCurrentProcess };
    TransformProcessType(&psn, kProcessTransformToUIElementApplication);
#endif
#if QV2RAY_FEATURE(ui_has_store_state)
    SaveState();
#endif
    this->hide();
    MWToggleVisibilitySetText();
}

void MainWindow::MWSetSystemProxy()
{
    const auto inboundInfo = KernelInstance->GetCurrentConnectionInboundInfo();
    bool httpEnabled = false;
    bool socksEnabled = false;
    int httpPort = 0;
    int socksPort = 0;
    QString httpAddress;
    QString socksAddress;

    for (const auto &info : inboundInfo)
    {
        if (info.protocol == "http")
        {
            httpEnabled = true;
            httpPort = info.port;
            httpAddress = info.address;
        }
        else if (info.protocol == "socks")
        {
            socksEnabled = true;
            socksPort = info.port;
            socksAddress = info.address;
        }
    }

    QString proxyAddress;
    if (httpEnabled)
        proxyAddress = httpAddress;
    else if (socksEnabled)
        proxyAddress = socksAddress;

    const QHostAddress ha(proxyAddress);
    if (ha.isEqual(QHostAddress::AnyIPv4)) // "0.0.0.0"
        proxyAddress = "127.0.0.1";
    else if (ha.isEqual(QHostAddress::AnyIPv6)) // "::"
        proxyAddress = "::1";

    if (!proxyAddress.isEmpty())
    {
        LOG("ProxyAddress: " + proxyAddress);
        LOG("HTTP Port: " + QSTRN(httpPort));
        LOG("SOCKS Port: " + QSTRN(socksPort));
        SetSystemProxy(proxyAddress, httpPort, socksPort);
        qvAppTrayIcon->setIcon(Q_TRAYICON("tray-systemproxy"));
        if (!GlobalConfig.uiConfig.quietMode)
        {
            QvWidgetApplication->ShowTrayMessage(tr("System proxy configured."));
        }
    }
    else
    {
        LOG("Neither of HTTP nor SOCKS is enabled, cannot set system proxy.");
        QvMessageBoxWarn(this, tr("Cannot set system proxy"), tr("Both HTTP and SOCKS inbounds are not enabled"));
    }
}

void MainWindow::MWClearSystemProxy()
{
    ClearSystemProxy();
    qvAppTrayIcon->setIcon(KernelInstance->CurrentConnection().isEmpty() ? Q_TRAYICON("tray") : Q_TRAYICON("tray-connected"));
    if (!GlobalConfig.uiConfig.quietMode)
    {
        QvWidgetApplication->ShowTrayMessage(tr("System proxy removed."));
    }
}

bool MainWindow::StartAutoConnectionEntry()
{
    if (QvCoreApplication->StartupArguments.noAutoConnection)
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
    QList<std::pair<QString, GroupId>> updateList;
    QStringList updateNamesList;

    for (const auto &entry : ConnectionManager->Subscriptions())
    {
        const auto info = ConnectionManager->GetGroupMetaObject(entry);
        //
        // The update is ignored.
        if (info.subscriptionOption.updateInterval == 0)
            continue;
        //
        const auto lastRenewDate = QDateTime::fromSecsSinceEpoch(info.lastUpdatedDate);
        const auto renewTime = lastRenewDate.addSecs(info.subscriptionOption.updateInterval * 86400);

        if (renewTime <= QDateTime::currentDateTime())
        {
            updateList << std::pair{ info.displayName, entry };
            updateNamesList << info.displayName;
            LOG(QString("Subscription update \"%1\": L=%2 R=%3 I=%4")
                    .arg(info.displayName)
                    .arg(lastRenewDate.toString())
                    .arg(QSTRN(info.subscriptionOption.updateInterval))
                    .arg(renewTime.toString()));
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
                LOG("Updating subscription: " + name);
                ConnectionManager->UpdateSubscriptionAsync(id);
            }
            else if (result == Ignore)
            {
                LOG("Ignored subscription update: " + name);
                ConnectionManager->IgnoreSubscriptionUpdate(id);
            }
        }
    }
}

void MainWindow::updateColorScheme()
{
    qvAppTrayIcon->setIcon(KernelInstance->CurrentConnection().isEmpty() ? Q_TRAYICON("tray") : Q_TRAYICON("tray-connected"));
    //
    importConfigButton->setIcon(QIcon(QV2RAY_COLORSCHEME_FILE("add")));
    updownImageBox->setStyleSheet("image: url(" + QV2RAY_COLORSCHEME_FILE("netspeed_arrow") + ")");
    updownImageBox_2->setStyleSheet("image: url(" + QV2RAY_COLORSCHEME_FILE("netspeed_arrow") + ")");
    //
    tray_action_ToggleVisibility->setIcon(this->windowIcon());

    action_RCM_Start->setIcon(QIcon(QV2RAY_COLORSCHEME_FILE("start")));
    action_RCM_Edit->setIcon(QIcon(QV2RAY_COLORSCHEME_FILE("edit")));
    action_RCM_EditJson->setIcon(QIcon(QV2RAY_COLORSCHEME_FILE("code")));
    action_RCM_EditComplex->setIcon(QIcon(QV2RAY_COLORSCHEME_FILE("edit")));
    action_RCM_DuplicateConnection->setIcon(QIcon(QV2RAY_COLORSCHEME_FILE("copy")));
    action_RCM_DeleteConnection->setIcon(QIcon(QV2RAY_COLORSCHEME_FILE("ashbin")));
    action_RCM_ResetStats->setIcon(QIcon(QV2RAY_COLORSCHEME_FILE("ashbin")));
    action_RCM_TestLatency->setIcon(QIcon(QV2RAY_COLORSCHEME_FILE("ping_gauge")));
    action_RCM_RealLatencyTest->setIcon(QIcon(QV2RAY_COLORSCHEME_FILE("ping_gauge")));
    //
    clearChartBtn->setIcon(QIcon(QV2RAY_COLORSCHEME_FILE("ashbin")));
    clearlogButton->setIcon(QIcon(QV2RAY_COLORSCHEME_FILE("ashbin")));
    //
    locateBtn->setIcon(QIcon(QV2RAY_COLORSCHEME_FILE("map")));
    sortBtn->setIcon(QIcon(QV2RAY_COLORSCHEME_FILE("arrow-down-filling")));
    collapseGroupsBtn->setIcon(QIcon(QV2RAY_COLORSCHEME_FILE("arrow-up")));
}

void MainWindow::UpdateActionTranslations()
{
    tray_BypassCNMenu->setTitle(tr("Bypass CN Mainland"));
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
    tray_action_SetBypassCN->setText(tr("Enable Bypassing CN Mainland"));
    tray_action_ClearBypassCN->setText(tr("Disable Bypassing CN Mainland"));
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
    action_RCM_CopySelected->setText(tr("Copy selected."));
}
