#include "w_MainWindow.hpp"

#include "components/update/UpdateChecker.hpp"
#include "core/handler/ConfigHandler.hpp"
#include "core/settings/SettingsBackend.hpp"
#include "src/Qv2rayApplication.hpp"
#include "ui/common/UIBase.hpp"
#include "ui/editors/w_JsonEditor.hpp"
#include "ui/editors/w_OutboundEditor.hpp"
#include "ui/editors/w_RoutesEditor.hpp"
#include "ui/widgets/ConnectionInfoWidget.hpp"
#include "ui/windows/w_GroupManager.hpp"
#include "ui/windows/w_ImportConfig.hpp"
#include "ui/windows/w_PluginManager.hpp"
#include "ui/windows/w_PreferencesWindow.hpp"

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

QvMessageBusSlotImpl(MainWindow)
{
    switch (msg)
    {
        MBShowDefaultImpl;
        MBHideDefaultImpl;
        MBRetranslateDefaultImpl;
        MBUpdateColorSchemeDefaultImpl;
    }
}

void MainWindow::updateColorScheme()
{
    qvAppTrayIcon->setIcon(KernelInstance->CurrentConnection().isEmpty() ? Q_TRAYICON("tray.png") : Q_TRAYICON("tray-connected.png"));
    //
    importConfigButton->setIcon(QICON_R("import.png"));
    updownImageBox->setStyleSheet("image: url(" + QV2RAY_COLORSCHEME_ROOT + "netspeed_arrow.png)");
    updownImageBox_2->setStyleSheet("image: url(" + QV2RAY_COLORSCHEME_ROOT + "netspeed_arrow.png)");
    //
    tray_action_ToggleVisibility->setIcon(this->windowIcon());
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
    collapseGroupsBtn->setIcon(QICON_R("arrow-up.svg"));
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

void MainWindow::ReloadRecentConnectionList()
{
    QList<ConnectionGroupPair> newRecentConnections;
    const auto iterateRange = std::min(GlobalConfig.uiConfig.maxJumpListCount, GlobalConfig.uiConfig.recentConnections.count());
    for (auto i = 0; i < iterateRange; i++)
    {
        const auto &item = GlobalConfig.uiConfig.recentConnections.at(i);
        if (newRecentConnections.contains(item) || item.isEmpty())
            continue;
        newRecentConnections << item;
    }
    GlobalConfig.uiConfig.recentConnections = newRecentConnections;
}

void MainWindow::OnRecentConnectionsMenuReadyToShow()
{
    tray_RecentConnectionsMenu->clear();
    tray_RecentConnectionsMenu->addAction(tray_ClearRecentConnectionsAction);
    tray_RecentConnectionsMenu->addSeparator();
    for (const auto &conn : GlobalConfig.uiConfig.recentConnections)
    {
        if (ConnectionManager->IsValidId(conn))
            tray_RecentConnectionsMenu->addAction(GetDisplayName(conn.connectionId) + " (" + GetDisplayName(conn.groupId) + ")",
                                                  [=]() { emit ConnectionManager->StartConnection(conn); });
    }
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setupUi(this);
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
    updateColorScheme();
    //
    //
    connect(ConnectionManager, &QvConfigHandler::OnKernelCrashed, [this](const ConnectionGroupPair &, const QString &reason) {
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
    connect(ConnectionManager, &QvConfigHandler::OnConnectionRemovedFromGroup, this, &MainWindow::OnConnectionDeleted);
    connect(ConnectionManager, &QvConfigHandler::OnConnectionCreated, this, &MainWindow::OnConnectionCreated);
    connect(ConnectionManager, &QvConfigHandler::OnConnectionLinkedWithGroup, this, &MainWindow::OnConnectionLinkedWithGroup);
    //
    connect(ConnectionManager, &QvConfigHandler::OnGroupCreated, this, &MainWindow::OnGroupCreated);
    connect(ConnectionManager, &QvConfigHandler::OnGroupDeleted, this, &MainWindow::OnGroupDeleted);
    //
    connect(ConnectionManager, &QvConfigHandler::OnSubscriptionAsyncUpdateFinished, [](const GroupId &gid) {
        qvApp->showMessage(tr("Subscription \"%1\" has been updated").arg(GetDisplayName(gid))); //
    });
    //
    connect(ConnectionManager, &QvConfigHandler::OnConnectionRenamed, [this](const ConnectionId &id, const QString &, const QString &newName) {
        for (const auto &gid : ConnectionManager->GetGroupId(id))
        {
            ConnectionGroupPair pair{ id, gid };
            if (connectionNodes.contains(pair))
                connectionNodes.value(pair)->setText(MW_ITEM_COL_NAME, newName);
        }
    });
    connect(ConnectionManager, &QvConfigHandler::OnLatencyTestFinished, [this](const ConnectionId &id, const int avg) {
        for (const auto &gid : ConnectionManager->GetGroupId(id))
        {
            ConnectionGroupPair pair{ id, gid };
            if (connectionNodes.contains(pair))
                connectionNodes.value(pair)->setText(MW_ITEM_COL_PING, NumericString(avg)); //
        }
    });
    //
    connect(infoWidget, &ConnectionInfoWidget::OnEditRequested, this, &MainWindow::OnEditRequested);
    connect(infoWidget, &ConnectionInfoWidget::OnJsonEditRequested, this, &MainWindow::OnEditJsonRequested);
    //
    connect(masterLogBrowser->verticalScrollBar(), &QSlider::valueChanged, this, &MainWindow::OnLogScrollbarValueChanged);
    //
    // Setup System tray icons and menus
    qvAppTrayIcon->setToolTip(TRAY_TOOLTIP_PREFIX);
    qvAppTrayIcon->show();
    //
    // Basic tray actions
    tray_action_Start->setEnabled(true);
    tray_action_Stop->setEnabled(false);
    tray_action_Restart->setEnabled(false);
    //
    tray_SystemProxyMenu->setEnabled(false);
    tray_SystemProxyMenu->addAction(tray_action_SetSystemProxy);
    tray_SystemProxyMenu->addAction(tray_action_ClearSystemProxy);
    //
    tray_RootMenu->addAction(tray_action_ToggleVisibility);
    tray_RootMenu->addSeparator();
    tray_RootMenu->addAction(tray_action_Preferences);
    tray_RootMenu->addMenu(tray_SystemProxyMenu);
    //
    tray_RootMenu->addSeparator();
    tray_RootMenu->addMenu(tray_RecentConnectionsMenu);
    connect(tray_RecentConnectionsMenu, &QMenu::aboutToShow, this, &MainWindow::OnRecentConnectionsMenuReadyToShow);
    //
    tray_RootMenu->addSeparator();
    tray_RootMenu->addAction(tray_action_Start);
    tray_RootMenu->addAction(tray_action_Stop);
    tray_RootMenu->addAction(tray_action_Restart);
    tray_RootMenu->addSeparator();
    tray_RootMenu->addAction(tray_action_Quit);
    qvAppTrayIcon->setContextMenu(tray_RootMenu);
    //
    connect(tray_action_ToggleVisibility, &QAction::triggered, this, &MainWindow::ToggleVisibility);
    connect(tray_action_Preferences, &QAction::triggered, this, &MainWindow::on_preferencesBtn_clicked);
    connect(tray_action_Start, &QAction::triggered, [this] { ConnectionManager->StartConnection(lastConnectedIdentifier); });
    connect(tray_action_Stop, &QAction::triggered, ConnectionManager, &QvConfigHandler::StopConnection);
    connect(tray_action_Restart, &QAction::triggered, ConnectionManager, &QvConfigHandler::RestartConnection);
    connect(tray_action_Quit, &QAction::triggered, this, &MainWindow::on_actionExit_triggered);
    connect(tray_action_SetSystemProxy, &QAction::triggered, this, &MainWindow::MWSetSystemProxy);
    connect(tray_action_ClearSystemProxy, &QAction::triggered, this, &MainWindow::MWClearSystemProxy);
    connect(tray_ClearRecentConnectionsAction, &QAction::triggered, [this]() {
        GlobalConfig.uiConfig.recentConnections.clear();
        ReloadRecentConnectionList();
        if (!GlobalConfig.uiConfig.quietMode)
        {
            qvApp->showMessage(tr("Recent connections' jump list cleared."));
        }
    });
    connect(qvAppTrayIcon, &QSystemTrayIcon::activated, this, &MainWindow::on_activatedTray);
    //
    // Actions for right click the log text browser
    //
    logRCM_Menu->addAction(action_RCM_tovCoreLog);
    logRCM_Menu->addAction(action_RCM_toQvLog);
    connect(masterLogBrowser, &QTextBrowser::customContextMenuRequested, [this](const QPoint &) { logRCM_Menu->popup(QCursor::pos()); });
    connect(action_RCM_tovCoreLog, &QAction::triggered, this, &MainWindow::on_action_RCM_tovCoreLog_triggered);
    connect(action_RCM_toQvLog, &QAction::triggered, this, &MainWindow::on_action_RCM_toQvLog_triggered);
    masterLogBrowser->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    {
        auto font = masterLogBrowser->font();
        font.setPointSize(9);
        masterLogBrowser->setFont(font);
        qvLogDocument->setDefaultFont(font);
        vCoreLogDocument->setDefaultFont(font);
    }
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
    connectionListRCM_Menu->addAction(action_RCM_UpdateSubscription);
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
    connect(action_RCM_UpdateSubscription, &QAction::triggered, this, &MainWindow::on_action_RCM_UpdateSubscription_triggered);
    connect(action_RCM_Delete, &QAction::triggered, this, &MainWindow::on_action_RCM_DeleteThese_triggered);
    //
    // Sort Menu
    //
    connect(sortAction_SortByName_Asc, &QAction::triggered, [this] { SortConnectionList(MW_ITEM_COL_NAME, true); });
    connect(sortAction_SortByName_Dsc, &QAction::triggered, [this] { SortConnectionList(MW_ITEM_COL_NAME, false); });
    connect(sortAction_SortByData_Asc, &QAction::triggered, [this] { SortConnectionList(MW_ITEM_COL_DATA, true); });
    connect(sortAction_SortByData_Dsc, &QAction::triggered, [this] { SortConnectionList(MW_ITEM_COL_DATA, false); });
    connect(sortAction_SortByPing_Asc, &QAction::triggered, [this] { SortConnectionList(MW_ITEM_COL_PING, true); });
    connect(sortAction_SortByPing_Dsc, &QAction::triggered, [this] { SortConnectionList(MW_ITEM_COL_PING, false); });
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
    LOG(MODULE_UI, "Loading data...")
    for (const auto &group : ConnectionManager->AllGroups())
    {
        MWAddGroupItem_p(group);
        for (const auto &connection : ConnectionManager->Connections(group))
        {
            MWAddConnectionItem_p({ connection, group });
        }
    }
    //
    // Find and start if there is an auto-connection
    const auto connectionStarted = StartAutoConnectionEntry();
    if (!connectionStarted && connectionListWidget->topLevelItemCount() > 0)
    {
        ReloadRecentConnectionList();
        // Select the first connection.
        const auto &topLevelItem = connectionListWidget->topLevelItem(0);
        const auto &item = (topLevelItem->childCount() > 0) ? topLevelItem->child(0) : topLevelItem;
        connectionListWidget->setCurrentItem(item);
        on_connectionListWidget_itemClicked(item, 0);
    }
    //
    //
    tray_action_ToggleVisibility->setText(!connectionStarted ? tr("Hide") : tr("Show"));
    if (!connectionStarted)
        this->show();
    //
    CheckSubscriptionsUpdate();
    //
    splitter->setSizes({ 100, 300 });
    qvLogTimerId = startTimer(1000);
    //
    auto checker = new QvUpdateChecker(this);
    checker->CheckUpdate();
}

void MainWindow::ProcessCommand(QString command, QStringList commands, QMap<QString, QString> args)
{
    if (commands.isEmpty())
        return;
    if (command == "open")
    {
        const auto subcommand = commands.takeFirst();
        QvDialog *w;
        if (subcommand == "preference")
            w = new PreferencesWindow();
        else if (subcommand == "plugin")
            w = new PluginManageWindow();
        else if (subcommand == "group")
            w = new GroupManager();
        else if (subcommand == "import")
            w = new ImportConfigWindow();
        else
            return;
        w->processCommands(command, commands, args);
        w->exec();
        delete w;
    }
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
        else if (e->key() == Qt::Key_Delete)
        {
            on_action_RCM_DeleteThese_triggered();
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
        if (QvMessageBoxAsk(this, tr("Quit Qv2ray"), tr("Are you sure to exit Qv2ray?")) == QMessageBox::Yes)
            on_actionExit_triggered();
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
}

void MainWindow::closeEvent(QCloseEvent *event)
{
#ifdef Q_OS_MAC
    ProcessSerialNumber psn = { 0, kCurrentProcess };
    TransformProcessType(&psn, kProcessTransformToUIElementApplication);
#endif
    this->hide();
    tray_action_ToggleVisibility->setText(tr("Show"));
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
        tray_action_ToggleVisibility->setText(tr("Hide"));
    }
    else
    {
#ifdef Q_OS_MAC
        ProcessSerialNumber psn = { 0, kCurrentProcess };
        TransformProcessType(&psn, kProcessTransformToUIElementApplication);
#endif
        this->hide();
        tray_action_ToggleVisibility->setText(tr("Show"));
    }
}

void MainWindow::on_actionExit_triggered()
{
    ConnectionManager->StopConnection();
    qvApp->QuitApplication();
}

void MainWindow::on_preferencesBtn_clicked()
{
    PreferencesWindow{ this }.exec();
    // ProcessCommand("open", { "preference", "general" }, {});
}
void MainWindow::on_clearlogButton_clicked()
{
    masterLogBrowser->document()->clear();
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
        action_RCM_UpdateSubscription->setEnabled(!isConnection);
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

    const auto strRemoveConnTitle = tr("Removing Connection(s)", "", connlist.count());
    const auto strRemoveConnContent = tr("Are you sure to remove selected connection(s)?", "", connlist.count());
    if (QvMessageBoxAsk(this, strRemoveConnTitle, strRemoveConnContent) != QMessageBox::Yes)
    {
        return;
    }

    for (const auto &conn : connlist)
    {
        if (ConnectionManager->IsConnected(conn))
            ConnectionManager->StopConnection();
        if (GlobalConfig.autoStartId == conn)
            GlobalConfig.autoStartId.clear();

        ConnectionManager->RemoveConnectionFromGroup(conn.connectionId, conn.groupId);
    }
}

void MainWindow::on_importConfigButton_clicked()
{
    ImportConfigWindow w(this);
    w.PerformImportConnection();
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
    qvAppTrayIcon->setIcon(Q_TRAYICON("tray.png"));
    tray_action_Start->setEnabled(true);
    tray_action_Stop->setEnabled(false);
    tray_action_Restart->setEnabled(false);
    tray_SystemProxyMenu->setEnabled(false);
    lastConnectedIdentifier = id;
    locateBtn->setEnabled(false);
    if (!GlobalConfig.uiConfig.quietMode)
    {
        qvApp->showMessage(tr("Disconnected from: ") + GetDisplayName(id.connectionId), this->windowIcon());
    }
    qvAppTrayIcon->setToolTip(TRAY_TOOLTIP_PREFIX);
    netspeedLabel->setText("0.00 B/s" NEWLINE "0.00 B/s");
    dataamountLabel->setText("0.00 B" NEWLINE "0.00 B");
    connetionStatusLabel->setText(tr("Not Connected"));
    if (GlobalConfig.inboundConfig.systemProxySettings.setSystemProxy)
    {
        MWClearSystemProxy();
    }
}

void MainWindow::OnConnected(const ConnectionGroupPair &id)
{
    Q_UNUSED(id)
    qvAppTrayIcon->setIcon(Q_TRAYICON("tray-connected.png"));
    tray_action_Start->setEnabled(false);
    tray_action_Stop->setEnabled(true);
    tray_action_Restart->setEnabled(true);
    tray_SystemProxyMenu->setEnabled(true);
    lastConnectedIdentifier = id;
    locateBtn->setEnabled(true);
    on_clearlogButton_clicked();
    speedChartWidget->Clear();
    auto name = GetDisplayName(id.connectionId);
    if (!GlobalConfig.uiConfig.quietMode)
    {
        qvApp->showMessage(tr("Connected: ") + name, this->windowIcon());
    }
    qvAppTrayIcon->setToolTip(TRAY_TOOLTIP_PREFIX NEWLINE + tr("Connected: ") + name);
    connetionStatusLabel->setText(tr("Connected: ") + name);
    //
    GlobalConfig.uiConfig.recentConnections.removeAll(id);
    GlobalConfig.uiConfig.recentConnections.push_front(id);
    ReloadRecentConnectionList();
    //
    ConnectionManager->StartLatencyTest(id.connectionId);
    if (GlobalConfig.inboundConfig.systemProxySettings.setSystemProxy)
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

void MainWindow::OnStatsAvailable(const ConnectionGroupPair &id, const QMap<StatisticsType, QvStatsSpeedData> &data)
{
    if (!ConnectionManager->IsConnected(id))
        return;
    // This may not be, or may not precisely be, speed per second if the backend
    // has "any" latency. (Hope not...)
    //
    QMap<SpeedWidget::GraphID, long> pointData;
    bool isOutbound = GlobalConfig.uiConfig.graphConfig.useOutboundStats;
    bool hasDirect = isOutbound && GlobalConfig.uiConfig.graphConfig.hasDirectStats;
    for (const auto &type : data.keys())
    {
        const auto upSpeed = data[type].first.first;
        const auto downSpeed = data[type].first.second;
        switch (type)
        {
            case API_INBOUND:
                if (!isOutbound)
                {
                    pointData[SpeedWidget::INBOUND_UP] = upSpeed;
                    pointData[SpeedWidget::INBOUND_DOWN] = downSpeed;
                }
                break;
            case API_OUTBOUND_PROXY:
                if (isOutbound)
                {
                    pointData[SpeedWidget::OUTBOUND_PROXY_UP] = upSpeed;
                    pointData[SpeedWidget::OUTBOUND_PROXY_DOWN] = downSpeed;
                }
                break;
            case API_OUTBOUND_DIRECT:
                if (hasDirect)
                {
                    pointData[SpeedWidget::OUTBOUND_DIRECT_UP] = upSpeed;
                    pointData[SpeedWidget::OUTBOUND_DIRECT_DOWN] = downSpeed;
                }
                break;
            case API_OUTBOUND_BLACKHOLE: break;
        }
    }

    speedChartWidget->AddPointData(pointData);
    //
    const auto upSpeed = data[CurrentStatAPIType].first.first;
    const auto downSpeed = data[CurrentStatAPIType].first.second;
    auto totalSpeedUp = FormatBytes(upSpeed) + "/s";
    auto totalSpeedDown = FormatBytes(downSpeed) + "/s";
    auto totalDataUp = FormatBytes(data[CurrentStatAPIType].second.first);
    auto totalDataDown = FormatBytes(data[CurrentStatAPIType].second.second);
    //
    netspeedLabel->setText(totalSpeedUp + NEWLINE + totalSpeedDown);
    dataamountLabel->setText(totalDataUp + NEWLINE + totalDataDown);
    //
    qvAppTrayIcon->setToolTip(TRAY_TOOLTIP_PREFIX NEWLINE + tr("Connected: ") + GetDisplayName(id.connectionId) + //
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
    bool isChanged;

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
void MainWindow::OnConnectionLinkedWithGroup(const ConnectionGroupPair &pairId)
{
    MWAddConnectionItem_p(pairId);
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

void MainWindow::OnLogScrollbarValueChanged(int value)
{
    if (masterLogBrowser->verticalScrollBar()->maximum() == value)
        qvLogAutoScoll = true;
    else
        qvLogAutoScoll = false;
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

    for (const auto &item : connectionListWidget->selectedItems())
    {
        auto widget = GetItemWidget(item);
        if (widget->IsConnection())
        {
            connlist.append(widget->Identifier());
        }
    }

    LOG(MODULE_UI, "Selected " + QSTRN(connlist.count()) + " items")

    const auto strDupConnTitle = tr("Duplicating Connection(s)", "", connlist.count());
    const auto strDupConnContent = tr("Are you sure to duplicate these connection(s)?", "", connlist.count());
    if (connlist.count() > 1 && QvMessageBoxAsk(this, strDupConnTitle, strDupConnContent) != QMessageBox::Yes)
    {
        return;
    }

    for (const auto &conn : connlist)
    {
        ConnectionManager->CreateConnection(ConnectionManager->GetConnectionRoot(conn.connectionId),
                                            GetDisplayName(conn.connectionId) + tr(" (Copy)"), conn.groupId);
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
    if (!qvLogAutoScoll)
        return;
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
        GlobalConfig.autoStartBehavior = AUTO_CONNECTION_FIXED;
        if (!GlobalConfig.uiConfig.quietMode)
        {
            qvApp->showMessage(tr("%1 has been set as auto connect.").arg(GetDisplayName(identifier.connectionId)));
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

void MainWindow::on_action_RCM_UpdateSubscription_triggered()
{
    auto current = connectionListWidget->currentItem();
    if (current != nullptr)
    {
        auto widget = GetItemWidget(current);
        if (widget)
        {
            if (widget->IsConnection())
                return;
            const auto gid = widget->Identifier().groupId;
            if (ConnectionManager->GetGroupMetaObject(gid).isSubscription)
                ConnectionManager->UpdateSubscriptionAsync(gid);
            else
                QvMessageBoxInfo(this, tr("Update Subscription"), tr("Selected group is not a subscription"));
        }
    }
}

void MainWindow::on_action_RCM_LatencyTest_triggered()
{
    for (const auto &current : connectionListWidget->selectedItems())
    {
        if (!current)
            continue;
        const auto widget = GetItemWidget(current);
        if (!widget)
            continue;
        if (widget->IsConnection())
            ConnectionManager->StartLatencyTest(widget->Identifier().connectionId);
        else
            ConnectionManager->StartLatencyTest(widget->Identifier().groupId);
    }
}

void MainWindow::on_pluginsBtn_clicked()
{
    PluginManageWindow(this).exec();
}

void MainWindow::on_collapseGroupsBtn_clicked()
{
    connectionListWidget->collapseAll();
}
