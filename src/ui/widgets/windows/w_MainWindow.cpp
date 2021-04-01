#include "w_MainWindow.hpp"

#include "components/update/UpdateChecker.hpp"
#include "core/handler/ConfigHandler.hpp"
#include "core/settings/SettingsBackend.hpp"
#include "plugin-interface/gui/QvGUIPluginInterface.hpp"
#include "ui/widgets/Qv2rayWidgetApplication.hpp"
#include "ui/widgets/common/WidgetUIBase.hpp"
#include "ui/widgets/editors/w_JsonEditor.hpp"
#include "ui/widgets/editors/w_OutboundEditor.hpp"
#include "ui/widgets/editors/w_RoutesEditor.hpp"
#include "ui/widgets/widgets/ConnectionInfoWidget.hpp"
#include "ui/widgets/windows/w_GroupManager.hpp"
#include "ui/widgets/windows/w_ImportConfig.hpp"
#include "ui/widgets/windows/w_PluginManager.hpp"
#include "ui/widgets/windows/w_PreferencesWindow.hpp"

#include <QClipboard>
#include <QInputDialog>
#include <QScrollBar>

#define QV_MODULE_NAME "MainWindow"
#define TRAY_TOOLTIP_PREFIX "Qv2ray " QV2RAY_VERSION_STRING

#define CheckCurrentWidget                                                                                                                           \
    auto widget = GetIndexWidget(connectionTreeView->currentIndex());                                                                                \
    if (widget == nullptr)                                                                                                                           \
        return;

#define GetIndexWidget(item) (qobject_cast<ConnectionItemWidget *>(connectionTreeView->indexWidget(item)))
#define NumericString(i) (QString("%1").arg(i, 30, 10, QLatin1Char('0')))

#define PLUGIN_BUTTON_PROPERTY_KEY "plugin_list_index"

QvMessageBusSlotImpl(MainWindow)
{
    switch (msg)
    {
        MBShowDefaultImpl;
        MBHideDefaultImpl;
        MBUpdateColorSchemeDefaultImpl;
        case RETRANSLATE:
        {
            retranslateUi(this);
            UpdateActionTranslations();
            break;
        }
    }
}

void MainWindow::SortConnectionList(ConnectionInfoRole byCol, bool asending)
{
    modelHelper->Sort(byCol, asending ? Qt::AscendingOrder : Qt::DescendingOrder);
    on_locateBtn_clicked();
}

void MainWindow::ReloadRecentConnectionList()
{
    QList<ConnectionGroupPair> newRecentConnections;
    const auto iterateRange = std::min(GlobalConfig.uiConfig.maxJumpListCount, (int) GlobalConfig.uiConfig.recentConnections.count());
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
        {
            const auto name = GetDisplayName(conn.connectionId) + " (" + GetDisplayName(conn.groupId) + ")";
            tray_RecentConnectionsMenu->addAction(name, [=]() { emit ConnectionManager->StartConnection(conn); });
        }
    }
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), QvStateObject("MainWindow")
{
    setupUi(this);
    addStateOptions("width", { [&] { return width(); }, [&](QJsonValue val) { resize(val.toInt(), size().height()); } });
    addStateOptions("height", { [&] { return height(); }, [&](QJsonValue val) { resize(size().width(), val.toInt()); } });
    addStateOptions("x", { [&] { return x(); }, [&](QJsonValue val) { move(val.toInt(), y()); } });
    addStateOptions("y", { [&] { return y(); }, [&](QJsonValue val) { move(x(), val.toInt()); } });

#if 0
    const auto setSplitterSize = [&](QJsonValue val) { splitter->setSizes({ val.toArray()[0].toInt(), val.toArray()[1].toInt() }); };
    addStateOptions("splitterSizes", { [&] { return QJsonArray{ splitter->sizes()[0], splitter->sizes()[1] }; }, setSplitterSize });

    const auto setSpeedWidgetVisibility = [&](QJsonValue val) { speedChartHolderWidget->setVisible(val.toBool()); };
    const auto setLogWidgetVisibility = [&](QJsonValue val) { masterLogBrowser->setVisible(val.toBool()); };
    addStateOptions("speedchart.visibility", { [&] { return speedChartHolderWidget->isVisible(); }, setSpeedWidgetVisibility });
    addStateOptions("log.visibility", { [&] { return masterLogBrowser->isVisible(); }, setLogWidgetVisibility });
#else
    constexpr auto sizeRatioA = 0.382;
    constexpr auto sizeRatioB = 1 - sizeRatioA;
    splitter->setSizes({ (int) (width() * sizeRatioA), (int) (width() * sizeRatioB) });
#endif

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
    modelHelper = new ConnectionListHelper(connectionTreeView);
    //
    this->setWindowIcon(QIcon(":/assets/icons/qv2ray.png"));
    updateColorScheme();
    UpdateActionTranslations();
    //
    //
    connect(ConnectionManager, &QvConfigHandler::OnKernelCrashed, [this](const ConnectionGroupPair &, const QString &reason) {
        MWShowWindow();
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
    connect(ConnectionManager, &QvConfigHandler::OnSubscriptionAsyncUpdateFinished, [](const GroupId &gid) {
        QvWidgetApplication->ShowTrayMessage(tr("Subscription \"%1\" has been updated").arg(GetDisplayName(gid))); //
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
    tray_BypassCNMenu->setEnabled(false);
    tray_BypassCNMenu->addAction(tray_action_SetBypassCN);
    tray_BypassCNMenu->addAction(tray_action_ClearBypassCN);
    //
    tray_SystemProxyMenu->setEnabled(false);
    tray_SystemProxyMenu->addAction(tray_action_SetSystemProxy);
    tray_SystemProxyMenu->addAction(tray_action_ClearSystemProxy);
    //
    tray_RootMenu->addAction(tray_action_ToggleVisibility);
    tray_RootMenu->addSeparator();
    tray_RootMenu->addAction(tray_action_Preferences);
    tray_RootMenu->addMenu(tray_BypassCNMenu);
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
    connect(tray_action_ToggleVisibility, &QAction::triggered, this, &MainWindow::MWToggleVisibility);
    connect(tray_action_Preferences, &QAction::triggered, this, &MainWindow::on_preferencesBtn_clicked);
    connect(tray_action_Start, &QAction::triggered, [this] { ConnectionManager->StartConnection(lastConnected); });
    connect(tray_action_Stop, &QAction::triggered, ConnectionManager, &QvConfigHandler::StopConnection);
    connect(tray_action_Restart, &QAction::triggered, ConnectionManager, &QvConfigHandler::RestartConnection);
    connect(tray_action_Quit, &QAction::triggered, this, &MainWindow::Action_Exit);
    connect(tray_action_SetBypassCN, &QAction::triggered, this, &MainWindow::on_setBypassCNBtn_clicked);
    connect(tray_action_ClearBypassCN, &QAction::triggered, this, &MainWindow::on_clearBypassCNBtn_clicked);
    connect(tray_action_SetSystemProxy, &QAction::triggered, this, &MainWindow::MWSetSystemProxy);
    connect(tray_action_ClearSystemProxy, &QAction::triggered, this, &MainWindow::MWClearSystemProxy);
    connect(tray_ClearRecentConnectionsAction, &QAction::triggered, [this]() {
        GlobalConfig.uiConfig.recentConnections.clear();
        ReloadRecentConnectionList();
        if (!GlobalConfig.uiConfig.quietMode)
            QvWidgetApplication->ShowTrayMessage(tr("Recent Connection list cleared."));
    });
    connect(qvAppTrayIcon, &QSystemTrayIcon::activated, this, &MainWindow::on_activatedTray);
    //
    // Actions for right click the log text browser
    //
    logRCM_Menu->addAction(action_RCM_CopySelected);
    logRCM_Menu->addAction(action_RCM_CopyRecentLogs);
    logRCM_Menu->addSeparator();
    logRCM_Menu->addAction(action_RCM_SwitchCoreLog);
    logRCM_Menu->addAction(action_RCM_SwitchQv2rayLog);
    connect(masterLogBrowser, &QTextBrowser::customContextMenuRequested, [this](const QPoint &) { logRCM_Menu->popup(QCursor::pos()); });
    connect(action_RCM_SwitchCoreLog, &QAction::triggered, [this] { masterLogBrowser->setDocument(vCoreLogDocument); });
    connect(action_RCM_SwitchQv2rayLog, &QAction::triggered, [this] { masterLogBrowser->setDocument(qvLogDocument); });
    connect(action_RCM_CopyRecentLogs, &QAction::triggered, this, &MainWindow::Action_CopyRecentLogs);
    connect(action_RCM_CopySelected, &QAction::triggered, masterLogBrowser, &QTextBrowser::copy);
    //
    speedChartWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(speedChartWidget, &QWidget::customContextMenuRequested, [this](const QPoint &) { graphWidgetMenu->popup(QCursor::pos()); });
    //
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

    connectionListRCM_Menu->addAction(action_RCM_TestLatency);
    connectionListRCM_Menu->addAction(action_RCM_RealLatencyTest);

    connectionListRCM_Menu->addSeparator();
    connectionListRCM_Menu->addAction(action_RCM_SetAutoConnection);
    connectionListRCM_Menu->addSeparator();
    connectionListRCM_Menu->addAction(action_RCM_RenameConnection);
    connectionListRCM_Menu->addAction(action_RCM_DuplicateConnection);
    connectionListRCM_Menu->addAction(action_RCM_ResetStats);
    connectionListRCM_Menu->addAction(action_RCM_UpdateSubscription);
    connectionListRCM_Menu->addSeparator();

    connectionListRCM_Menu->addAction(action_RCM_DeleteConnection);

    //
    connect(action_RCM_Start, &QAction::triggered, this, &MainWindow::Action_Start);
    connect(action_RCM_SetAutoConnection, &QAction::triggered, this, &MainWindow::Action_SetAutoConnection);
    connect(action_RCM_Edit, &QAction::triggered, this, &MainWindow::Action_Edit);
    connect(action_RCM_EditJson, &QAction::triggered, this, &MainWindow::Action_EditJson);
    connect(action_RCM_EditComplex, &QAction::triggered, this, &MainWindow::Action_EditComplex);
    connect(action_RCM_TestLatency, &QAction::triggered, this, &MainWindow::Action_TestLatency);
    connect(action_RCM_RealLatencyTest, &QAction::triggered, this, &MainWindow::Action_TestRealLatency);
    connect(action_RCM_RenameConnection, &QAction::triggered, this, &MainWindow::Action_RenameConnection);
    connect(action_RCM_DuplicateConnection, &QAction::triggered, this, &MainWindow::Action_DuplicateConnection);
    connect(action_RCM_ResetStats, &QAction::triggered, this, &MainWindow::Action_ResetStats);
    connect(action_RCM_UpdateSubscription, &QAction::triggered, this, &MainWindow::Action_UpdateSubscription);
    connect(action_RCM_DeleteConnection, &QAction::triggered, this, &MainWindow::Action_DeleteConnections);
    //
    // Sort Menu
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
    connect(sortAction_SortByName_Asc, &QAction::triggered, [this] { SortConnectionList(ROLE_DISPLAYNAME, true); });
    connect(sortAction_SortByName_Dsc, &QAction::triggered, [this] { SortConnectionList(ROLE_DISPLAYNAME, false); });
    connect(sortAction_SortByData_Asc, &QAction::triggered, [this] { SortConnectionList(ROLE_DATA_USAGE, true); });
    connect(sortAction_SortByData_Dsc, &QAction::triggered, [this] { SortConnectionList(ROLE_DATA_USAGE, false); });
    connect(sortAction_SortByPing_Asc, &QAction::triggered, [this] { SortConnectionList(ROLE_LATENCY, true); });
    connect(sortAction_SortByPing_Dsc, &QAction::triggered, [this] { SortConnectionList(ROLE_LATENCY, false); });
    //
    sortBtn->setMenu(sortMenu);
    //
    graphWidgetMenu->addAction(action_RCM_CopyGraph);
    connect(action_RCM_CopyGraph, &QAction::triggered, this, &MainWindow::Action_CopyGraphAsImage);
    //
    // Find and start if there is an auto-connection
    const auto connectionStarted = StartAutoConnectionEntry();

    if (!connectionStarted && !ConnectionManager->GetConnections().isEmpty())
    {
        // Select the first connection.
        const auto groups = ConnectionManager->AllGroups();
        if (!groups.isEmpty())
        {
            const auto connections = ConnectionManager->GetConnections(groups.first());
            if (!connections.empty())
            {
                const auto index = modelHelper->GetConnectionPairIndex({ connections.first(), groups.first() });
                connectionTreeView->setCurrentIndex(index);
                on_connectionTreeView_clicked(index);
            }
        }
    }
    ReloadRecentConnectionList();
    //
    //
    if (!connectionStarted || !GlobalConfig.uiConfig.startMinimized)
        MWShowWindow();
    if (GlobalConfig.uiConfig.startMinimized)
        MWToggleVisibilitySetText();
    //
    CheckSubscriptionsUpdate();
    qvLogTimerId = startTimer(1000);
    auto checker = new QvUpdateChecker(this);
    checker->CheckUpdate();
    //
    for (const auto &name : PluginHost->UsablePlugins())
    {
        const auto &plugin = PluginHost->GetPlugin(name);
        if (!plugin->hasComponent(COMPONENT_GUI))
            continue;
        const auto guiInterface = plugin->pluginInterface->GetGUIInterface();
        if (!guiInterface)
            continue;
        if (!guiInterface->GetComponents().contains(GUI_COMPONENT_MAINWINDOW_WIDGET))
            continue;
        auto mainWindowWidgetPtr = guiInterface->GetMainWindowWidget();
        if (!mainWindowWidgetPtr)
            continue;
        const auto index = pluginWidgets.count();
        {
            // Let Qt manage the ownership.
            auto widget = mainWindowWidgetPtr.release();
            pluginWidgets.append(widget);
        }
        auto btn = new QPushButton(plugin->metadata.Name, this);
        connect(btn, &QPushButton::clicked, this, &MainWindow::OnPluginButtonClicked);
        btn->setProperty(PLUGIN_BUTTON_PROPERTY_KEY, index);
        topButtonsLayout->addWidget(btn);
    }
}

void MainWindow::OnPluginButtonClicked()
{
    const auto senderWidget = qobject_cast<QPushButton *>(sender());
    if (!senderWidget)
        return;
    bool ok = false;
    const auto index = senderWidget->property(PLUGIN_BUTTON_PROPERTY_KEY).toInt(&ok);
    if (!ok)
        return;
    const auto widget = pluginWidgets.at(index);
    if (!widget)
        return;
    widget->setVisible(!widget->isVisible());
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
        auto log = ReadLog().trimmed();
        if (!log.isEmpty())
        {
            FastAppendTextDocument(NEWLINE + log, qvLogDocument);
        }
    }
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
    if (focusWidget() == connectionTreeView)
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
                connectionTreeView->expand(connectionTreeView->currentIndex());
            }
        }
        else if (e->key() == Qt::Key_F2)
        {
            widget->BeginRename();
        }
        else if (e->key() == Qt::Key_Delete)
        {
            Action_DeleteConnections();
        }
    }

    if (e->key() == Qt::Key_Escape)
    {
        auto widget = GetIndexWidget(connectionTreeView->currentIndex());
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
    // Ctrl + Q = Exit
    else if (e->modifiers() & Qt::ControlModifier && e->key() == Qt::Key_Q)
    {
        if (QvMessageBoxAsk(this, tr("Quit Qv2ray"), tr("Are you sure to exit Qv2ray?")) == Yes)
            Action_Exit();
    }
    // Control + W = Close Window
    else if (e->modifiers() & Qt::ControlModifier && e->key() == Qt::Key_W)
    {
        if (this->isActiveWindow())
            this->close();
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *e)
{
    // Workaround of QtWidget not grabbing KeyDown and KeyUp in keyPressEvent
    if (e->key() == Qt::Key_Up || e->key() == Qt::Key_Down)
    {
        if (focusWidget() == connectionTreeView)
        {
            CheckCurrentWidget;
            on_connectionTreeView_clicked(connectionTreeView->currentIndex());
        }
    }
}

void MainWindow::changeEvent(QEvent *e)
{

    if (e->type() == QEvent::WindowStateChange)
    {
        MWToggleVisibilitySetText();
    }
}

void MainWindow::Action_Start()
{
    CheckCurrentWidget;
    if (widget->IsConnection())
    {
        widget->BeginConnection();
    }
}

MainWindow::~MainWindow()
{
#if QV2RAY_FEATURE(ui_has_store_state)
    SaveState();
#endif
    delete modelHelper;
    for (auto &widget : pluginWidgets)
        widget->accept();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    MWHideWindow();
    event->ignore();
}

void MainWindow::on_activatedTray(QSystemTrayIcon::ActivationReason reason)
{
#ifndef __APPLE__
    const auto toggleTriggerEvent = QSystemTrayIcon::Trigger;
#else
    const auto toggleTriggerEvent = QSystemTrayIcon::DoubleClick;
#endif
    if (reason == toggleTriggerEvent)
        MWToggleVisibility();
}

void MainWindow::Action_Exit()
{
    ConnectionManager->StopConnection();
    QvWidgetApplication->quit();
}

void MainWindow::on_preferencesBtn_clicked()
{
    PreferencesWindow{ this }.exec();
}

void MainWindow::on_setBypassCNBtn_clicked()
{
    GlobalConfig.defaultRouteConfig.connectionConfig.bypassCN = true;
    SaveGlobalSettings();
    if (!KernelInstance->CurrentConnection().isEmpty())
    {
        qApp->processEvents();
        ConnectionManager->RestartConnection();
    }
}

void MainWindow::on_clearBypassCNBtn_clicked()
{
    GlobalConfig.defaultRouteConfig.connectionConfig.bypassCN = false;
    SaveGlobalSettings();
    if (!KernelInstance->CurrentConnection().isEmpty())
    {
        qApp->processEvents();
        ConnectionManager->RestartConnection();
    }
}

void MainWindow::on_clearlogButton_clicked()
{
    masterLogBrowser->document()->clear();
}
void MainWindow::on_connectionTreeView_customContextMenuRequested(const QPoint &pos)
{
    Q_UNUSED(pos)
    auto _pos = QCursor::pos();
    auto item = connectionTreeView->indexAt(connectionTreeView->mapFromGlobal(_pos));
    if (item.isValid())
    {
        bool isConnection = GetIndexWidget(item)->IsConnection();
        // Disable connection-specific settings.
        action_RCM_Start->setEnabled(isConnection);
        action_RCM_SetAutoConnection->setEnabled(isConnection);
        action_RCM_Edit->setEnabled(isConnection);
        action_RCM_EditJson->setEnabled(isConnection);
        action_RCM_EditComplex->setEnabled(isConnection);
        action_RCM_RenameConnection->setEnabled(isConnection);
        action_RCM_DuplicateConnection->setEnabled(isConnection);
        action_RCM_UpdateSubscription->setEnabled(!isConnection);
        action_RCM_RealLatencyTest->setEnabled(isConnection && ConnectionManager->IsConnected(GetIndexWidget(item)->Identifier()));
        connectionListRCM_Menu->popup(_pos);
    }
}

void MainWindow::Action_DeleteConnections()
{
    QList<ConnectionGroupPair> connlist;
    QList<GroupId> groupsList;

    for (const auto &item : connectionTreeView->selectionModel()->selectedIndexes())
    {
        const auto widget = GetIndexWidget(item);
        if (!widget)
            continue;

        const auto identifier = widget->Identifier();
        if (widget->IsConnection())
        {
            // Simply add the connection id
            connlist.append(identifier);
            continue;
        }

        for (const auto &conns : ConnectionManager->GetConnections(identifier.groupId))
        {
            connlist.append(ConnectionGroupPair{ conns, identifier.groupId });
        }

        const auto message = tr("Do you want to remove this group as well?") + NEWLINE + tr("Group: ") + GetDisplayName(identifier.groupId);
        if (QvMessageBoxAsk(this, tr("Removing Connection"), message) == Yes)
        {
            groupsList << identifier.groupId;
        }
    }

    const auto strRemoveConnTitle = tr("Removing Connection(s)", "", connlist.count());
    const auto strRemoveConnContent = tr("Are you sure to remove selected connection(s)?", "", connlist.count());
    if (QvMessageBoxAsk(this, strRemoveConnTitle, strRemoveConnContent) != Yes)
    {
        return;
    }

    for (const auto &conn : connlist)
    {
        ConnectionManager->RemoveConnectionFromGroup(conn.connectionId, conn.groupId);
    }

    for (const auto &group : groupsList)
    {
        ConnectionManager->DeleteGroup(group);
    }
}

void MainWindow::on_importConfigButton_clicked()
{
    ImportConfigWindow w(this);
    w.PerformImportConnection();
}

void MainWindow::Action_EditComplex()
{
    CheckCurrentWidget;
    if (widget->IsConnection())
    {
        auto id = widget->Identifier();
        CONFIGROOT root = ConnectionManager->GetConnectionRoot(id.connectionId);
        bool isChanged = false;
        //
        LOG("INFO: Opening route editor.");
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

void MainWindow::OnDisconnected(const ConnectionGroupPair &id)
{
    Q_UNUSED(id)
    qvAppTrayIcon->setIcon(Q_TRAYICON("tray"));
    tray_action_Start->setEnabled(true);
    tray_action_Stop->setEnabled(false);
    tray_action_Restart->setEnabled(false);
    tray_BypassCNMenu->setEnabled(false);
    tray_SystemProxyMenu->setEnabled(false);
    lastConnected = id;
    locateBtn->setEnabled(false);
    if (!GlobalConfig.uiConfig.quietMode)
    {
        QvWidgetApplication->ShowTrayMessage(tr("Disconnected from: ") + GetDisplayName(id.connectionId));
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
    qvAppTrayIcon->setIcon(Q_TRAYICON("tray-connected"));
    tray_action_Start->setEnabled(false);
    tray_action_Stop->setEnabled(true);
    tray_action_Restart->setEnabled(true);
    tray_BypassCNMenu->setEnabled(true);
    tray_SystemProxyMenu->setEnabled(true);
    lastConnected = id;
    locateBtn->setEnabled(true);
    on_clearlogButton_clicked();
    auto name = GetDisplayName(id.connectionId);
    if (!GlobalConfig.uiConfig.quietMode)
    {
        QvWidgetApplication->ShowTrayMessage(tr("Connected: ") + name);
    }
    qvAppTrayIcon->setToolTip(TRAY_TOOLTIP_PREFIX NEWLINE + tr("Connected: ") + name);
    connetionStatusLabel->setText(tr("Connected: ") + name);
    //
    GlobalConfig.uiConfig.recentConnections.removeAll(id);
    GlobalConfig.uiConfig.recentConnections.push_front(id);
    ReloadRecentConnectionList();
    if (GlobalConfig.inboundConfig.systemProxySettings.setSystemProxy)
    {
        MWSetSystemProxy();
    }
}

void MainWindow::on_connectionFilterTxt_textEdited(const QString &arg1)
{
    modelHelper->Filter(arg1);
}

void MainWindow::OnStatsAvailable(const ConnectionGroupPair &id, const QMap<StatisticsType, QvStatsSpeedData> &data)
{
    if (!ConnectionManager->IsConnected(id))
        return;
    // This may not be, or may not precisely be, speed per second if the backend
    // has "any" latency. (Hope not...)
    //
    QMap<SpeedWidget::GraphType, long> pointData;
    bool isOutbound = GlobalConfig.uiConfig.graphConfig.useOutboundStats;
    bool hasDirect = isOutbound && GlobalConfig.uiConfig.graphConfig.hasDirectStats;
    for (const auto &[type, data] : data.toStdMap())
    {
        const auto upSpeed = data.first.first;
        const auto downSpeed = data.first.second;
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
        LOG("INFO: Opening route editor.");
        RouteEditor routeWindow(outBoundRoot, this);
        root = routeWindow.OpenEditor();
        isChanged = routeWindow.result() == QDialog::Accepted;
    }
    else
    {
        LOG("INFO: Opening single connection edit window.");
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
        const auto index = modelHelper->GetConnectionPairIndex(id);
        connectionTreeView->setCurrentIndex(index);
        connectionTreeView->scrollTo(index);
        on_connectionTreeView_clicked(index);
    }
}

void MainWindow::Action_RenameConnection()
{
    CheckCurrentWidget;
    widget->BeginRename();
}

void MainWindow::Action_DuplicateConnection()
{
    QList<ConnectionGroupPair> connlist;

    for (const auto &item : connectionTreeView->selectionModel()->selectedIndexes())
    {
        auto widget = GetIndexWidget(item);
        if (widget->IsConnection())
        {
            connlist.append(widget->Identifier());
        }
    }

    LOG("Selected ", connlist.count(), " items");

    const auto strDupConnTitle = tr("Duplicating Connection(s)", "", connlist.count());
    const auto strDupConnContent = tr("Are you sure to duplicate these connection(s)?", "", connlist.count());
    if (connlist.count() > 1 && QvMessageBoxAsk(this, strDupConnTitle, strDupConnContent) != Yes)
    {
        return;
    }

    for (const auto &conn : connlist)
    {
        ConnectionManager->CreateConnection(ConnectionManager->GetConnectionRoot(conn.connectionId),
                                            GetDisplayName(conn.connectionId) + tr(" (Copy)"), conn.groupId);
    }
}

void MainWindow::Action_Edit()
{
    CheckCurrentWidget;
    OnEditRequested(widget->Identifier().connectionId);
}

void MainWindow::Action_EditJson()
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

void MainWindow::on_masterLogBrowser_textChanged()
{
    if (!qvLogAutoScoll)
        return;
    auto bar = masterLogBrowser->verticalScrollBar();
    bar->setValue(bar->maximum());
}

void MainWindow::Action_SetAutoConnection()
{
    auto current = connectionTreeView->currentIndex();
    if (current.isValid())
    {
        auto widget = GetIndexWidget(current);
        const auto identifier = widget->Identifier();
        GlobalConfig.autoStartId = identifier;
        GlobalConfig.autoStartBehavior = AUTO_CONNECTION_FIXED;
        if (!GlobalConfig.uiConfig.quietMode)
        {
            QvWidgetApplication->ShowTrayMessage(tr("%1 has been set as auto connect.").arg(GetDisplayName(identifier.connectionId)));
        }
        SaveGlobalSettings();
    }
}

void MainWindow::Action_ResetStats()
{
    auto current = connectionTreeView->currentIndex();
    if (current.isValid())
    {
        auto widget = GetIndexWidget(current);
        if (widget)
        {
            if (widget->IsConnection())
                ConnectionManager->ClearConnectionUsage(widget->Identifier());
            else
                ConnectionManager->ClearGroupUsage(widget->Identifier().groupId);
        }
    }
}

void MainWindow::Action_UpdateSubscription()
{
    auto current = connectionTreeView->currentIndex();
    if (current.isValid())
    {
        auto widget = GetIndexWidget(current);
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

void MainWindow::Action_TestLatency()
{
    for (const auto &current : connectionTreeView->selectionModel()->selectedIndexes())
    {
        if (!current.isValid())
            continue;
        const auto widget = GetIndexWidget(current);
        if (!widget)
            continue;
        if (widget->IsConnection())
            ConnectionManager->StartLatencyTest(widget->Identifier().connectionId);
        else
            ConnectionManager->StartLatencyTest(widget->Identifier().groupId);
    }
}

void MainWindow::Action_TestRealLatency()
{
    for (const auto &current : connectionTreeView->selectionModel()->selectedIndexes())
    {
        if (!current.isValid())
            continue;
        const auto widget = GetIndexWidget(current);
        if (!widget)
            continue;
        if (widget->IsConnection() && ConnectionManager->IsConnected(widget->Identifier()))
            ConnectionManager->StartLatencyTest(widget->Identifier().connectionId, REALPING);
    }
}

void MainWindow::Action_CopyGraphAsImage()
{
    const auto image = speedChartWidget->grab();
    qApp->clipboard()->setImage(image.toImage());
}

void MainWindow::on_pluginsBtn_clicked()
{
    PluginManageWindow(this).exec();
}

void MainWindow::on_newConnectionBtn_clicked()
{
    OutboundEditor w(OUTBOUND{}, this);
    auto outboundEntry = w.OpenEditor();
    bool isChanged = w.result() == QDialog::Accepted;
    if (isChanged)
    {
        const auto alias = w.GetFriendlyName();
        OUTBOUNDS outboundsList;
        outboundsList.push_back(outboundEntry);
        CONFIGROOT root;
        root.insert("outbounds", outboundsList);
        const auto item = connectionTreeView->currentIndex();
        const auto id = item.isValid() ? GetIndexWidget(item)->Identifier().groupId : DefaultGroupId;
        ConnectionManager->CreateConnection(root, alias, id);
    }
}

void MainWindow::on_newComplexConnectionBtn_clicked()
{
    RouteEditor w({}, this);
    auto root = w.OpenEditor();
    bool isChanged = w.result() == QDialog::Accepted;
    if (isChanged)
    {
        const auto item = connectionTreeView->currentIndex();
        const auto id = item.isValid() ? GetIndexWidget(item)->Identifier().groupId : DefaultGroupId;
        ConnectionManager->CreateConnection(root, QJsonIO::GetValue(root, "outbounds", 0, "tag").toString(), id);
    }
}

void MainWindow::on_collapseGroupsBtn_clicked()
{
    connectionTreeView->collapseAll();
}

void MainWindow::Action_CopyRecentLogs()
{
    const auto lines = SplitLines(masterLogBrowser->document()->toPlainText());
    bool accepted = false;
    const auto line = QInputDialog::getInt(this, tr("Copy latest logs"), tr("Number of lines of logs to copy"), 20, 0, 2500, 1, &accepted);
    if (!accepted)
        return;
    const auto totalLinesCount = lines.count();
    const auto linesToCopy = std::min((int) totalLinesCount, line);
    QStringList result;
    for (auto i = totalLinesCount - linesToCopy; i < totalLinesCount; i++)
    {
        result.append(lines[i]);
    }
    qApp->clipboard()->setText(result.join(NEWLINE));
}

void MainWindow::on_connectionTreeView_doubleClicked(const QModelIndex &index)
{
    auto widget = GetIndexWidget(index);
    if (widget == nullptr)
        return;
    if (widget->IsConnection())
        widget->BeginConnection();
}

void MainWindow::on_connectionTreeView_clicked(const QModelIndex &index)
{
    auto widget = GetIndexWidget(index);
    if (widget == nullptr)
        return;
    infoWidget->ShowDetails(widget->Identifier());
}
