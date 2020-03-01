#pragma once

#include "w_MainWindow.hpp"

#include "ui/editors/w_JsonEditor.hpp"
#include "ui/editors/w_OutboundEditor.hpp"
#include "ui/editors/w_RoutesEditor.hpp"
#include "w_ExportConfig.hpp"
#include "w_ImportConfig.hpp"
#include "w_PreferencesWindow.hpp"
#include "w_SubscriptionManager.hpp"
//#include <QAction>
#include "components/pac/QvPACHandler.hpp"
#include "components/plugins/toolbar/QvToolbar.hpp"
#include "core/connection/ConnectionIO.hpp"
#include "ui/widgets/ConnectionInfoWidget.hpp"

#include <QCloseEvent>
#include <QDebug>
#include <QDesktopServices>
#include <QFile>
#include <QFileInfo>
#include <QHeaderView>
#include <QInputDialog>
#include <QKeyEvent>
#include <QMenu>
#include <QStandardItemModel>
#include <QUrl>
#include <QVersionNumber>

// MainWindow.cpp --> Main MainWindow source file, handles mostly UI-related
// operations.

#define TRAY_TOOLTIP_PREFIX "Qv2ray " QV2RAY_VERSION_STRING
#define GetItemWidget(item) (qobject_cast<ConnectionItemWidget *>(connectionListWidget->itemWidget(item, 0)))

MainWindow *MainWindow::mwInstance = nullptr;

QvMessageBusSlotImpl(MainWindow)
{
    switch (msg)
    {
        MBShowDefaultImpl MBHideDefaultImpl MBRetranslateDefaultImpl
    }
}

void MainWindow::MWAddConnectionItem_p(const ConnectionId &connection, const GroupId &groupId)
{
    if (!groupNodes.contains(groupId))
    {
        MWAddGroupItem_p(groupId);
    }
    auto groupItem = groupNodes[groupId];
    auto connectionItem = make_shared<QTreeWidgetItem>(QStringList{ "", ConnectionManager->GetDisplayName(connection) });
    connectionNodes[connection] = connectionItem;
    groupItem->addChild(connectionItem.get());
    auto widget = new ConnectionItemWidget(connection, connectionListWidget);
    connect(widget, &ConnectionItemWidget::RequestWidgetFocus, this, &MainWindow::OnConnectionWidgetFocusRequested);
    connectionListWidget->setItemWidget(connectionItem.get(), 0, widget);
}

void MainWindow::MWAddGroupItem_p(const GroupId &groupId)
{
    auto groupItem = make_shared<QTreeWidgetItem>(QStringList{ "", ConnectionManager->GetDisplayName(groupId) });
    groupNodes[groupId] = groupItem;
    connectionListWidget->addTopLevelItem(groupItem.get());
    connectionListWidget->setItemWidget(groupItem.get(), 0, new ConnectionItemWidget(groupId, connectionListWidget));
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) //, vinstance(), hTray(this), tcpingHelper(3, this)
{
    setupUi(this);
    MainWindow::mwInstance = this;
    QvMessageBusConnect(MainWindow);
    //
    infoWidget = new ConnectionInfoWidget(this);
    connectionInfoLayout->addWidget(infoWidget);
    //
    vCoreLogHighlighter = new SyntaxHighlighter(GlobalConfig.uiConfig.useDarkTheme, masterLogBrowser->document());
    // For charts
    speedChartWidget = new SpeedWidget(this);
    speedChart->addWidget(speedChartWidget);
    this->setWindowIcon(QIcon(":/assets/icons/qv2ray.png"));
    hTray.setIcon(QIcon(GlobalConfig.uiConfig.useDarkTrayIcon ? ":/assets/icons/ui_dark/tray.png" : ":/assets/icons/ui_light/tray.png"));
    importConfigButton->setIcon(QICON_R("import.png"));
    // pingTestBtn->setIcon(QICON_R("ping_gauge.png"));
    // shareBtn->setIcon(QICON_R("share.png"));
    updownImageBox->setStyleSheet("image: url(" + QV2RAY_UI_RESOURCES_ROOT + "netspeed_arrow.png)");
    updownImageBox_2->setStyleSheet("image: url(" + QV2RAY_UI_RESOURCES_ROOT + "netspeed_arrow.png)");
    //
    //
    connect(ConnectionManager, &QvConnectionHandler::OnCrashed, [&] {
        this->show();
        QvMessageBoxWarn(this, tr("V2ray vcore terminated."),
                         tr("V2ray vcore terminated unexpectedly.") + NEWLINE + NEWLINE +
                             tr("To solve the problem, read the V2ray log in the log text browser."));
    });
    //
    connect(ConnectionManager, &QvConnectionHandler::OnConnected, this, &MainWindow::OnConnected);
    connect(ConnectionManager, &QvConnectionHandler::OnDisconnected, this, &MainWindow::OnDisconnected);
    connect(ConnectionManager, &QvConnectionHandler::OnStatsAvailable, this, &MainWindow::OnStatsAvailable);
    connect(ConnectionManager, &QvConnectionHandler::OnVCoreLogAvailable, this, &MainWindow::OnVCoreLogAvailable);
    //
    connect(ConnectionManager, &QvConnectionHandler::OnConnectionDeleted, this, &MainWindow::OnConnectionDeleted);
    connect(ConnectionManager, &QvConnectionHandler::OnConnectionCreated, this, &MainWindow::OnConnectionCreated);
    connect(ConnectionManager, &QvConnectionHandler::OnConnectionGroupChanged, this, &MainWindow::OnConnectionGroupChanged);
    //
    connect(ConnectionManager, &QvConnectionHandler::OnGroupCreated, this, &MainWindow::OnGroupCreated);
    connect(ConnectionManager, &QvConnectionHandler::OnGroupDeleted, this, &MainWindow::OnGroupDeleted);

    //
    connect(infoWidget, &ConnectionInfoWidget::OnEditRequested, this, &MainWindow::OnEditRequested);
    connect(infoWidget, &ConnectionInfoWidget::OnJsonEditRequested, this, &MainWindow::OnJsonEditRequested);
    //
    // Setup System tray icons and menus
    hTray.setToolTip(TRAY_TOOLTIP_PREFIX);
    // Basic actions
    action_Tray_ShowHide = new QAction(this->windowIcon(), tr("Hide"), this);
    action_Tray_ShowPreferencesWindow = new QAction(tr("Preferences"), this);
    action_Tray_Quit = new QAction(tr("Quit"), this);
    action_Tray_Start = new QAction(tr("Connect"), this);
    action_Tray_Reconnect = new QAction(tr("Reconnect"), this);
    action_Tray_Stop = new QAction(tr("Disconnect"), this);
    //
    action_Tray_SetSystemProxy = new QAction(tr("Enable System Proxy"), this);
    action_Tray_ClearSystemProxy = new QAction(tr("Disable System Proxy"), this);
    //
    action_Tray_Start->setEnabled(true);
    action_Tray_Stop->setEnabled(false);
    action_Tray_Reconnect->setEnabled(false);
    //
    tray_SystemProxyMenu->addAction(action_Tray_SetSystemProxy);
    tray_SystemProxyMenu->addAction(action_Tray_ClearSystemProxy);
    tray_SystemProxyMenu->setTitle(tr("System Proxy"));
    tray_SystemProxyMenu->setEnabled(false);
    //
    tray_RootMenu->addAction(action_Tray_ShowHide);
    tray_RootMenu->addSeparator();
    tray_RootMenu->addAction(action_Tray_ShowPreferencesWindow);
    tray_RootMenu->addMenu(tray_SystemProxyMenu);
    tray_RootMenu->addSeparator();
    tray_RootMenu->addAction(action_Tray_Start);
    tray_RootMenu->addAction(action_Tray_Stop);
    tray_RootMenu->addAction(action_Tray_Reconnect);
    tray_RootMenu->addSeparator();
    tray_RootMenu->addAction(action_Tray_Quit);
    //
    connect(action_Tray_ShowHide, &QAction::triggered, this, &MainWindow::ToggleVisibility);
    connect(action_Tray_ShowPreferencesWindow, &QAction::triggered, this, &MainWindow::on_preferencesBtn_clicked);
    //
    connect(action_Tray_Start, &QAction::triggered, [&] { ConnectionManager->StartConnection(lastConnectedId); });
    connect(action_Tray_Stop, &QAction::triggered, ConnectionManager, &QvConnectionHandler::StopConnection);
    connect(action_Tray_Reconnect, &QAction::triggered, ConnectionManager, &QvConnectionHandler::RestartConnection);
    //
    connect(action_Tray_Quit, &QAction::triggered, this, &MainWindow::on_actionExit_triggered);
    connect(action_Tray_SetSystemProxy, &QAction::triggered, this, &MainWindow::MWSetSystemProxy);
    connect(action_Tray_ClearSystemProxy, &QAction::triggered, this, &MainWindow::MWClearSystemProxy);
    connect(&hTray, &QSystemTrayIcon::activated, this, &MainWindow::on_activatedTray);
    //
    // Actions for right click the connection list
    //
    QAction *action_RCM_RenameConnection = new QAction(tr("Rename"), this);
    QAction *action_RCM_StartThis = new QAction(tr("Connect to this"), this);
    QAction *action_RCM_ConvToComplex = new QAction(QICON_R("edit.png"), tr("Edit as Complex Config"), this);
    QAction *action_RCM_EditJson = new QAction(QICON_R("json.png"), tr("Edit as Json"), this);
    QAction *action_RCM_ShareQR = new QAction(QICON_R("share.png"), tr("Share as QRCode/VMess URL"), this);
    //
    connect(action_RCM_RenameConnection, &QAction::triggered, this, &MainWindow::on_action_RCM_RenameConnection_triggered);
    connect(action_RCM_StartThis, &QAction::triggered, this, &MainWindow::on_action_StartThis_triggered);
    connect(action_RCM_EditJson, &QAction::triggered, this, &MainWindow::on_action_RCM_EditJson_triggered);
    connect(action_RCM_ConvToComplex, &QAction::triggered, this, &MainWindow::on_action_RCM_ConvToComplex_triggered);
    //
    // Share options
    connect(action_RCM_ShareQR, &QAction::triggered, this, &MainWindow::on_action_RCM_ShareQR_triggered);
    //
    // Globally invokable signals.
    connect(this, &MainWindow::Connect, [&] { ConnectionManager->StartConnection(lastConnectedId); });
    connect(this, &MainWindow::DisConnect, ConnectionManager, &QvConnectionHandler::StopConnection);
    connect(this, &MainWindow::ReConnect, ConnectionManager, &QvConnectionHandler::RestartConnection);
    //
    hTray.setContextMenu(tray_RootMenu);
    hTray.show();
    //
    connectionListMenu = new QMenu(this);
    connectionListMenu->addAction(action_RCM_StartThis);
    connectionListMenu->addAction(action_RCM_ShareQR);
    connectionListMenu->addAction(action_RCM_RenameConnection);
    connectionListMenu->addAction(action_RCM_EditJson);
    connectionListMenu->addAction(action_RCM_ConvToComplex);
    //
    LOG(MODULE_UI, "Loading data...")
    auto groups = ConnectionManager->AllGroups();

    for (auto group : groups)
    {
        MWAddGroupItem_p(group);
        auto connections = ConnectionManager->Connections(group);

        for (auto connection : connections)
        {
            MWAddConnectionItem_p(connection, group); //
        }
    }
    //
    // Find and start if there is an auto-connection
    auto needShowWindow = true;

    if (!GlobalConfig.autoStartId.isEmpty())
    {
        auto id = ConnectionId(GlobalConfig.autoStartId);
        needShowWindow = ConnectionManager->StartConnection(id).has_value();
    }

    if (needShowWindow)
        this->show();

#ifndef DISABLE_AUTO_UPDATE
    requestHelper = new QvHttpRequestHelper();
    connect(requestHelper, &QvHttpRequestHelper::httpRequestFinished, this, &MainWindow::VersionUpdate);
    requestHelper->get("https://api.github.com/repos/Qv2ray/Qv2ray/releases/latest");
#endif

    if (StartupOption.enableToolbarPlguin)
    {
        LOG(MODULE_UI, "Plugin daemon is enabled.")
        StartProcessingPlugins();
    }

    CheckSubscriptionsUpdate();
    //
    splitter->setSizes(QList<int>() << 100 << 300);
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return)
    {
        // If pressed enter or return on connectionListWidget.
        // Try to connect to the selected connection.
        // if (focusWidget() == connectionListWidget) {
        //    if (!IsSelectionConnectable) return;
        //
        //    auto selections = connectionListWidget->selectedItems();
        //    QVariant v;
        //    auto vv = v.value<QvConnectionObject>();
        //    ShowAndSetConnection(ItemConnectionIdentifier(selections.first()),
        //    true, true);
        //}
    }
}

void MainWindow::on_action_StartThis_triggered()
{
    // if (!IsSelectionConnectable) {
    //    QvMessageBoxWarn(this, tr("No connection selected!"), tr("Please
    //    select a config from the list.")); return;
    //}
    //
    // CurrentSelectedItemPtr = connectionListWidget->selectedItems().first();
    // CurrentConnectionIdentifier =
    // ItemConnectionIdentifier(CurrentSelectedItemPtr);
    // on_reconnectButton_clicked();
}

#ifndef DISABLE_AUTO_UPDATE
void MainWindow::VersionUpdate(QByteArray &data)
{
    // Version update handler.
    QJsonObject root = JsonFromString(QString(data));
    //
    QVersionNumber newVersion = QVersionNumber::fromString(root["tag_name"].toString("v").remove(0, 1));
    QVersionNumber currentVersion = QVersionNumber::fromString(QString(QV2RAY_VERSION_STRING).remove(0, 1));
    QVersionNumber ignoredVersion = QVersionNumber::fromString(GlobalConfig.ignoredVersion);
    LOG(MODULE_UPDATE, "Received update info, Latest: " + newVersion.toString() + " Current: " + currentVersion.toString() +
                           " Ignored: " + ignoredVersion.toString())

    // If the version is newer than us.
    // And new version is newer than the ignored version.
    if (newVersion > currentVersion && newVersion > ignoredVersion)
    {
        LOG(MODULE_UPDATE, "New version detected.")
        auto link = root["html_url"].toString("");
        auto result =
            QvMessageBoxAsk(this, tr("Update"),
                            tr("Found a new version: ") + root["tag_name"].toString("") + "\r\n" + root["name"].toString("") +
                                "\r\n------------\r\n" + root["body"].toString("") + "\r\n------------\r\n" + tr("Download Link: ") + link,
                            QMessageBox::Ignore);

        if (result == QMessageBox::Yes)
        {
            QDesktopServices::openUrl(QUrl::fromUserInput(link));
        }
        else if (result == QMessageBox::Ignore)
        {
            // Set and save ingored version.
            GlobalConfig.ignoredVersion = newVersion.toString();
            // SaveGlobalConfig(GlobalConfig);
        }
    }
}
#endif

MainWindow::~MainWindow()
{
    hTray.hide();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    this->hide();
    action_Tray_ShowHide->setText(tr("Show"));
    event->ignore();
}
void MainWindow::on_activatedTray(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
        case QSystemTrayIcon::Trigger:
            // Toggle Show/Hide
#ifndef __APPLE__
            // Every single click will trigger the Show/Hide toggling.
            // So, as what common macOS Apps do, we don't toggle visibility
            // here.
            ToggleVisibility();
#endif
            break;

        case QSystemTrayIcon::DoubleClick:
#ifdef __APPLE__
            ToggleVisibility();
#endif
            break;

        default: break;
    }
}
void MainWindow::ToggleVisibility()
{
    if (this->isHidden())
    {
        this->show();
#ifdef Q_OS_WIN
        setWindowState(Qt::WindowNoState);
        SetWindowPos(HWND(this->winId()), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
        QThread::msleep(20);
        SetWindowPos(HWND(this->winId()), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
#endif
        tray_RootMenu->actions()[0]->setText(tr("Hide"));
    }
    else
    {
        this->hide();
        tray_RootMenu->actions()[0]->setText(tr("Show"));
    }
}

void MainWindow::on_actionExit_triggered()
{
    if (StartupOption.enableToolbarPlguin)
    {
        StopProcessingPlugins();
    }

    ExitQv2ray();
}

void MainWindow::on_preferencesBtn_clicked()
{
    PreferencesWindow(this).exec();
}
void MainWindow::on_clearlogButton_clicked()
{
    masterLogBrowser->clear();
}
void MainWindow::on_connectionListWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    Q_UNUSED(previous)
    // CurrentItem = current;
    // isRenamingInProgress = false;
    //
    // if (!IsConnectableItem(current)) return;
    //
    //// no need to check !isRenamingInProgress since it's always true.
    // ShowAndSetConnection(ItemConnectionIdentifier(current),
    // !vinstance->KernelStarted, false);
    ////on_connectionListWidget_itemClicked(current, 0);
}
void MainWindow::on_connectionListWidget_customContextMenuRequested(const QPoint &pos)
{
    Q_UNUSED(pos)
    auto _pos = QCursor::pos();
    auto item = connectionListWidget->itemAt(connectionListWidget->mapFromGlobal(_pos));

    if (GetItemWidget(item)->IsConnection())
    {
        connectionListMenu->popup(_pos);
    }
}
void MainWindow::on_action_RCM_RenameConnection_triggered()
{
    // auto item = connectionListWidget->currentItem();
    // SUBSCRIPTION_CONFIG_MODIFY_DENY(item)
    // item->setFlags(item->flags() | Qt::ItemIsEditable);
    // isRenamingInProgress = true;
    // connectionListWidget->editItem(item);
    // renameOriginalIdentifier = ItemConnectionIdentifier(item);
}
void MainWindow::on_connectionListWidget_itemChanged(QTreeWidgetItem *item, int)
{
    // DEBUG(UI, "A connection ListViewItem is changed. This should ONLY occur
    // when renaming an connection.")
    //
    // if (!isRenamingInProgress) {
    //    return;
    //}
    //
    // isRenamingInProgress = false;
    //// In this case it's after we entered the name.
    //// and tell user you should not rename a config from subscription.
    // auto newIdentifier = renameOriginalIdentifier;
    // newIdentifier.connectionName = item->text(0);
    // LOG(CONNECTION, "RENAME: " + renameOriginalIdentifier.IdentifierString()
    // + " -> " + newIdentifier.IdentifierString())
    //
    //// If I really did some changes.
    // if (renameOriginalIdentifier != newIdentifier) {
    //    bool canContinueRename = true;
    //
    //    if (newIdentifier.connectionName.trimmed().isEmpty()) {
    //        QvMessageBoxWarn(this, tr("Rename a Connection"), tr("The name
    //        cannot be empty")); canContinueRename = false;
    //    }
    //
    //    QvMessageBoxInfo(this, "NOT SUPPORTED", "WIP");
    //    //if (GlobalConfig.configs.contains(newIdentifier.connectionName)) {
    //    //    QvMessageBoxWarn(this, tr("Rename a Connection"), tr("The name
    //    has been used already, Please choose another."));
    //    //    canContinueRename = false;
    //    //}
    //
    //    if (!IsValidFileName(newIdentifier.connectionName +
    //    QV2RAY_CONFIG_FILE_EXTENSION)) {
    //        QvMessageBoxWarn(this, tr("Rename a Connection"), tr("The name you
    //        suggested is not valid, please try another.")); canContinueRename
    //        = false;
    //    }
    //
    //    if (!canContinueRename) {
    //        // Set the item text back
    //        assert(item != nullptr); // Let's say the item should not be null
    //        item->setText(0, renameOriginalIdentifier.connectionName);
    //        return;
    //    }
    //
    //    // Change auto start config.
    //    //  |--------------=== In case it's not in a subscription --|
    //    if (GlobalConfig.autoStartConfig == renameOriginalIdentifier) {
    //        GlobalConfig.autoStartConfig = newIdentifier;
    //    }
    //
    //    QvMessageBoxInfo(this, "NOT SUPPORTED", "WIP");
    //    //// Replace the items in the current loaded config list and settings.
    //    //// Note: This original name should only be a reguular.
    //    //GlobalConfig.configs.removeOne(renameOriginalIdentifier.connectionName);
    //    //GlobalConfig.configs.push_back(newIdentifier.connectionName);
    //    //
    //    //connections[newIdentifier] =
    //    connections.take(renameOriginalIdentifier);
    //    //RenameConnection(renameOriginalIdentifier.connectionName,
    //    newIdentifier.connectionName);
    //    //LOG(UI, "Saving a global config")
    //    //SaveGlobalConfig(GlobalConfig);
    //    ////
    //    //item->setData(0, Qt::UserRole, QVariant::fromValue(newIdentifier));
    //    //
    //    //if (CurrentConnectionIdentifier == renameOriginalIdentifier) {
    //    //    CurrentConnectionIdentifier = newIdentifier;
    //    //
    //    //    if (vinstance->KernelStarted) {
    //    //        on_reconnectButton_clicked();
    //    //    }
    //    //}
    //    //OnConfigListChanged(CurrentConnectionIdentifier.connectionName ==
    //    renameOriginalName);
    //}
}
void MainWindow::on_removeConfigButton_clicked()
{
    QvMessageBoxInfo(this, "NOT SUPPORTED", "WIP");
    // QList<ConnectionIdentifier> connlist;
    //
    // for (auto item : connectionListWidget->selectedItems()) {
    //    if (IsConnectableItem(item)) {
    //        connlist.append(ItemConnectionIdentifier(item));
    //    }
    //}
    //
    // LOG(UI, "Selected " + QSTRN(connlist.count()) + " items")
    //
    // if (connlist.isEmpty()) {
    //    // Remove nothing means doing nothing.
    //    return;
    //}
    //
    // if (QvMessageBoxAsk(this, tr("Removing Connection(s)"), tr("Are you sure
    // to remove selected connection(s)?")) != QMessageBox::Yes) {
    //    return;
    //}
    //
    //// A triple-state flag which indicates if the user wants to remove the
    /// configs loaded from a subscription.
    // int subscriptionRemovalCheckStatus = -1;
    //
    // for (auto conn : connlist) {
    //    if (conn == CurrentConnectionIdentifier) {
    //        on_stopButton_clicked();
    //        CurrentConnectionIdentifier = ConnectionIdentifier();
    //    }
    //
    //    auto connData = connections[conn];
    //
    //    // Remove auto start config.
    //    if (GlobalConfig.autoStartConfig.subscriptionName ==
    //    connData.subscriptionName &&
    //        GlobalConfig.autoStartConfig.connectionName ==
    //        connData.connectionName)
    //        // If all those settings match.
    //    {
    //        GlobalConfig.autoStartConfig.subscriptionName.clear();
    //        GlobalConfig.autoStartConfig.connectionName.clear();
    //    }
    //
    //    if (connData.configType == CONNECTION_REGULAR) {
    //        // Just remove the regular configs.
    //        if (!connData.subscriptionName.isEmpty()) {
    //            LOG(UI, "Unexpected subscription name in a single regular
    //            config.") connData.subscriptionName.clear();
    //        }
    //
    //        GlobalConfig.configs.removeOne(conn.connectionName);
    //
    //        if (!RemoveConnection(conn.connectionName)) {
    //            QvMessageBoxWarn(this, tr("Removing this Connection"),
    //            tr("Failed to delete connection file, please delete
    //            manually."));
    //        }
    //    } else if (connData.configType == CONNECTION_SUBSCRIPTION) {
    //        if (subscriptionRemovalCheckStatus == -1) {
    //            subscriptionRemovalCheckStatus = (QvMessageBoxAsk(this,
    //            tr("Removing a subscription config"), tr("Do you want to
    //            remove the config loaded from a subscription?")) ==
    //            QMessageBox::Yes)
    //                                             ? 1 // Yes i want
    //                                             : 0; // No please keep
    //        }
    //
    //        if (subscriptionRemovalCheckStatus == 1) {
    //            if (!RemoveSubscriptionConnection(connData.subscriptionName,
    //            connData.connectionName)) {
    //                QvMessageBoxWarn(this, tr("Removing this Connection"),
    //                tr("Failed to delete connection file, please delete
    //                manually."));
    //            }
    //        }
    //    } else {
    //        LOG(SETTINGS, "Unknown config type -> Not regular nor
    //        subscription...")
    //    }
    //}
    //
    // LOG(UI, "Saving GlobalConfig")
    // SaveGlobalConfig(GlobalConfig);
    // OnConfigListChanged(false);
    // ShowAndSetConnection(CurrentConnectionIdentifier, false, false);
}

void MainWindow::on_importConfigButton_clicked()
{

    ImportConfigWindow w(this);
    auto configs = w.OpenImport();
    if (!configs.isEmpty())
    {
        for (auto conf : configs)
        {
            auto name = configs.key(conf, "");

            if (name.isEmpty())
                continue;
            ConnectionManager->CreateConnection(name, DefaultGroupId, conf);
        }
    }
}
void MainWindow::on_editConfigButton_clicked()
{
}

void MainWindow::on_action_RCM_ConvToComplex_triggered()
{
    //// Check if we have a connection selected...
    // if (!IsSelectionConnectable)
    //{
    //    QvMessageBoxWarn(this, tr("No Config Selected"), tr("Please Select a Config"));
    //    return;
    //}
    //
    // auto selectedFirst = connectionListWidget->currentItem();
    // auto _identifier = ItemConnectionIdentifier(selectedFirst);
    // SUBSCRIPTION_CONFIG_MODIFY_DENY(selectedFirst)
    ////
    // auto outBoundRoot = connections[_identifier].config;
    // CONFIGROOT root;
    // bool isChanged = false;
    ////
    // LOG(UI, "INFO: Opening route editor.")
    // RouteEditor routeWindow(outBoundRoot, this);
    // root = routeWindow.OpenEditor();
    // isChanged = routeWindow.result() == QDialog::Accepted;
    // QString alias = _identifier.connectionName;
    //
    // if (isChanged)
    //{
    //    connections[_identifier].config = root;
    //    // true indicates the alias will NOT change
    //    SaveConnectionConfig(root, &alias, true);
    //    OnConfigListChanged(_identifier == CurrentConnectionIdentifier);
    //    ShowAndSetConnection(CurrentConnectionIdentifier, false, false);
    //}
}

void MainWindow::on_action_RCM_EditJson_triggered()
{
}

void MainWindow::on_action_RCM_ShareQR_triggered()
{
    // on_shareBtn_clicked();
}

void MainWindow::on_subsButton_clicked()
{
    SubscribeEditor().exec();
}

void MainWindow::on_connectionListWidget_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column)
    auto widget = GetItemWidget(item);

    if (widget->IsConnection())
    {
        widget->BeginConnection();
    }
}

void MainWindow::OnDisconnected(const ConnectionId &id)
{
    Q_UNUSED(id)
    this->hTray.showMessage("Qv2ray", tr("Disonnected"), this->windowIcon());
    hTray.setToolTip(TRAY_TOOLTIP_PREFIX NEWLINE);
    connetionStatusLabel->setText(tr("Disconnected"));

    if (systemProxyEnabled)
    {
        MWClearSystemProxy(false);
    }

    // QFile(QV2RAY_GENERATED_FILE_PATH).remove();

    if (GlobalConfig.inboundConfig.pacConfig.enablePAC)
    {
        // pacServer.StopServer();
        LOG(MODULE_UI, "Stopping PAC server")
    }
}

void MainWindow::OnConnected(const ConnectionId &id)
{
    Q_UNUSED(id)
    on_clearlogButton_clicked();
    auto name = ConnectionManager->GetDisplayName(id);
    this->hTray.showMessage("Qv2ray", tr("Connected: ") + name, this->windowIcon());
    hTray.setToolTip(TRAY_TOOLTIP_PREFIX NEWLINE + tr("Connected: ") + name);
    connetionStatusLabel->setText(tr("Connected: ") + name);
    //
    ConnectionManager->StartLatencyTest(id);
    bool usePAC = GlobalConfig.inboundConfig.pacConfig.enablePAC;
    bool pacUseSocks = GlobalConfig.inboundConfig.pacConfig.useSocksProxy;
    bool httpEnabled = GlobalConfig.inboundConfig.useHTTP;
    bool socksEnabled = GlobalConfig.inboundConfig.useSocks;

    if (usePAC)
    {
        bool canStartPAC = true;
        QString pacProxyString; // Something like this --> SOCKS5 127.0.0.1:1080; SOCKS
                                // 127.0.0.1:1080; DIRECT; http://proxy:8080
        auto pacIP = GlobalConfig.inboundConfig.pacConfig.localIP;

        if (pacIP.isEmpty())
        {
            LOG(MODULE_PROXY, "PAC Local IP is empty, default to 127.0.0.1")
            pacIP = "127.0.0.1";
        }

        if (pacUseSocks)
        {
            if (socksEnabled)
            {
                pacProxyString = "SOCKS5 " + pacIP + ":" + QSTRN(GlobalConfig.inboundConfig.socks_port);
            }
            else
            {
                LOG(MODULE_UI, "PAC is using SOCKS, but it is not enabled")
                QvMessageBoxWarn(this, tr("Configuring PAC"),
                                 tr("Could not start PAC server as it is configured to use SOCKS, but it is not enabled"));
                canStartPAC = false;
            }
        }
        else
        {
            if (httpEnabled)
            {
                pacProxyString = "PROXY " + pacIP + ":" + QSTRN(GlobalConfig.inboundConfig.http_port);
            }
            else
            {
                LOG(MODULE_UI, "PAC is using HTTP, but it is not enabled")
                QvMessageBoxWarn(this, tr("Configuring PAC"),
                                 tr("Could not start PAC server as it is configured to use HTTP, but it is not enabled"));
                canStartPAC = false;
            }
        }

        if (canStartPAC)
        {
            // pacServer.SetProxyString(pacProxyString);
            // pacServer.StartListen();
        }
        else
        {
            LOG(MODULE_PROXY, "Not starting PAC due to previous error.")
        }
    }

    if (GlobalConfig.inboundConfig.setSystemProxy)
    {
        MWSetSystemProxy();
    }
}

void MainWindow::OnConnectionWidgetFocusRequested(const ConnectionItemWidget *_widget)
{
    if (_widget == nullptr)
    {
        return;
    }

    for (auto _item_ : connectionListWidget->findItems(QString("*"), Qt::MatchWrap | Qt::MatchWildcard | Qt::MatchRecursive))
    {
        if (GetItemWidget(_item_) == _widget)
        {
            LOG(MODULE_UI, "Setting current item.")
            connectionListWidget->setCurrentItem(_item_);
            connectionListWidget->scrollToItem(_item_);
            // Click it to show details.
            on_connectionListWidget_itemClicked(_item_, 0);
        }
    }
}

void MainWindow::on_connectionFilterTxt_textEdited(const QString &arg1)
{
    // No recursive since we only need top level item
    for (auto _top_item_ : connectionListWidget->findItems(QString("*"), Qt::MatchWrap | Qt::MatchWildcard))
    {
        // auto topWidget = GetItemWidget(_top_item_);
        bool isTotallyHide = true;

        for (auto i = 0; i < _top_item_->childCount(); i++)
        {
            auto _child_ = _top_item_->child(i);

            if (GetItemWidget(_child_)->NameMatched(arg1))
            {
                LOG(MODULE_UI, "Setting current item.")
                // Show the child
                _child_->setHidden(false);
                // If any one of the children matches, the parent should not be hidden.
                isTotallyHide = false;
            }
            else
            {
                _child_->setHidden(true);
            }
        }

        _top_item_->setHidden(isTotallyHide);

        if (!isTotallyHide)
        {
            connectionListWidget->expandItem(_top_item_);
        }
    }
}

void MainWindow::on_connectionListWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column)
    infoWidget->ShowDetails(GetItemWidget(item)->Identifier());
}

void MainWindow::OnStatsAvailable(const ConnectionId &id, const quint64 upS, const quint64 downS, const quint64 upD, const quint64 downD)
{
    Q_UNUSED(id);
    // This may not be, or may not precisely be, speed per second if the backend
    // has "any" latency. (Hope not...)
    speedChartWidget->AddPointData(upS, downS);
    //
    auto totalSpeedUp = FormatBytes(upS) + "/s";
    auto totalSpeedDown = FormatBytes(downS) + "/s";
    auto totalDataUp = FormatBytes(upD);
    auto totalDataDown = FormatBytes(downD);
    //
    netspeedLabel->setText(totalSpeedUp + NEWLINE + totalSpeedDown);
    dataamountLabel->setText(totalDataUp + NEWLINE + totalDataDown);
    //
    hTray.setToolTip(TRAY_TOOLTIP_PREFIX NEWLINE + tr("Connected: ") + ConnectionManager->GetDisplayName(id) + NEWLINE "Up: " + totalSpeedUp +
                     " Down: " + totalSpeedDown);
}

void MainWindow::OnVCoreLogAvailable(const ConnectionId &id, const QString &log)
{
    Q_UNUSED(id);
    auto bar = masterLogBrowser->verticalScrollBar();
    auto max = bar->maximum();
    auto val = bar->value();
    masterLogBrowser->append(log);
    // From https://gist.github.com/jemyzhang/7130092
    auto maxLines = GlobalConfig.uiConfig.maximumLogLines;
    QTextBlock block = masterLogBrowser->document()->begin();

    while (block.isValid())
    {
        if (masterLogBrowser->document()->blockCount() > maxLines)
        {
            QTextCursor cursor(block);
            block = block.next();
            cursor.select(QTextCursor::BlockUnderCursor);
            cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
            cursor.removeSelectedText();
            continue;
        }

        break;
    }

    if (val >= max * 0.8 || val >= max - 20)
        bar->setValue(max);
}

void MainWindow::OnEditRequested(const ConnectionId &id)
{
    auto outBoundRoot = ConnectionManager->GetConnectionRoot(id);
    CONFIGROOT root;
    bool isChanged = false;

    if (IsComplexConfig(outBoundRoot))
    {
        LOG(MODULE_UI, "INFO: Opening route editor.")
        RouteEditor routeWindow(outBoundRoot, this);
        root = routeWindow.OpenEditor();
        isChanged = routeWindow.result() == QDialog::Accepted;
    }
    else
    {
        LOG(MODULE_UI, "INFO: Opening single connection edit window.")
        auto out = OUTBOUND(outBoundRoot["outbounds"].toArray().first().toObject());
        OutboundEditor w(out, this);
        auto outboundEntry = w.OpenEditor();
        isChanged = w.result() == QDialog::Accepted;
        QJsonArray outboundsList;
        outboundsList.push_back(outboundEntry);
        root.insert("outbounds", outboundsList);
    }

    if (isChanged)
    {
        ConnectionManager->UpdateConnection(id, root);
    }
}
void MainWindow::OnJsonEditRequested(const ConnectionId &id)
{
    JsonEditor w(ConnectionManager->GetConnectionRoot(id), this);
    auto root = CONFIGROOT(w.OpenEditor());

    if (w.result() == QDialog::Accepted)
    {
        ConnectionManager->UpdateConnection(id, root);
    }
}

void MainWindow::OnConnectionCreated(const ConnectionId &id, const QString &displayName)
{
    Q_UNUSED(displayName)
    MWAddConnectionItem_p(id, ConnectionManager->GetConnectionGroupId(id));
}
void MainWindow::OnConnectionDeleted(const ConnectionId &id, const GroupId &groupId)
{
    auto &child = connectionNodes[id];
    groupNodes[groupId]->removeChild(child.get());
    connectionNodes.remove(id);
}
void MainWindow::OnConnectionGroupChanged(const ConnectionId &id, const GroupId &originalGroup, const GroupId &newGroup)
{
    delete GetItemWidget(connectionNodes[id].get());
    groupNodes[originalGroup]->removeChild(connectionNodes[id].get());
    connectionNodes.remove(id);
    MWAddConnectionItem_p(id, newGroup);
}
void MainWindow::OnGroupCreated(const GroupId &id, const QString &displayName)
{
    Q_UNUSED(displayName)
    MWAddGroupItem_p(id);
}
void MainWindow::OnGroupDeleted(const GroupId &id, const QList<ConnectionId> &connections)
{
    for (auto conn : connections)
    {
        groupNodes[id]->removeChild(connectionNodes[conn].get());
    }
    groupNodes.remove(id);
}
