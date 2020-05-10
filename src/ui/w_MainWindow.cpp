#include "w_MainWindow.hpp"

#include "components/plugins/QvPluginHost.hpp"
#include "components/plugins/toolbar/QvToolbar.hpp"
#include "components/proxy/QvProxyConfigurator.hpp"
#include "components/update/UpdateChecker.hpp"
#include "core/settings/SettingsBackend.hpp"
#include "ui/editors/w_JsonEditor.hpp"
#include "ui/editors/w_OutboundEditor.hpp"
#include "ui/editors/w_RoutesEditor.hpp"
#include "ui/w_GroupManager.hpp"
#include "ui/w_ImportConfig.hpp"
#include "ui/w_PluginManager.hpp"
#include "ui/w_PreferencesWindow.hpp"
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

#ifdef Q_OS_MAC
    #include <ApplicationServices/ApplicationServices.h>
#endif

#define TRAY_TOOLTIP_PREFIX "Qv2ray " QV2RAY_VERSION_STRING
#define CheckCurrentWidget                                                                                                                      \
    auto widget = GetItemWidget(connectionListWidget->currentItem());                                                                           \
    if (widget == nullptr)                                                                                                                      \
        return;

#define GetItemWidget(item) (qobject_cast<ConnectionItemWidget *>(connectionListWidget->itemWidget(item, 0)))
#define NumericString(i) (QString("%1").arg(i, 30, 10, QLatin1Char('0')))

MainWindow *MainWindow::MainWindowInstance = nullptr;

QvMessageBusSlotImpl(MainWindow)
{
    switch (msg)
    {
        MBShowDefaultImpl MBHideDefaultImpl MBRetranslateDefaultImpl MBUpdateColorSchemeDefaultImpl
    }
}

void MainWindow::UpdateColorScheme()
{
    hTray.setIcon(KernelInstance->CurrentConnection().isEmpty() ? Q_TRAYICON("tray.png") : Q_TRAYICON("tray-connected.png"));
    //
    importConfigButton->setIcon(QICON_R("import.png"));
    updownImageBox->setStyleSheet("image: url(" + QV2RAY_COLORSCHEME_ROOT + "netspeed_arrow.png)");
    updownImageBox_2->setStyleSheet("image: url(" + QV2RAY_COLORSCHEME_ROOT + "netspeed_arrow.png)");
    //
    tray_action_ShowHide->setIcon(this->windowIcon());
    action_RCM_Start->setIcon(QICON_R("connect.png"));
    action_RCM_Edit->setIcon(QICON_R("edit.png"));
    action_RCM_EditJson->setIcon(QICON_R("json.png"));
    action_RCM_EditComplex->setIcon(QICON_R("edit.png"));
    action_RCM_Duplicate->setIcon(QICON_R("duplicate.png"));
    action_RCM_Delete->setIcon(QICON_R("delete.png"));
    action_RCM_ClearUsage->setIcon(QICON_R("delete.png"));
    action_RCM_LatencyTest->setIcon(QICON_R("ping_gauge.png"));
    //
    clearChartBtn->setIcon(QICON_R("delete.png"));
    clearlogButton->setIcon(QICON_R("delete.png"));
    //
    locateBtn->setIcon(QICON_R("locate.png"));
    sortBtn->setIcon(QICON_R("sort.png"));
}

void MainWindow::MWAddConnectionItem_p(const ConnectionGroupPair &id)
{
    if (!groupNodes.contains(id.groupId))
    {
        MWAddGroupItem_p(id.groupId);
    }
    auto groupItem = groupNodes.value(id.groupId);
    auto connectionItem = std::make_shared<QTreeWidgetItem>(QStringList{
        "",                                                    //
        GetDisplayName(id.connectionId),                       //
        NumericString(GetConnectionLatency(id.connectionId)),  //
        "IMPORTTIME_NOT_SUPPORTED",                            //
        "LAST_CONNECTED_NOT_SUPPORTED",                        //
        NumericString(GetConnectionTotalData(id.connectionId)) //
    });
    connectionNodes.insert(id, connectionItem);
    groupItem->addChild(connectionItem.get());
    auto widget = new ConnectionItemWidget(id, connectionListWidget);
    connect(widget, &ConnectionItemWidget::RequestWidgetFocus, this, &MainWindow::OnConnectionWidgetFocusRequested);
    connectionListWidget->setItemWidget(connectionItem.get(), 0, widget);
}

void MainWindow::MWAddGroupItem_p(const GroupId &groupId)
{
    auto groupItem = std::make_shared<QTreeWidgetItem>(QStringList{ "", GetDisplayName(groupId) });
    groupNodes.insert(groupId, groupItem);
    connectionListWidget->addTopLevelItem(groupItem.get());
    connectionListWidget->setItemWidget(groupItem.get(), 0, new ConnectionItemWidget(groupId, connectionListWidget));
}

void MainWindow::SortConnectionList(MW_ITEM_COL byCol, bool asending)
{
    connectionListWidget->sortByColumn(MW_ITEM_COL_NAME, Qt::AscendingOrder);
    for (auto i = 0; i < connectionListWidget->topLevelItemCount(); i++)
    {
        connectionListWidget->topLevelItem(i)->sortChildren(byCol, asending ? Qt::AscendingOrder : Qt::DescendingOrder);
    }
    on_locateBtn_clicked();
}

void MainWindow::ReloadRecentConnectionList(const QList<ConnectionGroupPair> &items)
{
    QList<QAction *> newActions;
    for (const auto &item : items)
    {
        auto action = new QAction(tray_RecentConnectionsMenu);
        action->setText(GetDisplayName(item.connectionId));
        action->setData(QVariant::fromValue(item));
        connect(ConnectionManager, &QvConfigHandler::OnConnectionRenamed,
                [action](const ConnectionId &_t1, const QString &, const QString &_t3) {
                    if (_t1.toString() == action->data().toString())
                    {
                        action->setText(_t3);
                    }
                });
        connect(action, &QAction::triggered, [action]() { //
            emit ConnectionManager->StartConnection(action->data().value<ConnectionGroupPair>());
        });
        newActions << action;
    }
    for (const auto action : tray_RecentConnectionsMenu->actions())
    {
        tray_RecentConnectionsMenu->removeAction(action);
        action->deleteLater();
    }
    tray_RecentConnectionsMenu->addActions(newActions);
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setupUi(this);
    MainWindow::MainWindowInstance = this;
    QvMessageBusConnect(MainWindow);
    //
    infoWidget = new ConnectionInfoWidget(this);
    connectionInfoLayout->addWidget(infoWidget);
    //
    masterLogBrowser->setDocument(vCoreLogDocument);
    vCoreLogHighlighter = new SyntaxHighlighter(GlobalConfig.uiConfig.useDarkTheme, masterLogBrowser->document());
    // For charts
    speedChartWidget = new SpeedWidget(this);
    speedChart->addWidget(speedChartWidget);
    //
    this->setWindowIcon(QIcon(":/assets/icons/qv2ray.png"));
    UpdateColorScheme();
    //
    //
    connect(ConnectionManager, &QvConfigHandler::OnKernelCrashed, [&](const ConnectionGroupPair &, const QString &reason) {
        this->show();
        QvMessageBoxWarn(this, tr("Kernel terminated."),
                         tr("The kernel terminated unexpectedly:") + NEWLINE + reason + NEWLINE + NEWLINE +
                             tr("To solve the problem, read the kernel log in the log text browser."));
    });
    //
    connect(ConnectionManager, &QvConfigHandler::OnConnected, this, &MainWindow::OnConnected);
    connect(ConnectionManager, &QvConfigHandler::OnDisconnected, this, &MainWindow::OnDisconnected);
    connect(ConnectionManager, &QvConfigHandler::OnStatsAvailable, this, &MainWindow::OnStatsAvailable);
    connect(ConnectionManager, &QvConfigHandler::OnKernelLogAvailable, this, &MainWindow::OnVCoreLogAvailable);
    //
    connect(ConnectionManager, &QvConfigHandler::OnConnectionDeleted, this, &MainWindow::OnConnectionDeleted);
    connect(ConnectionManager, &QvConfigHandler::OnConnectionCreated, this, &MainWindow::OnConnectionCreated);
    connect(ConnectionManager, &QvConfigHandler::OnConnectionLinkedWithGroup, this, &MainWindow::OnConnectionLinkedWithGroup);
    //
    connect(ConnectionManager, &QvConfigHandler::OnGroupCreated, this, &MainWindow::OnGroupCreated);
    connect(ConnectionManager, &QvConfigHandler::OnGroupDeleted, this, &MainWindow::OnGroupDeleted);
    //
    connect(ConnectionManager, &QvConfigHandler::OnConnectionRenamed, [&](const ConnectionId &id, const QString &, const QString &newName) {
        ConnectionGroupPair pair = { id, ConnectionManager->GetGroupId(id).first() };
        if (connectionNodes.contains(pair))
            connectionNodes.value(pair)->setText(MW_ITEM_COL_NAME, newName); //
    });
    connect(ConnectionManager, &QvConfigHandler::OnLatencyTestFinished, [&](const ConnectionId &id, const uint avg) {
        ConnectionGroupPair pair = { id, ConnectionManager->GetGroupId(id).first() };
        if (connectionNodes.contains(pair))
            connectionNodes.value(pair)->setText(MW_ITEM_COL_PING, NumericString(avg)); //
    });
    //
    connect(infoWidget, &ConnectionInfoWidget::OnEditRequested, this, &MainWindow::OnEditRequested);
    connect(infoWidget, &ConnectionInfoWidget::OnJsonEditRequested, this, &MainWindow::OnEditJsonRequested);
    //
    //
    // Setup System tray icons and menus
    hTray.setToolTip(TRAY_TOOLTIP_PREFIX);
    //
    // Basic tray actions
    hTray.show();
    tray_action_Start->setEnabled(true);
    tray_action_Stop->setEnabled(false);
    tray_action_Restart->setEnabled(false);
    //
    tray_SystemProxyMenu->setEnabled(false);
    tray_SystemProxyMenu->addAction(tray_action_SetSystemProxy);
    tray_SystemProxyMenu->addAction(tray_action_ClearSystemProxy);
    //
    tray_RootMenu->addAction(tray_action_ShowHide);
    tray_RootMenu->addSeparator();
    tray_RootMenu->addAction(tray_action_ShowPreferencesWindow);
    tray_RootMenu->addMenu(tray_SystemProxyMenu);
    // This feature is not ready
    // tray_RootMenu->addSeparator();
    // tray_RootMenu->addMenu(tray_RecentConnectionsMenu);
    tray_RootMenu->addSeparator();
    tray_RootMenu->addAction(tray_action_Start);
    tray_RootMenu->addAction(tray_action_Stop);
    tray_RootMenu->addAction(tray_action_Restart);
    tray_RootMenu->addSeparator();
    tray_RootMenu->addAction(tray_action_Quit);
    hTray.setContextMenu(tray_RootMenu);
    //
    connect(tray_action_ShowHide, &QAction::triggered, this, &MainWindow::ToggleVisibility);
    connect(tray_action_ShowPreferencesWindow, &QAction::triggered, this, &MainWindow::on_preferencesBtn_clicked);
    connect(tray_action_Start, &QAction::triggered, [&] { ConnectionManager->StartConnection(lastConnectedIdentifier); });
    connect(tray_action_Stop, &QAction::triggered, ConnectionManager, &QvConfigHandler::StopConnection);
    connect(tray_action_Restart, &QAction::triggered, ConnectionManager, &QvConfigHandler::RestartConnection);
    connect(tray_action_Quit, &QAction::triggered, this, &MainWindow::on_actionExit_triggered);
    connect(tray_action_SetSystemProxy, &QAction::triggered, this, &MainWindow::MWSetSystemProxy);
    connect(tray_action_ClearSystemProxy, &QAction::triggered, this, &MainWindow::MWClearSystemProxy);
    connect(&hTray, &QSystemTrayIcon::activated, this, &MainWindow::on_activatedTray);
    //
    // Actions for right click the log text browser
    //
    logRCM_Menu->addAction(action_RCM_tovCoreLog);
    logRCM_Menu->addAction(action_RCM_toQvLog);
    connect(masterLogBrowser, &QTextBrowser::customContextMenuRequested, [&](const QPoint &) { logRCM_Menu->popup(QCursor::pos()); });
    connect(action_RCM_tovCoreLog, &QAction::triggered, this, &MainWindow::on_action_RCM_tovCoreLog_triggered);
    connect(action_RCM_toQvLog, &QAction::triggered, this, &MainWindow::on_action_RCM_toQvLog_triggered);
    //
    // Globally invokable signals.
    //
    connect(this, &MainWindow::StartConnection, ConnectionManager, &QvConfigHandler::RestartConnection);
    connect(this, &MainWindow::StopConnection, ConnectionManager, &QvConfigHandler::StopConnection);
    connect(this, &MainWindow::RestartConnection, ConnectionManager, &QvConfigHandler::RestartConnection);
    //
    // Actions for right click the connection list
    //
    connectionListRCM_Menu->addAction(action_RCM_Start);
    connectionListRCM_Menu->addSeparator();
    connectionListRCM_Menu->addAction(action_RCM_Edit);
    connectionListRCM_Menu->addAction(action_RCM_EditJson);
    connectionListRCM_Menu->addAction(action_RCM_EditComplex);
    connectionListRCM_Menu->addSeparator();
    connectionListRCM_Menu->addAction(action_RCM_LatencyTest);
    connectionListRCM_Menu->addSeparator();
    connectionListRCM_Menu->addAction(action_RCM_SetAutoConnection);
    connectionListRCM_Menu->addSeparator();
    connectionListRCM_Menu->addAction(action_RCM_Rename);
    connectionListRCM_Menu->addAction(action_RCM_Duplicate);
    connectionListRCM_Menu->addAction(action_RCM_ClearUsage);
    connectionListRCM_Menu->addSeparator();
    connectionListRCM_Menu->addAction(action_RCM_Delete);
    connect(action_RCM_Start, &QAction::triggered, this, &MainWindow::on_action_StartThis_triggered);
    connect(action_RCM_SetAutoConnection, &QAction::triggered, this, &MainWindow::on_action_RCM_SetAutoConnection_triggered);
    connect(action_RCM_Edit, &QAction::triggered, this, &MainWindow::on_action_RCM_EditThis_triggered);
    connect(action_RCM_EditJson, &QAction::triggered, this, &MainWindow::on_action_RCM_EditAsJson_triggered);
    connect(action_RCM_EditComplex, &QAction::triggered, this, &MainWindow::on_action_RCM_EditAsComplex_triggered);
    connect(action_RCM_LatencyTest, &QAction::triggered, this, &MainWindow::on_action_RCM_LatencyTest_triggered);
    connect(action_RCM_Rename, &QAction::triggered, this, &MainWindow::on_action_RCM_RenameThis_triggered);
    connect(action_RCM_Duplicate, &QAction::triggered, this, &MainWindow::on_action_RCM_DuplicateThese_triggered);
    connect(action_RCM_ClearUsage, &QAction::triggered, this, &MainWindow::on_action_RCM_ClearUsage_triggered);
    connect(action_RCM_Delete, &QAction::triggered, this, &MainWindow::on_action_RCM_DeleteThese_triggered);
    //
    // Sort Menu
    //
    connect(sortAction_SortByName_Asc, &QAction::triggered, [&] { SortConnectionList(MW_ITEM_COL_NAME, true); });
    connect(sortAction_SortByName_Dsc, &QAction::triggered, [&] { SortConnectionList(MW_ITEM_COL_NAME, false); });
    connect(sortAction_SortByData_Asc, &QAction::triggered, [&] { SortConnectionList(MW_ITEM_COL_DATA, true); });
    connect(sortAction_SortByData_Dsc, &QAction::triggered, [&] { SortConnectionList(MW_ITEM_COL_DATA, false); });
    connect(sortAction_SortByPing_Asc, &QAction::triggered, [&] { SortConnectionList(MW_ITEM_COL_PING, true); });
    connect(sortAction_SortByPing_Dsc, &QAction::triggered, [&] { SortConnectionList(MW_ITEM_COL_PING, false); });
    //
    sortMenu->addAction(sortAction_SortByName_Asc);
    sortMenu->addAction(sortAction_SortByName_Dsc);
    sortMenu->addSeparator();
    sortMenu->addAction(sortAction_SortByData_Asc);
    sortMenu->addAction(sortAction_SortByData_Dsc);
    sortMenu->addSeparator();
    sortMenu->addAction(sortAction_SortByPing_Asc);
    sortMenu->addAction(sortAction_SortByPing_Dsc);
    //
    sortBtn->setMenu(sortMenu);
    //
    LOG(MODULE_UI, "Loading data...") //
    auto groups = ConnectionManager->AllGroups();

    for (auto group : groups)
    {
        MWAddGroupItem_p(group);
        auto connections = ConnectionManager->Connections(group);

        for (const auto &connection : connections)
        {
            MWAddConnectionItem_p({ connection, group });
        }
    }
    //
    // Find and start if there is an auto-connection
    auto needShowWindow = true;

    if (GlobalConfig.autoStartId.connectionId != NullConnectionId)
    {
        // Empty means we are connected, so has_value is false.
        // So no need to show is false.
        needShowWindow = ConnectionManager->StartConnection(GlobalConfig.autoStartId.connectionId, GlobalConfig.autoStartId.groupId).has_value();
    }
    if (needShowWindow && connectionListWidget->topLevelItemCount() > 0)
    {
        // Select the first connection.
        auto item = (connectionListWidget->topLevelItem(0)->childCount() > 0) ? connectionListWidget->topLevelItem(0)->child(0) :
                                                                                connectionListWidget->topLevelItem(0);
        connectionListWidget->setCurrentItem(item);
        on_connectionListWidget_itemClicked(item, 0);
    }
    ReloadRecentConnectionList(GlobalConfig.uiConfig.recentConnections);
    //
    if (needShowWindow)
        this->show();
    //
    tray_action_ShowHide->setText(needShowWindow ? tr("Hide") : tr("Show"));
    //
    if (StartupOption.enableToolbarPlguin)
    {
        LOG(MODULE_UI, "Plugin daemon is enabled.")
        StartProcessingPlugins();
    }

    CheckSubscriptionsUpdate();
    //
    splitter->setSizes(QList<int>() << 100 << 300);
    qvLogTimerId = startTimer(1000);
    //
    UpdateChecker.CheckUpdate();
}

void MainWindow::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == qvLogTimerId)
    {
        auto log = readLastLog().trimmed();
        if (!log.isEmpty())
        {
            FastAppendTextDocument(NEWLINE + log, qvLogDocument); /*end*/
            // qvLogDocument->setPlainText(qvLogDocument->toPlainText() + NEWLINE + log);
        }
    }
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
    if (focusWidget() == connectionListWidget)
    {
        CheckCurrentWidget;
        if (e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return)
        {
            // If pressed enter or return on connectionListWidget. Try to connect to the selected connection.
            if (widget->IsConnection())
            {
                widget->BeginConnection();
            }
            else
            {
                connectionListWidget->expandItem(connectionListWidget->currentItem());
            }
        }
        else if (e->key() == Qt::Key_F2)
        {
            widget->BeginRename();
        }
    }

    if (e->key() == Qt::Key_Escape)
    {
        auto widget = GetItemWidget(connectionListWidget->currentItem());
        // Check if this key was accpted by the ConnectionItemWidget
        if (widget && widget->IsRenaming())
        {
            widget->CancelRename();
            return;
        }
        else if (this->isActiveWindow())
        {
            this->close();
        }
    }
    else if (e->modifiers() & Qt::ControlModifier && e->key() == Qt::Key_Q)
    {
        if (QvMessageBoxAsk(this, tr("Quit Qv2ray"), tr("Are you sure to exit Qv2ray?"), QMessageBox::No) == QMessageBox::Yes)
        {
            ExitQv2ray();
        }
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *e)
{
    // Workaround of QtWidget not grabbing KeyDown and KeyUp in keyPressEvent
    if (e->key() == Qt::Key_Up || e->key() == Qt::Key_Down)
    {
        if (focusWidget() == connectionListWidget)
        {
            CheckCurrentWidget;
            on_connectionListWidget_itemClicked(connectionListWidget->currentItem(), 0);
        }
    }
}

void MainWindow::on_action_StartThis_triggered()
{
    CheckCurrentWidget;
    if (widget->IsConnection())
    {
        widget->BeginConnection();
    }
}

MainWindow::~MainWindow()
{
    hTray.hide();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
#ifdef Q_OS_MAC
    ProcessSerialNumber psn = { 0, kCurrentProcess };
    TransformProcessType(&psn, kProcessTransformToUIElementApplication);
#endif
    this->hide();
    tray_action_ShowHide->setText(tr("Show"));
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
#ifdef Q_OS_MAC
        ProcessSerialNumber psn = { 0, kCurrentProcess };
        TransformProcessType(&psn, kProcessTransformToForegroundApplication);
#endif
        tray_action_ShowHide->setText(tr("Hide"));
    }
    else
    {
#ifdef Q_OS_MAC
        ProcessSerialNumber psn = { 0, kCurrentProcess };
        TransformProcessType(&psn, kProcessTransformToUIElementApplication);
#endif
        this->hide();
        tray_action_ShowHide->setText(tr("Show"));
    }
}

void MainWindow::on_actionExit_triggered()
{
    ConnectionManager->StopConnection();
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
    vCoreLogDocument->clear();
}
void MainWindow::on_connectionListWidget_customContextMenuRequested(const QPoint &pos)
{
    Q_UNUSED(pos)
    auto _pos = QCursor::pos();
    auto item = connectionListWidget->itemAt(connectionListWidget->mapFromGlobal(_pos));
    if (item != nullptr)
    {
        bool isConnection = GetItemWidget(item)->IsConnection();
        // Disable connection-specific settings.
        action_RCM_Start->setEnabled(isConnection);
        action_RCM_SetAutoConnection->setEnabled(isConnection);
        action_RCM_Edit->setEnabled(isConnection);
        action_RCM_EditJson->setEnabled(isConnection);
        action_RCM_EditComplex->setEnabled(isConnection);
        action_RCM_Rename->setEnabled(isConnection);
        action_RCM_Duplicate->setEnabled(isConnection);
        connectionListRCM_Menu->popup(_pos);
    }
}

void MainWindow::on_action_RCM_DeleteThese_triggered()
{
    QList<ConnectionGroupPair> connlist;

    for (const auto &item : connectionListWidget->selectedItems())
    {
        auto widget = GetItemWidget(item);
        if (widget)
        {
            const auto identifier = widget->Identifier();
            if (widget->IsConnection())
            {
                connlist.append(identifier);
            }
            else
            {
                for (const auto &conns : ConnectionManager->GetGroupMetaObject(identifier.groupId).connections)
                {
                    ConnectionGroupPair i;
                    i.connectionId = conns;
                    i.groupId = identifier.groupId;
                    connlist.append(i);
                }
            }
        }
    }

    LOG(MODULE_UI, "Selected " + QSTRN(connlist.count()) + " items")

    if (connlist.isEmpty())
    {
        // Remove nothing means doing nothing.
        return;
    }

    if (QvMessageBoxAsk(this, tr("Removing Connection(s)"), tr("Are you sure to remove selected connection(s)?")) != QMessageBox::Yes)
    {
        return;
    }

    for (const auto &conn : connlist)
    {
        if (ConnectionManager->IsConnected(conn))
            ConnectionManager->StopConnection();
        if (GlobalConfig.autoStartId == conn)
            GlobalConfig.autoStartId.clear();

        ConnectionManager->DeleteConnectionFromGroup(conn.connectionId, conn.groupId);
    }
}

void MainWindow::on_importConfigButton_clicked()
{
    ImportConfigWindow w(this);
    w.ImportConnection();
}

void MainWindow::on_action_RCM_EditAsComplex_triggered()
{
    CheckCurrentWidget;
    if (widget->IsConnection())
    {
        auto id = widget->Identifier();
        CONFIGROOT root = ConnectionManager->GetConnectionRoot(id.connectionId);
        bool isChanged = false;
        //
        LOG(MODULE_UI, "INFO: Opening route editor.")
        RouteEditor routeWindow(root, this);
        root = routeWindow.OpenEditor();
        isChanged = routeWindow.result() == QDialog::Accepted;
        if (isChanged)
        {
            ConnectionManager->UpdateConnection(id.connectionId, root);
        }
    }
}

void MainWindow::on_subsButton_clicked()
{
    GroupManager().exec();
}

void MainWindow::on_connectionListWidget_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column)
    auto widget = GetItemWidget(item);
    if (widget == nullptr)
        return;
    if (widget->IsConnection())
    {
        widget->BeginConnection();
    }
}

void MainWindow::OnDisconnected(const ConnectionGroupPair &id)
{
    Q_UNUSED(id)
    hTray.setIcon(Q_TRAYICON("tray.png"));
    tray_action_Start->setEnabled(true);
    tray_action_Stop->setEnabled(false);
    tray_action_Restart->setEnabled(false);
    tray_SystemProxyMenu->setEnabled(false);
    lastConnectedIdentifier = id;
    locateBtn->setEnabled(false);
    if (!GlobalConfig.uiConfig.quietMode)
    {
        this->hTray.showMessage("Qv2ray", tr("Disconnected from: ") + GetDisplayName(id.connectionId), this->windowIcon());
    }
    hTray.setToolTip(TRAY_TOOLTIP_PREFIX);
    netspeedLabel->setText("0.00 B/s" NEWLINE "0.00 B/s");
    dataamountLabel->setText("0.00 B" NEWLINE "0.00 B");
    connetionStatusLabel->setText(tr("Not Connected"));
    if (GlobalConfig.inboundConfig.setSystemProxy)
    {
        ClearSystemProxy();
    }
}

void MainWindow::OnConnected(const ConnectionGroupPair &id)
{
    Q_UNUSED(id)
    hTray.setIcon(Q_TRAYICON("tray-connected.png"));
    tray_action_Start->setEnabled(false);
    tray_action_Stop->setEnabled(true);
    tray_action_Restart->setEnabled(true);
    tray_SystemProxyMenu->setEnabled(true);
    lastConnectedIdentifier = id;
    locateBtn->setEnabled(true);
    on_clearlogButton_clicked();
    auto name = GetDisplayName(id.connectionId);
    if (!GlobalConfig.uiConfig.quietMode)
    {
        this->hTray.showMessage("Qv2ray", tr("Connected: ") + name, this->windowIcon());
    }
    hTray.setToolTip(TRAY_TOOLTIP_PREFIX NEWLINE + tr("Connected: ") + name);
    connetionStatusLabel->setText(tr("Connected: ") + name);
    //
    ConnectionManager->StartLatencyTest(id.connectionId);
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
    auto widget = GetItemWidget(item);
    if (widget == nullptr)
        return;
    infoWidget->ShowDetails(widget->Identifier());
}

void MainWindow::OnStatsAvailable(const ConnectionGroupPair &id, const quint64 upS, const quint64 downS, const quint64 upD, const quint64 downD)
{
    if (!ConnectionManager->IsConnected(id))
        return;
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
    hTray.setToolTip(TRAY_TOOLTIP_PREFIX NEWLINE + tr("Connected: ") + GetDisplayName(id.connectionId) + //
                     NEWLINE "Up: " + totalSpeedUp + " Down: " + totalSpeedDown);
    //
    // Set data accordingly
    if (connectionNodes.contains(id))
    {
        connectionNodes.value(id)->setText(MW_ITEM_COL_DATA, NumericString(GetConnectionTotalData(id.connectionId)));
    }
}

void MainWindow::OnVCoreLogAvailable(const ConnectionGroupPair &id, const QString &log)
{
    Q_UNUSED(id);
    FastAppendTextDocument(log.trimmed(), vCoreLogDocument);
    // vCoreLogDocument->setPlainText(vCoreLogDocument->toPlainText() + log);
    // From https://gist.github.com/jemyzhang/7130092
    auto maxLines = GlobalConfig.uiConfig.maximumLogLines;
    auto block = vCoreLogDocument->begin();

    while (block.isValid())
    {
        if (vCoreLogDocument->blockCount() > maxLines)
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
void MainWindow::OnEditJsonRequested(const ConnectionId &id)
{
    JsonEditor w(ConnectionManager->GetConnectionRoot(id), this);
    auto root = CONFIGROOT(w.OpenEditor());

    if (w.result() == QDialog::Accepted)
    {
        ConnectionManager->UpdateConnection(id, root);
    }
}

void MainWindow::OnConnectionCreated(const ConnectionGroupPair &id, const QString &displayName)
{
    Q_UNUSED(displayName)
    MWAddConnectionItem_p(id);
}
void MainWindow::OnConnectionDeleted(const ConnectionGroupPair &id)
{
    auto child = connectionNodes.take(id);
    groupNodes.value(id.groupId)->removeChild(child.get());
}
void MainWindow::OnConnectionLinkedWithGroup(const ConnectionId &id, const GroupId &newGroup)
{
    MWAddConnectionItem_p({ id, newGroup });
}
void MainWindow::OnGroupCreated(const GroupId &id, const QString &displayName)
{
    Q_UNUSED(displayName)
    MWAddGroupItem_p(id);
}
void MainWindow::OnGroupDeleted(const GroupId &id, const QList<ConnectionId> &connections)
{
    for (const auto &conn : connections)
    {
        groupNodes.value(id)->removeChild(connectionNodes.value({ conn, id }).get());
    }
    groupNodes.remove(id);
}

void MainWindow::on_locateBtn_clicked()
{
    auto id = KernelInstance->CurrentConnection();
    if (!id.isEmpty())
    {
        connectionListWidget->setCurrentItem(connectionNodes.value(id).get());
        connectionListWidget->scrollToItem(connectionNodes.value(id).get());
        on_connectionListWidget_itemClicked(connectionNodes.value(id).get(), 0);
    }
}

void MainWindow::on_action_RCM_RenameThis_triggered()
{
    CheckCurrentWidget;
    widget->BeginRename();
}

void MainWindow::on_action_RCM_DuplicateThese_triggered()
{
    QList<ConnectionGroupPair> connlist;

    for (auto item : connectionListWidget->selectedItems())
    {
        auto widget = GetItemWidget(item);
        if (widget->IsConnection())
        {
            connlist.append(widget->Identifier());
        }
    }

    LOG(MODULE_UI, "Selected " + QSTRN(connlist.count()) + " items")

    if (connlist.count() > 1 && QvMessageBoxAsk(this, tr("Duplicating Connection(s)"), //
                                                tr("Are you sure to duplicate these connection(s)?")) != QMessageBox::Yes)
    {
        return;
    }

    for (const auto &conn : connlist)
    {
        ConnectionManager->CreateConnection(GetDisplayName(conn.connectionId) + tr(" (Copy)"), conn.groupId,
                                            ConnectionManager->GetConnectionRoot(conn.connectionId));
    }
}

void MainWindow::on_action_RCM_EditThis_triggered()
{
    CheckCurrentWidget;
    OnEditRequested(widget->Identifier().connectionId);
}

void MainWindow::on_action_RCM_EditAsJson_triggered()
{
    CheckCurrentWidget;
    OnEditJsonRequested(widget->Identifier().connectionId);
}

void MainWindow::on_chartVisibilityBtn_clicked()
{
    speedChartHolderWidget->setVisible(!speedChartWidget->isVisible());
}

void MainWindow::on_logVisibilityBtn_clicked()
{
    masterLogBrowser->setVisible(!masterLogBrowser->isVisible());
}

void MainWindow::on_clearChartBtn_clicked()
{
    speedChartWidget->Clear();
}

void MainWindow::on_connectionListWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    Q_UNUSED(previous)
    if (current != nullptr && !isExiting)
    {
        on_connectionListWidget_itemClicked(current, 0);
    }
}

void MainWindow::on_action_RCM_tovCoreLog_triggered()
{
    masterLogBrowser->setDocument(vCoreLogDocument);
}

void MainWindow::on_action_RCM_toQvLog_triggered()
{
    masterLogBrowser->setDocument(qvLogDocument);
}

void MainWindow::on_masterLogBrowser_textChanged()
{
    auto bar = masterLogBrowser->verticalScrollBar();
    bar->setValue(bar->maximum());
}

void MainWindow::on_action_RCM_SetAutoConnection_triggered()
{
    auto current = connectionListWidget->currentItem();
    if (current != nullptr)
    {
        auto widget = GetItemWidget(current);
        const auto identifier = widget->Identifier();
        GlobalConfig.autoStartId = identifier;
        if (!GlobalConfig.uiConfig.quietMode)
        {
            hTray.showMessage(tr("Set auto connection"), tr("Set %1 as auto connect.").arg(GetDisplayName(identifier.connectionId)));
        }
        SaveGlobalSettings();
    }
}

void MainWindow::on_action_RCM_ClearUsage_triggered()
{
    auto current = connectionListWidget->currentItem();
    if (current != nullptr)
    {
        auto widget = GetItemWidget(current);
        if (widget)
        {
            if (widget->IsConnection())
                ConnectionManager->ClearConnectionUsage(widget->Identifier());
            else
                ConnectionManager->ClearGroupUsage(widget->Identifier().groupId);
        }
    }
}

void MainWindow::on_action_RCM_LatencyTest_triggered()
{
    auto current = connectionListWidget->currentItem();
    if (current != nullptr)
    {
        auto widget = GetItemWidget(current);
        if (widget)
        {
            if (widget->IsConnection())
                ConnectionManager->StartLatencyTest(widget->Identifier().connectionId);
            else
                ConnectionManager->StartLatencyTest(widget->Identifier().groupId);
        }
    }
}

void MainWindow::on_pluginsBtn_clicked()
{
    PluginManageWindow(this).exec();
}
