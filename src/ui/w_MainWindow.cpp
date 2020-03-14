#include "w_MainWindow.hpp"

#include "components/pac/QvPACHandler.hpp"
#include "components/plugins/toolbar/QvToolbar.hpp"
#include "components/proxy/QvProxyConfigurator.hpp"
#include "core/connection/ConnectionIO.hpp"
#include "ui/editors/w_JsonEditor.hpp"
#include "ui/editors/w_OutboundEditor.hpp"
#include "ui/editors/w_RoutesEditor.hpp"
#include "ui/widgets/ConnectionInfoWidget.hpp"
#include "w_ImportConfig.hpp"
#include "w_PreferencesWindow.hpp"
#include "w_SubscriptionManager.hpp"

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

#define TRAY_TOOLTIP_PREFIX "Qv2ray " QV2RAY_VERSION_STRING
#define CheckCurrentWidget                                                                                                                      \
    auto widget = GetItemWidget(connectionListWidget->currentItem());                                                                           \
    if (widget == nullptr)                                                                                                                      \
        return;

#define GetItemWidget(item) (qobject_cast<ConnectionItemWidget *>(connectionListWidget->itemWidget(item, 0)))
#define NumericString(i) (QString("%1").arg(i, 30, 10, QLatin1Char('0')))

MainWindow *MainWindow::mwInstance = nullptr;

QvMessageBusSlotImpl(MainWindow)
{
    switch (msg)
    {
        MBShowDefaultImpl MBHideDefaultImpl MBRetranslateDefaultImpl MBUpdateColorSchemeDefaultImpl
    }
}

void MainWindow::UpdateColorScheme()
{
    hTray.setIcon(QIcon(GlobalConfig.uiConfig.useDarkTrayIcon ? ":/assets/icons/ui_dark/tray.png" : ":/assets/icons/ui_light/tray.png"));
    //
    importConfigButton->setIcon(QICON_R("import.png"));
    updownImageBox->setStyleSheet("image: url(" + QV2RAY_UI_COLORSCHEME_ROOT + "netspeed_arrow.png)");
    updownImageBox_2->setStyleSheet("image: url(" + QV2RAY_UI_COLORSCHEME_ROOT + "netspeed_arrow.png)");
    //
    tray_action_ShowHide->setIcon(this->windowIcon());
    action_RCM_Start->setIcon(QICON_R("connect.png"));
    action_RCM_Edit->setIcon(QICON_R("edit.png"));
    action_RCM_EditJson->setIcon(QICON_R("json.png"));
    action_RCM_EditComplex->setIcon(QICON_R("edit.png"));
    action_RCM_Duplicate->setIcon(QICON_R("duplicate.png"));
    action_RCM_Delete->setIcon(QICON_R("delete.png"));
    //
    locateBtn->setIcon(QICON_R("locate.png"));
    sortBtn->setIcon(QICON_R("sort.png"));
}

void MainWindow::MWAddConnectionItem_p(const ConnectionId &connection, const GroupId &groupId)
{
    if (!groupNodes.contains(groupId))
    {
        MWAddGroupItem_p(groupId);
    }
    auto groupItem = groupNodes.value(groupId);
    auto connectionItem = make_shared<QTreeWidgetItem>(QStringList{
        "",                                               //
        GetDisplayName(connection),                       //
        NumericString(GetConnectionLatency(connection)),  //
        "IMPORTTIME_NOT_SUPPORTED",                       //
        "LAST_CONNECTED_NOT_SUPPORTED",                   //
        NumericString(GetConnectionTotalData(connection)) //
    });
    connectionNodes.insert(connection, connectionItem);
    groupItem->addChild(connectionItem.get());
    auto widget = new ConnectionItemWidget(connection, connectionListWidget);
    connect(widget, &ConnectionItemWidget::RequestWidgetFocus, this, &MainWindow::OnConnectionWidgetFocusRequested);
    connectionListWidget->setItemWidget(connectionItem.get(), 0, widget);
}

void MainWindow::MWAddGroupItem_p(const GroupId &groupId)
{
    auto groupItem = make_shared<QTreeWidgetItem>(QStringList{ "", GetDisplayName(groupId) });
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

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setupUi(this);
    MainWindow::mwInstance = this;
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
    connect(ConnectionManager, &QvConfigHandler::OnCrashed, [&] {
        this->show();
        QvMessageBoxWarn(this, tr("V2ray vcore terminated."),
                         tr("V2ray vcore terminated unexpectedly.") + NEWLINE + NEWLINE +
                             tr("To solve the problem, read the V2ray log in the log text browser."));
    });
    //
    connect(ConnectionManager, &QvConfigHandler::OnConnected, this, &MainWindow::OnConnected);
    connect(ConnectionManager, &QvConfigHandler::OnDisconnected, this, &MainWindow::OnDisconnected);
    connect(ConnectionManager, &QvConfigHandler::OnStatsAvailable, this, &MainWindow::OnStatsAvailable);
    connect(ConnectionManager, &QvConfigHandler::OnVCoreLogAvailable, this, &MainWindow::OnVCoreLogAvailable);
    //
    connect(ConnectionManager, &QvConfigHandler::OnConnectionDeleted, this, &MainWindow::OnConnectionDeleted);
    connect(ConnectionManager, &QvConfigHandler::OnConnectionCreated, this, &MainWindow::OnConnectionCreated);
    connect(ConnectionManager, &QvConfigHandler::OnConnectionGroupChanged, this, &MainWindow::OnConnectionGroupChanged);
    //
    connect(ConnectionManager, &QvConfigHandler::OnGroupCreated, this, &MainWindow::OnGroupCreated);
    connect(ConnectionManager, &QvConfigHandler::OnGroupDeleted, this, &MainWindow::OnGroupDeleted);
    //
    connect(ConnectionManager, &QvConfigHandler::OnConnectionRenamed, [&](const ConnectionId &id, const QString &, const QString &newName) {
        if (connectionNodes.contains(id))
            connectionNodes.value(id)->setText(MW_ITEM_COL_NAME, newName); //
    });
    connect(ConnectionManager, &QvConfigHandler::OnLatencyTestFinished, [&](const ConnectionId &id, const uint avg) {
        if (connectionNodes.contains(id))
            connectionNodes.value(id)->setText(MW_ITEM_COL_PING, NumericString(avg)); //
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
    connect(tray_action_Start, &QAction::triggered, [&] { ConnectionManager->StartConnection(lastConnectedId); });
    connect(tray_action_Stop, &QAction::triggered, ConnectionManager, &QvConfigHandler::StopConnection);
    connect(tray_action_Restart, &QAction::triggered, ConnectionManager, &QvConfigHandler::RestartConnection);
    connect(tray_action_Quit, &QAction::triggered, this, &MainWindow::on_actionExit_triggered);
    connect(tray_action_SetSystemProxy, &QAction::triggered, this, &MainWindow::MWSetSystemProxy);
    connect(tray_action_ClearSystemProxy, &QAction::triggered, &ClearSystemProxy);
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
    connectionListRCM_Menu->addAction(action_RCM_Rename);
    connectionListRCM_Menu->addAction(action_RCM_Duplicate);
    connectionListRCM_Menu->addSeparator();
    connectionListRCM_Menu->addAction(action_RCM_Delete);
    connect(action_RCM_Start, &QAction::triggered, this, &MainWindow::on_action_StartThis_triggered);
    connect(action_RCM_Edit, &QAction::triggered, this, &MainWindow::on_action_RCM_EditThis_triggered);
    connect(action_RCM_EditJson, &QAction::triggered, this, &MainWindow::on_action_RCM_EditAsJson_triggered);
    connect(action_RCM_EditComplex, &QAction::triggered, this, &MainWindow::on_action_RCM_EditAsComplex_triggered);
    connect(action_RCM_Rename, &QAction::triggered, this, &MainWindow::on_action_RCM_RenameThis_triggered);
    connect(action_RCM_Duplicate, &QAction::triggered, this, &MainWindow::on_action_RCM_DuplicateThese_triggered);
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

        for (auto connection : connections)
        {
            MWAddConnectionItem_p(connection, group);
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
    else
    {
        if (connectionListWidget->topLevelItemCount() > 0 && connectionListWidget->topLevelItem(0)->childCount() > 0)
        {
            // Select the first connection.
            auto item = connectionListWidget->topLevelItem(0)->child(0);
            on_connectionListWidget_itemClicked(item, 0);
            connectionListWidget->setCurrentItem(item);
        }
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
    qvLogTimerId = startTimer(1000);
}

void MainWindow::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == qvLogTimerId)
    {
        auto log = readLastLog().trimmed();
        if (!log.isEmpty())
        {
            qvLogDocument->setPlainText(qvLogDocument->toPlainText() + NEWLINE + log);
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
        widget->keyPressEvent(e);
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
        tray_action_ShowHide->setText(tr("Hide"));
    }
    else
    {
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
    masterLogBrowser->clear();
}
void MainWindow::on_connectionListWidget_customContextMenuRequested(const QPoint &pos)
{
    Q_UNUSED(pos)
    auto _pos = QCursor::pos();
    auto item = connectionListWidget->itemAt(connectionListWidget->mapFromGlobal(_pos));
    if (item != nullptr)
    {
        if (GetItemWidget(item)->IsConnection())
        {
            connectionListRCM_Menu->popup(_pos);
        }
    }
}

void MainWindow::on_action_RCM_DeleteThese_triggered()
{
    QList<ConnectionId> connlist;

    for (auto item : connectionListWidget->selectedItems())
    {
        auto widget = GetItemWidget(item);
        if (widget->IsConnection())
        {
            connlist.append(get<1>(widget->Identifier()));
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

    for (auto conn : connlist)
    {
        if (ConnectionManager->IsConnected(conn))
            ConnectionManager->StopConnection();
        if (GlobalConfig.autoStartId == conn.toString())
            GlobalConfig.autoStartId.clear();

        ConnectionManager->DeleteConnection(conn);
    }
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

void MainWindow::on_action_RCM_EditAsComplex_triggered()
{
    CheckCurrentWidget;
    if (widget->IsConnection())
    {
        auto id = get<1>(widget->Identifier());
        CONFIGROOT root = ConnectionManager->GetConnectionRoot(id);
        bool isChanged = false;
        //
        LOG(MODULE_UI, "INFO: Opening route editor.")
        RouteEditor routeWindow(root, this);
        root = routeWindow.OpenEditor();
        isChanged = routeWindow.result() == QDialog::Accepted;
        if (isChanged)
        {
            ConnectionManager->UpdateConnection(id, root);
        }
    }
}

void MainWindow::on_subsButton_clicked()
{
    SubscribeEditor().exec();
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

void MainWindow::OnDisconnected(const ConnectionId &id)
{
    Q_UNUSED(id)
    tray_action_Start->setEnabled(true);
    tray_action_Stop->setEnabled(false);
    tray_action_Restart->setEnabled(false);
    tray_SystemProxyMenu->setEnabled(false);
    lastConnectedId = id;
    locateBtn->setEnabled(false);
    this->hTray.showMessage("Qv2ray", tr("Disconnected from: ") + GetDisplayName(id), this->windowIcon());
    hTray.setToolTip(TRAY_TOOLTIP_PREFIX NEWLINE);
    connetionStatusLabel->setText(tr("Not Connected"));
    if (GlobalConfig.inboundConfig.setSystemProxy)
    {
        ClearSystemProxy();
    }

    if (GlobalConfig.inboundConfig.pacConfig.enablePAC)
    {
        pacServer.StopServer();
        LOG(MODULE_UI, "Stopping PAC server")
    }
}

void MainWindow::OnConnected(const ConnectionId &id)
{
    Q_UNUSED(id)
    tray_action_Start->setEnabled(false);
    tray_action_Stop->setEnabled(true);
    tray_action_Restart->setEnabled(true);
    tray_SystemProxyMenu->setEnabled(true);
    lastConnectedId = id;
    locateBtn->setEnabled(true);
    on_clearlogButton_clicked();
    auto name = GetDisplayName(id);
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
            pacServer.SetProxyString(pacProxyString);
            pacServer.StartListen();
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
    auto widget = GetItemWidget(item);
    if (widget == nullptr)
        return;
    infoWidget->ShowDetails(widget->Identifier());
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
    hTray.setToolTip(TRAY_TOOLTIP_PREFIX NEWLINE + tr("Connected: ") + GetDisplayName(id) + //
                     NEWLINE "Up: " + totalSpeedUp + " Down: " + totalSpeedDown);
    //
    // Set data accordingly
    if (connectionNodes.contains(id))
    {
        connectionNodes.value(id)->setText(MW_ITEM_COL_DATA, NumericString(GetConnectionTotalData(id)));
    }
}

void MainWindow::OnVCoreLogAvailable(const ConnectionId &id, const QString &log)
{
    Q_UNUSED(id);
    vCoreLogDocument->setPlainText(vCoreLogDocument->toPlainText() + log);
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

void MainWindow::OnConnectionCreated(const ConnectionId &id, const QString &displayName)
{
    Q_UNUSED(displayName)
    MWAddConnectionItem_p(id, GetConnectionGroupId(id));
}
void MainWindow::OnConnectionDeleted(const ConnectionId &id, const GroupId &groupId)
{
    auto &child = connectionNodes.value(id);
    groupNodes.value(groupId)->removeChild(child.get());
    connectionNodes.remove(id);
}
void MainWindow::OnConnectionGroupChanged(const ConnectionId &id, const GroupId &originalGroup, const GroupId &newGroup)
{
    delete GetItemWidget(connectionNodes.value(id).get());
    groupNodes.value(originalGroup)->removeChild(connectionNodes.value(id).get());
    connectionNodes.remove(id);
    MWAddConnectionItem_p(id, newGroup);
}
void MainWindow::OnGroupCreated(const GroupId &id, const QString &displayName)
{
    Q_UNUSED(displayName)
    MWAddGroupItem_p(id);
}
void MainWindow::OnGroupDeleted(const GroupId &id, const QSet<ConnectionId> &connections)
{
    for (auto conn : connections)
    {
        groupNodes.value(id)->removeChild(connectionNodes.value(conn).get());
    }
    groupNodes.remove(id);
}

void MainWindow::on_locateBtn_clicked()
{
    auto id = ConnectionManager->CurrentConnection();
    if (id != NullConnectionId)
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
    QList<ConnectionId> connlist;

    for (auto item : connectionListWidget->selectedItems())
    {
        auto widget = GetItemWidget(item);
        if (widget->IsConnection())
        {
            connlist.append(get<1>(widget->Identifier()));
        }
    }

    LOG(MODULE_UI, "Selected " + QSTRN(connlist.count()) + " items")

    if (connlist.count() > 1 && QvMessageBoxAsk(this, tr("Duplicating Connection(s)"), //
                                                tr("Are you sure to duplicate these connection(s)?")) != QMessageBox::Yes)
    {
        return;
    }

    for (auto conn : connlist)
    {
        ConnectionManager->CreateConnection(GetDisplayName(conn) + tr(" (Copy)"), //
                                            GetConnectionGroupId(conn),           //
                                            ConnectionManager->GetConnectionRoot(conn));
    }
}

void MainWindow::on_action_RCM_EditThis_triggered()
{
    CheckCurrentWidget;
    OnEditRequested(get<1>(widget->Identifier()));
}

void MainWindow::on_action_RCM_EditAsJson_triggered()
{
    CheckCurrentWidget;
    OnEditJsonRequested(get<1>(widget->Identifier()));
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
