#include "w_MainWindow.hpp"

#include "components/pac/QvPACHandler.hpp"
#include "components/plugins/toolbar/QvToolbar.hpp"
#include "components/proxy/QvProxyConfigurator.hpp"
#include "core/connection/ConnectionIO.hpp"
#include "ui/editors/w_JsonEditor.hpp"
#include "ui/editors/w_OutboundEditor.hpp"
#include "ui/editors/w_RoutesEditor.hpp"
#include "ui/widgets/ConnectionInfoWidget.hpp"
#include "w_ExportConfig.hpp"
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
#define NumericString(i) (QString("%1").arg(i, 15, 10, QLatin1Char('0')))

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
    auto connectionItem = make_shared<QTreeWidgetItem>(QStringList{
        "",                                               //
        GetDisplayName(connection),                       //
        NumericString(GetConnectionLatency(connection)),  //
        "IMPORTTIME_NOT_SUPPORTED",                       //
        "LAST_CONNECTED_NOT_SUPPORTED",                   //
        NumericString(GetConnectionTotalData(connection)) //
    });
    connectionNodes[connection] = connectionItem;
    groupItem->addChild(connectionItem.get());
    auto widget = new ConnectionItemWidget(connection, connectionListWidget);
    connect(widget, &ConnectionItemWidget::RequestWidgetFocus, this, &MainWindow::OnConnectionWidgetFocusRequested);
    connectionListWidget->setItemWidget(connectionItem.get(), 0, widget);
}

void MainWindow::MWAddGroupItem_p(const GroupId &groupId)
{
    auto groupItem = make_shared<QTreeWidgetItem>(QStringList{ "", GetDisplayName(groupId) });
    groupNodes[groupId] = groupItem;
    connectionListWidget->addTopLevelItem(groupItem.get());
    connectionListWidget->setItemWidget(groupItem.get(), 0, new ConnectionItemWidget(groupId, connectionListWidget));
}

void MainWindow::SortConnectionList(MW_ITEM_COL byCol, bool asending)
{
    connectionListWidget->sortByColumn(MW_ITEM_COL_DISPLAYNAME, Qt::AscendingOrder);
    for (auto i = 0; i < connectionListWidget->topLevelItemCount(); i++)
    {
        connectionListWidget->topLevelItem(i)->sortChildren(byCol, asending ? Qt::AscendingOrder : Qt::DescendingOrder);
    }
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
    vCoreLogHighlighter = new SyntaxHighlighter(GlobalConfig.uiConfig.useDarkTheme, masterLogBrowser->document());
    // For charts
    speedChartWidget = new SpeedWidget(this);
    speedChart->addWidget(speedChartWidget);
    //
    this->setWindowIcon(QIcon(":/assets/icons/qv2ray.png"));
    hTray.setIcon(QIcon(GlobalConfig.uiConfig.useDarkTrayIcon ? ":/assets/icons/ui_dark/tray.png" : ":/assets/icons/ui_light/tray.png"));
    //
    importConfigButton->setIcon(QICON_R("import.png"));
    updownImageBox->setStyleSheet("image: url(" + QV2RAY_UI_RESOURCES_ROOT + "netspeed_arrow.png)");
    updownImageBox_2->setStyleSheet("image: url(" + QV2RAY_UI_RESOURCES_ROOT + "netspeed_arrow.png)");
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
        connectionNodes[id]->setText(MW_ITEM_COL_DISPLAYNAME, newName); //
    });
    connect(ConnectionManager, &QvConfigHandler::OnLatencyTestFinished, [&](const ConnectionId &id, const uint avg) {
        connectionNodes[id]->setText(MW_ITEM_COL_LATENCY, NumericString(avg)); //
    });
    //
    connect(infoWidget, &ConnectionInfoWidget::OnEditRequested, this, &MainWindow::OnEditRequested);
    connect(infoWidget, &ConnectionInfoWidget::OnJsonEditRequested, this, &MainWindow::OnEditJsonRequested);
    //
    // Setup System tray icons and menus
    hTray.setToolTip(TRAY_TOOLTIP_PREFIX);
    //
    // Basic actions
    action_Tray_Start->setEnabled(true);
    action_Tray_Stop->setEnabled(false);
    action_Tray_Restart->setEnabled(false);
    //
    tray_SystemProxyMenu->setTitle(tr("System Proxy"));
    tray_SystemProxyMenu->setEnabled(false);
    tray_SystemProxyMenu->addAction(action_Tray_SetSystemProxy);
    tray_SystemProxyMenu->addAction(action_Tray_ClearSystemProxy);
    //
    tray_RootMenu->addAction(action_Tray_ShowHide);
    tray_RootMenu->addSeparator();
    tray_RootMenu->addAction(action_Tray_ShowPreferencesWindow);
    tray_RootMenu->addMenu(tray_SystemProxyMenu);
    tray_RootMenu->addSeparator();
    tray_RootMenu->addAction(action_Tray_Start);
    tray_RootMenu->addAction(action_Tray_Stop);
    tray_RootMenu->addAction(action_Tray_Restart);
    tray_RootMenu->addSeparator();
    tray_RootMenu->addAction(action_Tray_Quit);
    //
    connect(action_Tray_ShowHide, &QAction::triggered, this, &MainWindow::ToggleVisibility);
    connect(action_Tray_ShowPreferencesWindow, &QAction::triggered, this, &MainWindow::on_preferencesBtn_clicked);
    //
    connect(action_Tray_Start, &QAction::triggered, [&] { ConnectionManager->StartConnection(lastConnectedId); });
    connect(action_Tray_Stop, &QAction::triggered, ConnectionManager, &QvConfigHandler::StopConnection);
    connect(action_Tray_Restart, &QAction::triggered, ConnectionManager, &QvConfigHandler::RestartConnection);
    //
    connect(action_Tray_Quit, &QAction::triggered, this, &MainWindow::on_actionExit_triggered);
    connect(action_Tray_SetSystemProxy, &QAction::triggered, this, &MainWindow::MWSetSystemProxy);
    connect(action_Tray_ClearSystemProxy, &QAction::triggered, &ClearSystemProxy);
    connect(&hTray, &QSystemTrayIcon::activated, this, &MainWindow::on_activatedTray);
    //
    // Actions for right click the connection list
    //
    QAction *action_RCM_StartThis = new QAction(QICON_R("connect.png"), tr("Connect to this"), this);
    //
    QAction *action_RCM_EditThis = new QAction(QICON_R("edit.png"), tr("Edit"), this);
    QAction *action_RCM_EditAsJson = new QAction(QICON_R("json.png"), tr("Edit as JSON"), this);
    QAction *action_RCM_EditAsComplex = new QAction(QICON_R("edit.png"), tr("Edit as Complex Config"), this);
    //
    QAction *action_RCM_RenameThis = new QAction(tr("Rename"), this);
    QAction *action_RCM_DuplicateThese = new QAction(QICON_R("duplicate.png"), tr("Duplicate to the Same Group"), this);
    QAction *action_RCM_DeleteThese = new QAction(QICON_R("delete.png"), tr("Delete Connection"), this);
    //
    connect(action_RCM_StartThis, &QAction::triggered, this, &MainWindow::on_action_StartThis_triggered);
    //
    connect(action_RCM_EditThis, &QAction::triggered, this, &MainWindow::on_action_RCM_EditThis_triggered);
    connect(action_RCM_EditAsJson, &QAction::triggered, this, &MainWindow::on_action_RCM_EditAsJson_triggered);
    connect(action_RCM_EditAsComplex, &QAction::triggered, this, &MainWindow::on_action_RCM_EditAsComplex_triggered);
    //
    connect(action_RCM_RenameThis, &QAction::triggered, this, &MainWindow::on_action_RCM_RenameThis_triggered);
    connect(action_RCM_DuplicateThese, &QAction::triggered, this, &MainWindow::on_action_RCM_DuplicateThese_triggered);
    connect(action_RCM_DeleteThese, &QAction::triggered, this, &MainWindow::on_action_RCM_DeleteThese_triggered);
    //
    // Globally invokable signals.
    connect(this, &MainWindow::StartConnection, ConnectionManager, &QvConfigHandler::RestartConnection);
    connect(this, &MainWindow::StopConnection, ConnectionManager, &QvConfigHandler::StopConnection);
    connect(this, &MainWindow::RestartConnection, ConnectionManager, &QvConfigHandler::RestartConnection);
    //
    hTray.setContextMenu(tray_RootMenu);
    hTray.show();
    //
    connectionListMenu = new QMenu(this);
    connectionListMenu->addAction(action_RCM_StartThis);
    connectionListMenu->addSeparator();
    connectionListMenu->addAction(action_RCM_EditThis);
    connectionListMenu->addAction(action_RCM_EditAsJson);
    connectionListMenu->addAction(action_RCM_EditAsComplex);
    connectionListMenu->addSeparator();
    connectionListMenu->addAction(action_RCM_RenameThis);
    connectionListMenu->addAction(action_RCM_DuplicateThese);
    connectionListMenu->addSeparator();
    connectionListMenu->addAction(action_RCM_DeleteThese);
    //
    QMenu *sortMenu = new QMenu(tr("Sort connection list."), this);
    QAction *sortAction_SortByName_Asc = new QAction(tr("By connection name, A-Z"));
    QAction *sortAction_SortByName_Dsc = new QAction(tr("By connection name, Z-A"));
    QAction *sortAction_SortByLatency_Asc = new QAction(tr("By data, Ascending"));
    QAction *sortAction_SortByLatency_Dsc = new QAction(tr("By data, Descending"));
    QAction *sortAction_SortByData_Asc = new QAction(tr("By latency, Ascending"));
    QAction *sortAction_SortByData_Dsc = new QAction(tr("By latency, Descending"));
    //
    connect(sortAction_SortByName_Asc, &QAction::triggered, [&] { SortConnectionList(MW_ITEM_COL_DISPLAYNAME, true); });
    connect(sortAction_SortByName_Dsc, &QAction::triggered, [&] { SortConnectionList(MW_ITEM_COL_DISPLAYNAME, false); });
    connect(sortAction_SortByData_Asc, &QAction::triggered, [&] { SortConnectionList(MW_ITEM_COL_DATAUSAGE, true); });
    connect(sortAction_SortByData_Dsc, &QAction::triggered, [&] { SortConnectionList(MW_ITEM_COL_DATAUSAGE, false); });
    connect(sortAction_SortByLatency_Asc, &QAction::triggered, [&] { SortConnectionList(MW_ITEM_COL_LATENCY, true); });
    connect(sortAction_SortByLatency_Dsc, &QAction::triggered, [&] { SortConnectionList(MW_ITEM_COL_LATENCY, false); });
    //
    sortMenu->addAction(sortAction_SortByName_Asc);
    sortMenu->addAction(sortAction_SortByName_Dsc);
    tray_RootMenu->addSeparator();
    sortMenu->addAction(sortAction_SortByData_Asc);
    sortMenu->addAction(sortAction_SortByData_Dsc);
    tray_RootMenu->addSeparator();
    sortMenu->addAction(sortAction_SortByLatency_Asc);
    sortMenu->addAction(sortAction_SortByLatency_Dsc);
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
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return)
    {
        if (focusWidget() == connectionListWidget)
        {
            CheckCurrentWidget;
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
        action_Tray_ShowHide->setText(tr("Hide"));
    }
    else
    {
        this->hide();
        action_Tray_ShowHide->setText(tr("Show"));
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
            connectionListMenu->popup(_pos);
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
    action_Tray_Start->setEnabled(true);
    action_Tray_Stop->setEnabled(false);
    action_Tray_Restart->setEnabled(false);
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
    action_Tray_Start->setEnabled(false);
    action_Tray_Stop->setEnabled(true);
    action_Tray_Restart->setEnabled(true);
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
    connectionNodes[id]->setText(MW_ITEM_COL_DATAUSAGE, NumericString(GetConnectionTotalData(id)));
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

void MainWindow::on_locateBtn_clicked()
{
    auto id = ConnectionManager->CurrentConnection();
    if (id != NullConnectionId)
    {
        connectionListWidget->setCurrentItem(connectionNodes[id].get());
        connectionListWidget->scrollToItem(connectionNodes[id].get());
        on_connectionListWidget_itemClicked(connectionNodes[id].get(), 0);
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
