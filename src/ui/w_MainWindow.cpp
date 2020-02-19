#pragma once

#include "w_MainWindow.hpp"
#include "w_ImportConfig.hpp"
#include "w_PreferencesWindow.hpp"
#include "w_SubscriptionManager.hpp"
#include "w_ExportConfig.hpp"
#include "ui/editors/w_OutboundEditor.hpp"
#include "ui/editors/w_RoutesEditor.hpp"
#include "ui/editors/w_JsonEditor.hpp"
//#include <QAction>
#include <QCloseEvent>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QHeaderView>
#include <QInputDialog>
#include <QMenu>
#include <QStandardItemModel>
#include <QDesktopServices>
#include <QUrl>
#include <QVersionNumber>
#include <QKeyEvent>

#include "components/plugins/toolbar/QvToolbar.hpp"
#include "components/pac/QvPACHandler.hpp"

#include "core/connection/ConnectionIO.hpp"
#include "ui/widgets/ConnectionInfoWidget.hpp"

// MainWindow.cpp --> Main MainWindow source file, handles mostly UI-related operations.

#define TRAY_TOOLTIP_PREFIX "Qv2ray " QV2RAY_VERSION_STRING
//
#define GetItemWidget(item) (static_cast<ConnectionWidget*>(connectionListWidget->itemWidget(item, 0)))
//
//#define ItemConnectionIdentifier(__item__) (__item__->data(0, Qt::UserRole).value<ConnectionIdentifier>())
//
//#define CheckConfigType(_item_, TYPE) (connections.contains(ItemConnectionIdentifier(_item_)) && connections[ItemConnectionIdentifier(_item_)].configType == CONNECTION_ ## TYPE)
//
//#define SUBSCRIPTION_CONFIG_MODIFY_ASK(_item_)                                                                                                                  \
//    if (!CheckConfigType(_item_, REGULAR)) {                                                                                                                    \
//        if (QvMessageBoxAsk(this, QObject::tr("Editing a subscription config"), QObject::tr("You are trying to edit a config loaded from subscription.") +      \
//                            NEWLINE + QObject::tr("All changes will be overwritten when the subscriptions are updated next time.") +                            \
//                            NEWLINE + QObject::tr("Are you still going to do so?")) != QMessageBox::Yes) {                                                      \
//            return;                                                                                                                                             \
//        }                                                                                                                                                       \
//    }                                                                                                                                                           \
//
//
//#define SUBSCRIPTION_CONFIG_MODIFY_DENY(_item_)                                                                                                                 \
//    if (!CheckConfigType(_item_, REGULAR)) {                                                                                                                    \
//        QvMessageBoxWarn(this, QObject::tr("Editing a subscription config"), QObject::tr("You should not modity this property of a config from a subscription"));   \
//        return;                                                                                                                                                 \
//    }                                                                                                                                                           \
//
//#define IsConnectableItem(item) (item != nullptr && item->childCount() == 0 && (CheckConfigType(item, REGULAR) || CheckConfigType(item, SUBSCRIPTION)))
#define IsSelectionConnectable (!connectionListWidget->selectedItems().empty() && IsConnectableItem(connectionListWidget->selectedItems().first()))

// From https://gist.github.com/jemyzhang/7130092
#define CleanUpLogs(browser) \
    {\
        auto maxLines = GlobalConfig.uiConfig.maximumLogLines; \
        QTextBlock block = browser->document()->begin();\
        while (block.isValid()) {\
            if (browser->document()->blockCount() > maxLines) {\
                QTextCursor cursor(block);\
                block = block.next();\
                cursor.select(QTextCursor::BlockUnderCursor);\
                cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);\
                cursor.removeSelectedText();\
            } else {\
                break;\
            }\
        }\
    }

MainWindow *MainWindow::mwInstance = nullptr;

QvMessageBusSlotImpl(MainWindow)
{
    switch (msg) {
            QvMessageBusShowDefault
            QvMessageBusHideDefault
            QvMessageBusRetranslateDefault
    }
}

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent)//, vinstance(), hTray(this), tcpingHelper(3, this)
{
    setupUi(this);
    MainWindow::mwInstance = this;
    //vinstance = new V2rayKernelInstance();
    //connect(vinstance, &V2rayKernelInstance::onProcessOutputReadyRead, this, &MainWindow::UpdateVCoreLog);
    //connect(vinstance, &V2rayKernelInstance::onProcessErrored, [this] {
    //    on_stopButton_clicked();
    //    this->show();
    //    QvMessageBoxWarn(this, tr("V2ray vcore terminated."),
    //                     tr("V2ray vcore terminated unexpectedly.") + NEWLINE + NEWLINE +
    //                     tr("To solve the problem, read the V2ray log in the log text browser."));
    //});
    //
    QvMessageBusConnect(MainWindow);
    //
    infoWidget = new ConnectionInfoWidget(this);
    connectionInfoLayout->addWidget(infoWidget);
    //
    vCoreLogHighlighter = new SyntaxHighlighter(GlobalConfig.uiConfig.useDarkTheme, masterLogBrowser->document());
    masterLogBrowser->document()->setDocumentMargin(8);
    masterLogBrowser->document()->adjustSize();
    masterLogBrowser->setLineWrapMode(QTextBrowser::LineWrapMode::NoWrap);
    //
    requestHelper = new QvHttpRequestHelper();
    //connect(&tcpingHelper, &QvTCPingModel::PingFinished, this, &MainWindow::onPingFinished);
    //
    this->setWindowIcon(QIcon(":/assets/icons/qv2ray.png"));
    hTray.setIcon(QIcon(GlobalConfig.uiConfig.useDarkTrayIcon ? ":/assets/icons/ui_dark/tray.png" : ":/assets/icons/ui_light/tray.png"));
    importConfigButton->setIcon(QICON_R("import.png"));
    duplicateBtn->setIcon(QICON_R("duplicate.png"));
    removeConfigButton->setIcon(QICON_R("delete.png"));
    //editConfigButton->setIcon(QICON_R("edit.png"));
    //editJsonBtn->setIcon(QICON_R("json.png"));
    ////
    //pingTestBtn->setIcon(QICON_R("ping_gauge.png"));
    //shareBtn->setIcon(QICON_R("share.png"));
    updownImageBox->setStyleSheet("image: url(" + QV2RAY_UI_RESOURCES_ROOT + "netspeed_arrow.png)");
    updownImageBox_2->setStyleSheet("image: url(" + QV2RAY_UI_RESOURCES_ROOT + "netspeed_arrow.png)");
    //
    // Setup System tray icons and menus
    //
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
    connect(action_Tray_Start, &QAction::triggered, this, &MainWindow::on_startButton_clicked);
    connect(action_Tray_Stop, &QAction::triggered, this, &MainWindow::on_stopButton_clicked);
    connect(action_Tray_Reconnect, &QAction::triggered, this, &MainWindow::on_reconnectButton_clicked);
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
    connect(this, &MainWindow::Connect, this, &MainWindow::on_startButton_clicked);
    connect(this, &MainWindow::DisConnect, this, &MainWindow::on_stopButton_clicked);
    connect(this, &MainWindow::ReConnect, this, &MainWindow::on_reconnectButton_clicked);
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
    OnConfigListChanged(false);
    //
    // For charts
    speedChartView = new SpeedWidget(this);
    //speedChartView->setContentsMargins(1, 1, 1, 1);
    speedChart->addWidget(speedChartView);
    //
    // Find and start if there is an auto-connection
    MWFindAndStartAutoConfig();
    //// If we are not connected to anything, show the MainWindow.
    //if (!vinstance->KernelStarted) {
    //    this->show();
    //}
    connect(requestHelper, &QvHttpRequestHelper::httpRequestFinished, this, &MainWindow::VersionUpdate);
    requestHelper->get("https://api.github.com/repos/Qv2ray/Qv2ray/releases/latest");

    if (StartupOption.enableToolbarPlguin) {
        LOG(MODULE_UI, "Plugin daemon is enabled.")
        StartProcessingPlugins();
    }

    CheckSubscriptionsUpdate();
}

void MainWindow::SetEditWidgetEnable(bool enabled)
{
    removeConfigButton->setEnabled(enabled);
    //editConfigButton->setEnabled(enabled);
    duplicateBtn->setEnabled(enabled);
    //editJsonBtn->setEnabled(enabled);
    //shareBtn->setEnabled(enabled);
    // Allow ping all.
    //pingTestBtn->setText(enabled ? tr("Ping") : tr("Ping All"));
}

void MainWindow::mouseReleaseEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
    //// If mouse is on the logLabel, switch log source.
    //if (logbox->underMouse()) {
    //    //auto layout = masterLogBrowser->document()->setDocumentLayout()
    //    currentLogBrowserId = (currentLogBrowserId + 1) % logTextBrowsers.count();
    //    masterLogBrowser->setDocument(currentLogBrowser->document());
    //    masterLogBrowser->document()->setDocumentMargin(4);
    //    masterLogBrowser->document()->adjustSize();
    //    masterLogBrowser->setLineWrapMode(QTextBrowser::LineWrapMode::NoWrap);
    //    auto bar = masterLogBrowser->verticalScrollBar();
    //    bar->setValue(bar->maximum());
    //}
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return) {
        // If pressed enter or return on connectionListWidget.
        // Try to connect to the selected connection.
        //if (focusWidget() == connectionListWidget) {
        //    if (!IsSelectionConnectable) return;
        //
        //    auto selections = connectionListWidget->selectedItems();
        //    QVariant v;
        //    auto vv = v.value<QvConnectionObject>();
        //    ShowAndSetConnection(ItemConnectionIdentifier(selections.first()), true, true);
        //}
    }
}

void MainWindow::on_action_StartThis_triggered()
{
    //if (!IsSelectionConnectable) {
    //    QvMessageBoxWarn(this, tr("No connection selected!"), tr("Please select a config from the list."));
    //    return;
    //}
    //
    //CurrentSelectedItemPtr = connectionListWidget->selectedItems().first();
    //CurrentConnectionIdentifier = ItemConnectionIdentifier(CurrentSelectedItemPtr);
    //on_reconnectButton_clicked();
}
void MainWindow::VersionUpdate(QByteArray &data)
{
    // Version update handler.
    QJsonObject root = JsonFromString(QString(data));
    //
    QVersionNumber newVersion = QVersionNumber::fromString(root["tag_name"].toString("v").remove(0, 1));
    QVersionNumber currentVersion = QVersionNumber::fromString(QString(QV2RAY_VERSION_STRING).remove(0, 1));
    QVersionNumber ignoredVersion = QVersionNumber::fromString(GlobalConfig.ignoredVersion);
    LOG(MODULE_UPDATE, "Received update info, Latest: " + newVersion.toString() + " Current: " + currentVersion.toString() + " Ignored: " + ignoredVersion.toString())

    // If the version is newer than us.
    // And new version is newer than the ignored version.
    if (newVersion > currentVersion && newVersion > ignoredVersion) {
        LOG(MODULE_UPDATE, "New version detected.")
        auto link = root["html_url"].toString("");
        auto result = QvMessageBoxAsk(this, tr("Update"),
                                      tr("Found a new version: ") + root["tag_name"].toString("") +
                                      "\r\n" +
                                      root["name"].toString("") +
                                      "\r\n------------\r\n" +
                                      root["body"].toString("") +
                                      "\r\n------------\r\n" +
                                      tr("Download Link: ") + link, QMessageBox::Ignore);

        if (result == QMessageBox::Yes) {
            QDesktopServices::openUrl(QUrl::fromUserInput(link));
        } else if (result == QMessageBox::Ignore) {
            // Set and save ingored version.
            GlobalConfig.ignoredVersion = newVersion.toString();
            //SaveGlobalConfig(GlobalConfig);
        }
    }
}

void MainWindow::OnConfigListChanged(bool need_restart)
{
    LOG(MODULE_UI, "Loading data...")
    auto groups = ConnectionHandler->Groups();

    for (auto group : groups) {
        auto groupItem = new QTreeWidgetItem();
        connectionListWidget->addTopLevelItem(groupItem);
        connectionListWidget->setItemWidget(groupItem, 0, new ConnectionWidget(group, connectionListWidget));
        auto connections = ConnectionHandler->Connections(group);

        for (auto connection : connections) {
            auto connectionItem = new QTreeWidgetItem();
            groupItem->addChild(connectionItem);
            auto widget = new ConnectionWidget(connection, connectionListWidget);
            connect(widget, &ConnectionWidget::RequestWidgetFocus, this, &MainWindow::onConnectionWidgetFocusRequested);
            connectionListWidget->setItemWidget(connectionItem, 0, widget);
        }
    }

    auto subscriptions = ConnectionHandler->Subscriptions();

    for (auto subscription : subscriptions) {
        auto subscriptionItem = new QTreeWidgetItem();
        connectionListWidget->addTopLevelItem(subscriptionItem);
        connectionListWidget->setItemWidget(subscriptionItem, 0, new ConnectionWidget(subscription, connectionListWidget));
        auto connections = ConnectionHandler->Connections(subscription);

        for (auto connection : connections) {
            auto connectionItem = new QTreeWidgetItem();
            subscriptionItem->addChild(connectionItem);
            auto widget = new ConnectionWidget(connection, connectionListWidget);
            connect(widget, &ConnectionWidget::RequestWidgetFocus, this, &MainWindow::onConnectionWidgetFocusRequested);
            connectionListWidget->setItemWidget(connectionItem, 0, widget);
        }
    }

    //auto wasRunning = vinstance->KernelStarted && need_restart;
    //
    //if (wasRunning) on_stopButton_clicked();
    //
    //LOG(UI, "Loading new GlobalConfig")
    //SetEditWidgetEnable(false);
    //
    // Store the latency test value.
    //QMap<QvConnectionObject, double> latencyValueCache;
    //for (auto i = 0; i < connections.count(); i++) {
    //    latencyValueCache[connections.keys()[i]] = connections.values()[i].latency;
    //}
    //connections.clear();
    //connectionListWidget->clear();
    //QvMessageBoxInfo(this, "NOT SUPPORTED", "WIP, only subscriptions are supported");
    ////auto _regularConnections = GetRegularConnections(GlobalConfig.configs);
    //auto _subsConnections = GetSubscriptionConnections(GlobalConfig.subscriptions.keys());
    //for (auto i = 0; i < _regularConnections.count(); i++) {
    //    ConnectionObject _o;
    //    _o.configType = CONNECTION_REGULAR;
    //    _o.connectionName = _regularConnections.keys()[i];
    //    _o.config = _regularConnections.values()[i];
    //    auto name = _o.IdentifierString();
    //    _o.latency = latencyValueCache[name]; // restore latency values
    //    connections[name] = _o;
    //    auto item = new QTreeWidgetItem(QStringList() << _o.connectionName);
    //    item->setData(0, Qt::UserRole, QVariant::fromValue<ConnectionIdentifier>(_o));
    //    connectionListWidget->addTopLevelItem(item);
    //}
    //for (auto i = 0; i < _subsConnections.count(); i++) {
    //    auto subName = _subsConnections.keys()[i];
    //    auto subTopLevelItem = new QTreeWidgetItem(QStringList() << tr("Subscription") + ": " + subName);
    //    connectionListWidget->addTopLevelItem(subTopLevelItem);
    //
    //    for (auto j = 0; j < _subsConnections.values()[i].count(); j++) {
    //        ConnectionObject _o;
    //        _o.configType = CONNECTION_SUBSCRIPTION;
    //        _o.connectionName = _subsConnections.values()[i].keys()[j];
    //        _o.subscriptionName = subName;
    //        _o.config = _subsConnections.values()[i].values()[j];
    //        // connection name generated from subscription name and connection name.
    //        auto connName = _o.IdentifierString();
    //        _o.latency = latencyValueCache[connName];
    //        connections[connName] = _o;
    //        auto item = new QTreeWidgetItem(QStringList() << _o.connectionName);
    //        item->setData(0, Qt::UserRole, QVariant::fromValue<QvConnectionObject>(_o));
    //        subTopLevelItem->addChild(item);
    //    }
    //}
    //
    //// We set the current selected item back...
    //if (connections.contains(CurrentConnectionIdentifier)) {
    //    auto item = FindItemByIdentifier(CurrentConnectionIdentifier);
    //
    //    if (item != nullptr) {
    //        connectionListWidget->setCurrentItem(item);
    //        connectionListWidget->scrollToItem(item);
    //    } else if (connectionListWidget->topLevelItemCount() > 0) {
    //        item = connectionListWidget->topLevelItem(0);
    //        CurrentConnectionIdentifier = ItemConnectionIdentifier(item);
    //    } else {
    //        return;
    //    }
    //
    //    ShowAndSetConnection(CurrentConnectionIdentifier, false, false);
    //}
    //
    //connectionListWidget->sortItems(0, Qt::AscendingOrder);
    //
    //if (wasRunning) on_startButton_clicked();
}
MainWindow::~MainWindow()
{
    hTray.hide();
}
void MainWindow::UpdateVCoreLog(const QString &log)
{
    masterLogBrowser->append(log);
    CleanUpLogs(masterLogBrowser)
    auto bar = masterLogBrowser->verticalScrollBar();
    auto max = bar->maximum();
    auto val = bar->value();

    if (val >= max * 0.8 || val >= max - 20)
        bar->setValue(max);
}
void MainWindow::on_startButton_clicked()
{
}

void MainWindow::on_stopButton_clicked()
{
    //// Is running or starting
    //killTimer(speedTimerId);
    //killTimer(pingTimerId);
    ////
    //MWStopConnection();
    ////
    //hTray.setToolTip(TRAY_TOOLTIP_PREFIX);
    //statusLabel->setText(tr("Disconnected"));
    action_Tray_Start->setEnabled(true);
    action_Tray_Stop->setEnabled(false);
    action_Tray_Reconnect->setEnabled(false);
    // Set to false as the system proxy has been cleared in the StopConnection function.
    tray_SystemProxyMenu->setEnabled(false);
    startButton->setEnabled(true);
    stopButton->setEnabled(false);
    ////
    //netspeedLabel->setText("0.00 B/s\r\n0.00 B/s");
    //dataamountLabel->setText("0.00 B\r\n0.00 B");
    //LOG(UI, "Stopped successfully.")
    //this->hTray.showMessage("Qv2ray", tr("Disconnected from: ") + CurrentConnectionIdentifier.IdentifierString());
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    this->hide();
    tray_RootMenu->actions().first()->setText(tr("Show"));
    event->ignore();
}
void MainWindow::on_activatedTray(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
        case QSystemTrayIcon::Trigger:
            // Toggle Show/Hide
#ifndef __APPLE__
            // Every single click will trigger the Show/Hide toggling.
            // So, as what common macOS Apps do, we don't toggle visibility here.
            ToggleVisibility();
#endif
            break;

        case QSystemTrayIcon::DoubleClick:
#ifdef __APPLE__
            ToggleVisibility();
#endif
            break;

        default:
            break;
    }
}
void MainWindow::ToggleVisibility()
{
    if (this->isHidden()) {
        this->show();
#ifdef Q_OS_WIN
        setWindowState(Qt::WindowNoState);
        SetWindowPos(HWND(this->winId()), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
        QThread::msleep(20);
        SetWindowPos(HWND(this->winId()), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
#endif
        tray_RootMenu->actions()[0]->setText(tr("Hide"));
    } else {
        this->hide();
        tray_RootMenu->actions()[0]->setText(tr("Show"));
    }
}

void MainWindow::on_actionExit_triggered()
{
    if (StartupOption.enableToolbarPlguin) {
        StopProcessingPlugins();
    }

    //tcpingHelper.StopAllPing();
    on_stopButton_clicked();
    ExitQv2ray();
}

void MainWindow::on_preferencesBtn_clicked()
{
    PreferencesWindow w(this);
    connect(&w, &PreferencesWindow::s_reload_config, this, &MainWindow::OnConfigListChanged);
    w.exec();
}
void MainWindow::on_connectionListWidget_doubleClicked(const QModelIndex &index)
{
    //Q_UNUSED(index)
    //
    //if (!IsSelectionConnectable) return;
    //
    //ShowAndSetConnection(ItemConnectionIdentifier(connectionListWidget->currentItem()), true, false);
    //on_reconnectButton_clicked();
}
void MainWindow::on_clearlogButton_clicked()
{
    masterLogBrowser->clear();
}
void MainWindow::on_connectionListWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    Q_UNUSED(previous)
    CurrentItem = current;
    //isRenamingInProgress = false;
    //
    //if (!IsConnectableItem(current)) return;
    //
    //// no need to check !isRenamingInProgress since it's always true.
    //ShowAndSetConnection(ItemConnectionIdentifier(current), !vinstance->KernelStarted, false);
    ////on_connectionListWidget_itemClicked(current, 0);
}
void MainWindow::on_connectionListWidget_customContextMenuRequested(const QPoint &pos)
{
    //Q_UNUSED(pos)
    //auto _pos = QCursor::pos();
    //auto item = connectionListWidget->itemAt(connectionListWidget->mapFromGlobal(_pos));
    //
    //if (IsConnectableItem(item)) {
    //    connectionListMenu->popup(_pos);
    //}
}
void MainWindow::on_action_RCM_RenameConnection_triggered()
{
    //auto item = connectionListWidget->currentItem();
    //SUBSCRIPTION_CONFIG_MODIFY_DENY(item)
    //item->setFlags(item->flags() | Qt::ItemIsEditable);
    //isRenamingInProgress = true;
    //connectionListWidget->editItem(item);
    //renameOriginalIdentifier = ItemConnectionIdentifier(item);
}
void MainWindow::on_connectionListWidget_itemChanged(QTreeWidgetItem *item, int)
{
    //DEBUG(UI, "A connection ListViewItem is changed. This should ONLY occur when renaming an connection.")
    //
    //if (!isRenamingInProgress) {
    //    return;
    //}
    //
    //isRenamingInProgress = false;
    //// In this case it's after we entered the name.
    //// and tell user you should not rename a config from subscription.
    //auto newIdentifier = renameOriginalIdentifier;
    //newIdentifier.connectionName = item->text(0);
    //LOG(CONNECTION, "RENAME: " + renameOriginalIdentifier.IdentifierString() + " -> " + newIdentifier.IdentifierString())
    //
    //// If I really did some changes.
    //if (renameOriginalIdentifier != newIdentifier) {
    //    bool canContinueRename = true;
    //
    //    if (newIdentifier.connectionName.trimmed().isEmpty()) {
    //        QvMessageBoxWarn(this, tr("Rename a Connection"), tr("The name cannot be empty"));
    //        canContinueRename = false;
    //    }
    //
    //    QvMessageBoxInfo(this, "NOT SUPPORTED", "WIP");
    //    //if (GlobalConfig.configs.contains(newIdentifier.connectionName)) {
    //    //    QvMessageBoxWarn(this, tr("Rename a Connection"), tr("The name has been used already, Please choose another."));
    //    //    canContinueRename = false;
    //    //}
    //
    //    if (!IsValidFileName(newIdentifier.connectionName + QV2RAY_CONFIG_FILE_EXTENSION)) {
    //        QvMessageBoxWarn(this, tr("Rename a Connection"), tr("The name you suggested is not valid, please try another."));
    //        canContinueRename = false;
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
    //    //connections[newIdentifier] = connections.take(renameOriginalIdentifier);
    //    //RenameConnection(renameOriginalIdentifier.connectionName, newIdentifier.connectionName);
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
    //    //OnConfigListChanged(CurrentConnectionIdentifier.connectionName == renameOriginalName);
    //}
}
void MainWindow::on_removeConfigButton_clicked()
{
    QvMessageBoxInfo(this, "NOT SUPPORTED", "WIP");
    //QList<ConnectionIdentifier> connlist;
    //
    //for (auto item : connectionListWidget->selectedItems()) {
    //    if (IsConnectableItem(item)) {
    //        connlist.append(ItemConnectionIdentifier(item));
    //    }
    //}
    //
    //LOG(UI, "Selected " + QSTRN(connlist.count()) + " items")
    //
    //if (connlist.isEmpty()) {
    //    // Remove nothing means doing nothing.
    //    return;
    //}
    //
    //if (QvMessageBoxAsk(this, tr("Removing Connection(s)"), tr("Are you sure to remove selected connection(s)?")) != QMessageBox::Yes) {
    //    return;
    //}
    //
    //// A triple-state flag which indicates if the user wants to remove the configs loaded from a subscription.
    //int subscriptionRemovalCheckStatus = -1;
    //
    //for (auto conn : connlist) {
    //    if (conn == CurrentConnectionIdentifier) {
    //        on_stopButton_clicked();
    //        CurrentConnectionIdentifier = ConnectionIdentifier();
    //    }
    //
    //    auto connData = connections[conn];
    //
    //    // Remove auto start config.
    //    if (GlobalConfig.autoStartConfig.subscriptionName == connData.subscriptionName &&
    //        GlobalConfig.autoStartConfig.connectionName == connData.connectionName)
    //        // If all those settings match.
    //    {
    //        GlobalConfig.autoStartConfig.subscriptionName.clear();
    //        GlobalConfig.autoStartConfig.connectionName.clear();
    //    }
    //
    //    if (connData.configType == CONNECTION_REGULAR) {
    //        // Just remove the regular configs.
    //        if (!connData.subscriptionName.isEmpty()) {
    //            LOG(UI, "Unexpected subscription name in a single regular config.")
    //            connData.subscriptionName.clear();
    //        }
    //
    //        GlobalConfig.configs.removeOne(conn.connectionName);
    //
    //        if (!RemoveConnection(conn.connectionName)) {
    //            QvMessageBoxWarn(this, tr("Removing this Connection"), tr("Failed to delete connection file, please delete manually."));
    //        }
    //    } else if (connData.configType == CONNECTION_SUBSCRIPTION) {
    //        if (subscriptionRemovalCheckStatus == -1) {
    //            subscriptionRemovalCheckStatus = (QvMessageBoxAsk(this, tr("Removing a subscription config"), tr("Do you want to remove the config loaded from a subscription?")) == QMessageBox::Yes)
    //                                             ? 1 // Yes i want
    //                                             : 0; // No please keep
    //        }
    //
    //        if (subscriptionRemovalCheckStatus == 1) {
    //            if (!RemoveSubscriptionConnection(connData.subscriptionName, connData.connectionName)) {
    //                QvMessageBoxWarn(this, tr("Removing this Connection"), tr("Failed to delete connection file, please delete manually."));
    //            }
    //        }
    //    } else {
    //        LOG(SETTINGS, "Unknown config type -> Not regular nor subscription...")
    //    }
    //}
    //
    //LOG(UI, "Saving GlobalConfig")
    //SaveGlobalConfig(GlobalConfig);
    //OnConfigListChanged(false);
    //ShowAndSetConnection(CurrentConnectionIdentifier, false, false);
}

void MainWindow::on_importConfigButton_clicked()
{
    QvMessageBoxInfo(this, "NOT SUPPORTED", "WIP");
    //ImportConfigWindow w(this);
    //auto configs = w.OpenImport();
    //if (!configs.isEmpty()) {
    //    for (auto conf : configs) {
    //        auto name = configs.key(conf, "");
    //
    //        if (name.isEmpty())
    //            continue;
    //
    //        SaveConnectionConfig(conf, &name, false);
    //        GlobalConfig.configs.push_back(name);
    //    }
    //
    //    SaveGlobalConfig(GlobalConfig);
    //    OnConfigListChanged(false);
    //}
}
void MainWindow::on_editConfigButton_clicked()
{
    //// Check if we have a connection selected...
    //if (!IsSelectionConnectable) {
    //    QvMessageBoxWarn(this, tr("No Config Selected"), tr("Please Select a Config"));
    //    return;
    //}
    //
    //auto firstSelected = connectionListWidget->selectedItems().first();
    //auto _identifier = ItemConnectionIdentifier(firstSelected);
    //SUBSCRIPTION_CONFIG_MODIFY_ASK(firstSelected)
    ////
    //auto outBoundRoot = connections[_identifier].config;
    //CONFIGROOT root;
    //bool isChanged = false;
    //
    //if (IsComplexConfig(outBoundRoot)) {
    //    LOG(UI, "INFO: Opening route editor.")
    //    RouteEditor routeWindow(outBoundRoot, this);
    //    root = routeWindow.OpenEditor();
    //    isChanged = routeWindow.result() == QDialog::Accepted;
    //} else {
    //    LOG(UI, "INFO: Opening single connection edit window.")
    //    OutboundEditor w(OUTBOUND(outBoundRoot["outbounds"].toArray().first().toObject()), this);
    //    auto outboundEntry = w.OpenEditor();
    //    isChanged = w.result() == QDialog::Accepted;
    //    QJsonArray outboundsList;
    //    outboundsList.push_back(outboundEntry);
    //    root.insert("outbounds", outboundsList);
    //}
    //
    //QString alias = _identifier.connectionName;
    //
    //if (isChanged) {
    //    if (CheckConfigType(firstSelected, SUBSCRIPTION)) {
    //        auto name = connections[_identifier].connectionName;
    //        // Assume name will not change.
    //        SaveSubscriptionConfig(root, connections[_identifier].subscriptionName, &name);
    //    } else {
    //        connections[_identifier].config = root;
    //        // true indicates the alias will NOT change
    //        SaveConnectionConfig(root, &alias, true);
    //    }
    //
    //    OnConfigListChanged(alias == CurrentConnectionIdentifier.connectionName);
    //}
}
void MainWindow::on_reconnectButton_clicked()
{
    on_stopButton_clicked();
    on_startButton_clicked();
}

void MainWindow::on_action_RCM_ConvToComplex_triggered()
{
    //// Check if we have a connection selected...
    //if (!IsSelectionConnectable) {
    //    QvMessageBoxWarn(this, tr("No Config Selected"), tr("Please Select a Config"));
    //    return;
    //}
    //
    //auto selectedFirst = connectionListWidget->currentItem();
    //auto _identifier = ItemConnectionIdentifier(selectedFirst);
    //SUBSCRIPTION_CONFIG_MODIFY_DENY(selectedFirst)
    ////
    //auto outBoundRoot = connections[_identifier].config;
    //CONFIGROOT root;
    //bool isChanged = false;
    ////
    //LOG(UI, "INFO: Opening route editor.")
    //RouteEditor routeWindow(outBoundRoot, this);
    //root = routeWindow.OpenEditor();
    //isChanged = routeWindow.result() == QDialog::Accepted;
    //QString alias = _identifier.connectionName;
    //
    //if (isChanged) {
    //    connections[_identifier].config = root;
    //    // true indicates the alias will NOT change
    //    SaveConnectionConfig(root, &alias, true);
    //    OnConfigListChanged(_identifier == CurrentConnectionIdentifier);
    //    ShowAndSetConnection(CurrentConnectionIdentifier, false, false);
    //}
}

void MainWindow::on_action_RCM_EditJson_triggered()
{
    //// Check if we have a connection selected...
    //if (!IsSelectionConnectable) {
    //    QvMessageBoxWarn(this, tr("No Config Selected"), tr("Please Select a Config"));
    //    return;
    //}
    //
    //auto selectedFirst = connectionListWidget->currentItem();
    //auto _identifier = ItemConnectionIdentifier(selectedFirst);
    //SUBSCRIPTION_CONFIG_MODIFY_DENY(selectedFirst)
    //JsonEditor w(connections[_identifier].config, this);
    //auto root = CONFIGROOT(w.OpenEditor());
    //bool isChanged = w.result() == QDialog::Accepted;
    //QString alias = _identifier.connectionName;
    //
    //if (isChanged) {
    //    connections[_identifier].config = root;
    //    // Alias here will not change.
    //    SaveConnectionConfig(root, &alias, true);
    //    ShowAndSetConnection(CurrentConnectionIdentifier, false, false);
    //}
}
void MainWindow::on_editJsonBtn_clicked()
{
    // See above.
    on_action_RCM_EditJson_triggered();
}
void MainWindow::on_pingTestBtn_clicked()
{
    //// Get data from UI
    //QList<QvConnectionObject> aliases;
    //auto selection = connectionListWidget->selectedItems();
    //
    //if (selection.count() == 0) {
    //    // Ping ALL connections, warning.
    //    if (QvMessageBoxAsk(this, tr("Latency Test"), tr("You are about to run latency test on all servers, do you want to continue?")) == QMessageBox::Yes) {
    //        aliases.append(connections.keys());
    //    }
    //} else {
    //    for (auto i = 0; i < selection.count(); i++) {
    //        auto thisItem = selection[i];
    //
    //        if (thisItem->childCount() > 0) {
    //            // So we add another check to make sure the selected one is a subscription entry.
    //            // Loop to add all sub-connections to the list.
    //            for (auto j = 0; j < thisItem->childCount(); j++) {
    //                aliases.append(ItemConnectionIdentifier(thisItem->child(j)));
    //            }
    //        } else {
    //            aliases.append(ItemConnectionIdentifier(thisItem));
    //        }
    //    }
    //}
    //
    //LOG(UI, "Will perform latency test on " + QSTRN(aliases.count()) + " hosts.")
    //latencyLabel->setText(tr("Testing..."));
    //
    //for (auto alias : aliases) {
    //    MWTryPingConnection(alias);
    //}
}
void MainWindow::on_shareBtn_clicked()
{
    //// Share QR
    //if (!IsSelectionConnectable) {
    //    return;
    //}
    //
    //auto _identifier = ItemConnectionIdentifier(connectionListWidget->currentItem());
    //auto root = connections[_identifier].config;
    //auto type = get<2>(GetConnectionInfo(root));
    //
    //if (!IsComplexConfig(root) && (type == "vmess" || type == "shadowsocks")) {
    //    ConfigExporter v(root, _identifier, this);
    //    v.OpenExport();
    //} else {
    //    QvMessageBoxWarn(this, tr("Share Connection"), tr("There're no support of sharing configs other than vmess and shadowsocks"));
    //}
}
void MainWindow::on_action_RCM_ShareQR_triggered()
{
    on_shareBtn_clicked();
}
void MainWindow::timerEvent(QTimerEvent *event)
{
    //// Calling base class
    //QMainWindow::timerEvent(event);
    //
    //if (event->timerId() == speedTimerId) {
    //    auto _totalSpeedUp = vinstance->getAllSpeedUp();
    //    auto _totalSpeedDown = vinstance->getAllSpeedDown();
    //    auto _totalDataUp = vinstance->getAllDataUp();
    //    auto _totalDataDown = vinstance->getAllDataDown();
    //    //
    //    speedChartView->AddPointData(_totalSpeedUp, _totalSpeedDown);
    //    //
    //    auto totalSpeedUp = FormatBytes(_totalSpeedUp) + "/s";
    //    auto totalSpeedDown = FormatBytes(_totalSpeedDown) + "/s";
    //    auto totalDataUp = FormatBytes(_totalDataUp);
    //    auto totalDataDown = FormatBytes(_totalDataDown);
    //    //
    //    netspeedLabel->setText(totalSpeedUp + NEWLINE + totalSpeedDown);
    //    dataamountLabel->setText(totalDataUp + NEWLINE + totalDataDown);
    //    //
    //    hTray.setToolTip(TRAY_TOOLTIP_PREFIX NEWLINE + tr("Connected: ") + CurrentConnectionIdentifier.IdentifierString() + NEWLINE "Up: " + totalSpeedUp + " Down: " + totalSpeedDown);
    //} else if (event->timerId() == qvLogTimerId) {
    //    QString lastLog = readLastLog();
    //
    //    if (!lastLog.isEmpty()) {
    //        qvAppLogBrowser->append(lastLog);
    //    }
    //
    //    CleanUpLogs(vCoreLogBrowser)
    //} else if (event->timerId() == pingTimerId) {
    //    MWTryPingConnection(CurrentConnectionIdentifier);
    //}
}
void MainWindow::on_duplicateBtn_clicked()
{
    QvMessageBoxInfo(this, "NOT SUPPORTED", "WIP");
    //if (!IsSelectionConnectable) {
    //    return;
    //}
    //
    //auto selectedFirst = connectionListWidget->currentItem();
    //auto _identifier = ItemConnectionIdentifier(selectedFirst);
    //SUBSCRIPTION_CONFIG_MODIFY_ASK(selectedFirst)
    //CONFIGROOT conf;
    //// Alias may change.
    //QString alias = _identifier.connectionName;
    //bool isComplex = IsComplexConfig(connections[_identifier].config);
    //
    //if (connections[_identifier].configType == CONNECTION_REGULAR) {
    //    conf = ConvertConfigFromFile(QV2RAY_CONFIG_DIR + _identifier.connectionName + QV2RAY_CONFIG_FILE_EXTENSION, isComplex);
    //} else {
    //    conf = ConvertConfigFromFile(QV2RAY_SUBSCRIPTION_DIR + _identifier.subscriptionName + "/" + _identifier.connectionName  + QV2RAY_CONFIG_FILE_EXTENSION, isComplex);
    //    alias = _identifier.subscriptionName + "_" + _identifier.connectionName;
    //}
    //
    //SaveConnectionConfig(conf, &alias, false);
    //GlobalConfig.configs.push_back(alias);
    //SaveGlobalConfig(GlobalConfig);
    //this->OnConfigListChanged(false);
}

void MainWindow::on_subsButton_clicked()
{
    SubscribeEditor w;
    w.exec();
    OnConfigListChanged(false);
}

void MainWindow::on_connectionListWidget_itemSelectionChanged()
{
    //if (!isRenamingInProgress && !IsSelectionConnectable) {
    //    // If renaming is not in progress AND our selection is invalid.
    //    CurrentSelectedItemPtr = nullptr;
    //    SetEditWidgetEnable(false);
    //    routeCountLabel->setText(tr("N/A"));
    //    _OutBoundTypeLabel->setText(tr("N/A"));
    //    _hostLabel->setText(tr("N/A"));
    //    _portLabel->setText(tr("N/A"));
    //    latencyLabel->setText(tr("N/A"));
    //} else {
    //    if (!connectionListWidget->selectedItems().isEmpty()) {
    //        on_connectionListWidget_currentItemChanged(connectionListWidget->selectedItems().first(), nullptr);
    //        CurrentSelectedItemPtr = connectionListWidget->selectedItems().first();
    //    }
    //}
}

void MainWindow::on_connectionListWidget_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column)
    auto widget = GetItemWidget(item);

    if (widget->IsConnection()) {
        widget->BeginConnection();
    }
}
/*
 *        //if (startFlag) {
        //    MWTryPingConnection(name);
        //    speedTimerId = startTimer(1000);
        //    pingTimerId = startTimer(60000);
        //    this->hTray.showMessage("Qv2ray", tr("Connected: ") + name, this->windowIcon());
        //    hTray.setToolTip(TRAY_TOOLTIP_PREFIX NEWLINE + tr("Connected: ") + name);
        //    statusLabel->setText(tr("Connected: ") + name);
        //}
        //} else {
        //    this->hTray.showMessage("Qv2ray", tr("Already connected to: ") + CurrentConnectionIdentifier.IdentifierString(), this->windowIcon());
        //}
 */

void MainWindow::OnConnected(const ConnectionId &id)
{
    Q_UNUSED(id)
}


void MainWindow::onConnectionWidgetFocusRequested(const ConnectionWidget *_widget)
{
    if (_widget == nullptr) {
        return;
    }

    for (auto _item_ : connectionListWidget->findItems(QString("*"), Qt::MatchWrap | Qt::MatchWildcard | Qt::MatchRecursive)) {
        auto myWidget = GetItemWidget(_item_);

        if (myWidget == _widget) {
            LOG(MODULE_UI, "Setting current item.")
            connectionListWidget->setCurrentItem(_item_);
        }
    }
}

void MainWindow::on_connectionFilterTxt_textEdited(const QString &arg1)
{
    // No recursive since we only need top level item
    for (auto _top_item_ : connectionListWidget->findItems(QString("*"), Qt::MatchWrap | Qt::MatchWildcard)) {
        //auto topWidget = GetItemWidget(_top_item_);
        bool isTotallyHide = true;

        for (auto i = 0; i < _top_item_->childCount(); i++) {
            auto _child_ = _top_item_->child(i);
            auto childWidget = GetItemWidget(_child_);

            if (childWidget->NameMatched(arg1)) {
                LOG(MODULE_UI, "Setting current item.")
                // Show the child
                _child_->setHidden(false);
                isTotallyHide = false;
            } else {
                _child_->setHidden(true);
            }
        }

        _top_item_->setHidden(isTotallyHide);

        if (!isTotallyHide) {
            connectionListWidget->expandItem(_top_item_);
        }
    }
}

void MainWindow::on_connectionListWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column)
    infoWidget->ShowConnectionDetails(GetItemWidget(item)->Identifier());
}
