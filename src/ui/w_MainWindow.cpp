#include <QAction>
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

#include "w_MainWindow.hpp"
#include "w_OutboundEditor.hpp"
#include "w_ImportConfig.hpp"
#include "w_RoutesEditor.hpp"
#include "w_PreferencesWindow.hpp"
#include "w_SubscriptionEditor.hpp"
#include "w_JsonEditor.hpp"
#include "w_ExportConfig.hpp"

#include "QvNetSpeedPlugin.hpp"
#include "QvPACHandler.hpp"

// MainWindow.cpp --> Main MainWindow source file, handles mostly UI-related operations.

#define TRAY_TOOLTIP_PREFIX "Qv2ray " QV2RAY_VERSION_STRING
//
#define vCoreLogBrowser this->logTextBrowsers[0]
#define qvAppLogBrowser this->logTextBrowsers[1]
#define currentLogBrowser this->logTextBrowsers[currentLogBrowserId]
//
#define IsRegularConfig(var) (connections.contains(var) && connections[var].configType == CON_REGULAR)
#define IsSubscription(var) (connections.contains(var) && connections[var].configType == CON_SUBSCRIPTION)
//
#define SUBSCRIPTION_CONFIG_MODIFY_ASK(varName)                                                                                                                 \
    if (!IsRegularConfig(varName)) {                                                                                                                            \
        if (QvMessageBoxAsk(this, QObject::tr("Editing a subscription config"), QObject::tr("You are trying to edit a config loaded from subscription.") +      \
                            NEWLINE + QObject::tr("All changes will be overwritten when the subscriptions are updated next time.") +                            \
                            NEWLINE + QObject::tr("Are you still going to do so?")) != QMessageBox::Yes) {                                                      \
            return;                                                                                                                                             \
        }                                                                                                                                                       \
    }                                                                                                                                                           \


#define SUBSCRIPTION_CONFIG_MODIFY_DENY(varName)                                                                                                                \
    if (!IsRegularConfig(varName)) {                                                                                                                            \
        QvMessageBox(this, QObject::tr("Editing a subscription config"), QObject::tr("You should not modity this property of a config from a subscription"));   \
        return;                                                                                                                                                 \
    }                                                                                                                                                           \

#define IsConnectableItem(item) (item != nullptr && item->childCount() == 0 && (IsRegularConfig(item->text(0)) || IsSubscription(item->text(0))))
#define IsSelectionConnectable (!connectionListWidget->selectedItems().empty() && IsConnectableItem(connectionListWidget->selectedItems().first()))

MainWindow *MainWindow::mwInstance = nullptr;

MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent), vinstance(), uploadList(), downloadList(),
    hTray(new QSystemTrayIcon(this)), vCoreLogHighlighter(), qvAppLogHighlighter()
{
    MainWindow::mwInstance = this;
    currentConfig = GetGlobalConfig();
    vinstance = new ConnectionInstance();
    connect(vinstance, &ConnectionInstance::onProcessOutputReadyRead, this, &MainWindow::UpdateVCoreLog);
    setupUi(this);
    //
    // Two browsers
    logTextBrowsers.append(new QTextBrowser());
    logTextBrowsers.append(new QTextBrowser());
    vCoreLogBrowser->setFontPointSize(8);
    vCoreLogBrowser->setReadOnly(true);
    vCoreLogBrowser->setLineWrapMode(QTextBrowser::LineWrapMode::NoWrap);
    qvAppLogBrowser->setFontPointSize(8);
    qvAppLogBrowser->setReadOnly(true);
    qvAppLogBrowser->setLineWrapMode(QTextBrowser::LineWrapMode::NoWrap);
    //
    vCoreLogHighlighter = new SyntaxHighlighter(currentConfig.uiConfig.useDarkTheme, vCoreLogBrowser->document());
    qvAppLogHighlighter = new SyntaxHighlighter(currentConfig.uiConfig.useDarkTheme, qvAppLogBrowser->document());
    currentLogBrowserId = 0;
    masterLogBrowser->setDocument(currentLogBrowser->document());
    masterLogBrowser->document()->setDocumentMargin(8);
    masterLogBrowser->document()->adjustSize();
    masterLogBrowser->setLineWrapMode(QTextBrowser::LineWrapMode::NoWrap);
    //
    logTimerId = startTimer(500);
    //
    pacServer = new PACServer();
    tcpingModel = new QvTCPingModel(3, this);
    requestHelper = new QvHttpRequestHelper();
    connect(tcpingModel, &QvTCPingModel::PingFinished, this, &MainWindow::onPingFinished);
    //
    this->setWindowIcon(QIcon(":/icons/qv2ray.png"));
    hTray->setIcon(QIcon(currentConfig.uiConfig.useDarkTrayIcon ? ":/icons/ui_dark/tray.png" : ":/icons/ui_light/tray.png"));
    importConfigButton->setIcon(QICON_R("import.png"));
    duplicateBtn->setIcon(QICON_R("duplicate.png"));
    removeConfigButton->setIcon(QICON_R("delete.png"));
    editConfigButton->setIcon(QICON_R("edit.png"));
    editJsonBtn->setIcon(QICON_R("json.png"));
    //
    pingTestBtn->setIcon(QICON_R("ping_gauge.png"));
    shareBtn->setIcon(QICON_R("share.png"));
    updownImageBox->setStyleSheet("image: url(" + QV2RAY_UI_RESOURCES_ROOT + "netspeed_arrow.png)");
    updownImageBox_2->setStyleSheet("image: url(" + QV2RAY_UI_RESOURCES_ROOT + "netspeed_arrow.png)");
    //
    // Setup System tray icons and menus
    //
    hTray->setToolTip(TRAY_TOOLTIP_PREFIX);
    // Basic actions
    action_Tray_ShowHide = new QAction(this->windowIcon(), tr("Hide"), this);
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
    //
    tray_RootMenu->addAction(action_Tray_ShowHide);
    tray_RootMenu->addSeparator();
    tray_RootMenu->addMenu(tray_SystemProxyMenu);
    tray_RootMenu->addSeparator();
    tray_RootMenu->addAction(action_Tray_Start);
    tray_RootMenu->addAction(action_Tray_Stop);
    tray_RootMenu->addAction(action_Tray_Reconnect);
    tray_RootMenu->addSeparator();
    tray_RootMenu->addAction(action_Tray_Quit);
    //
    connect(action_Tray_ShowHide, &QAction::triggered, this, &MainWindow::ToggleVisibility);
    connect(action_Tray_Start, &QAction::triggered, this, &MainWindow::on_startButton_clicked);
    connect(action_Tray_Stop, &QAction::triggered, this, &MainWindow::on_stopButton_clicked);
    connect(action_Tray_Reconnect, &QAction::triggered, this, &MainWindow::on_reconnectButton_clicked);
    connect(action_Tray_Quit, &QAction::triggered, this, &MainWindow::quit);
    connect(action_Tray_SetSystemProxy, &QAction::triggered, [this]() {
        this->MWSetSystemProxy();
    });
    connect(action_Tray_ClearSystemProxy, &QAction::triggered, [this]() {
        this->MWClearSystemProxy(true);
    });
    connect(hTray, &QSystemTrayIcon::activated, this, &MainWindow::on_activatedTray);
    //
    // Actions for right click the connection list
    //
    QAction *action_RCM_RenameConnection = new QAction(tr("Rename"), this);
    QAction *action_RCM_StartThis = new QAction(tr("Connect to this"), this);
    QAction *action_RCM_ConvToComplex = new QAction(tr("Edit as Complex Config"), this);
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
    connect(this, &MainWindow::Connect, this, &MainWindow::on_startButton_clicked);
    connect(this, &MainWindow::DisConnect, this, &MainWindow::on_stopButton_clicked);
    connect(this, &MainWindow::ReConnect, this, &MainWindow::on_reconnectButton_clicked);
    //
    hTray->setContextMenu(tray_RootMenu);
    hTray->show();
    //
    connectionListMenu = new QMenu(this);
    connectionListMenu->addAction(action_RCM_RenameConnection);
    connectionListMenu->addAction(action_RCM_StartThis);
    connectionListMenu->addAction(action_RCM_ConvToComplex);
    connectionListMenu->addAction(action_RCM_EditJson);
    connectionListMenu->addAction(action_RCM_ShareQR);
    //
    OnConfigListChanged(false);
    //
    // For charts
    uploadSerie = new QSplineSeries(this);
    downloadSerie = new QSplineSeries(this);
    uploadSerie->setName(tr("Upload"));
    downloadSerie->setName(tr("Download"));

    for (int i = 0; i < 30 ; i++) {
        uploadList.append(0);
        downloadList.append(0);
        uploadSerie->append(i, 0);
        downloadSerie->append(i, 0);
    }

    speedChartObj = new QChart();
    speedChartObj->setTheme(currentConfig.uiConfig.useDarkTheme ? QChart::ChartThemeDark : QChart::ChartThemeLight);
    speedChartObj->setTitle(""); // Fake hidden
    speedChartObj->legend()->hide();
    speedChartObj->createDefaultAxes();
    speedChartObj->addSeries(uploadSerie);
    speedChartObj->addSeries(downloadSerie);
    speedChartObj->createDefaultAxes();
    speedChartObj->axes(Qt::Vertical).first()->setRange(0, 512);
    //
    static_cast<QValueAxis>(speedChartObj->axes(Qt::Horizontal).first()).setLabelFormat("dd");
    speedChartObj->axes(Qt::Horizontal).first()->setRange(0, 30);
    speedChartObj->setContentsMargins(-20, -50, -20, -25);
    speedChartView = new QChartView(speedChartObj, this);
    speedChartView->setRenderHint(QPainter::RenderHint::HighQualityAntialiasing, true);
    //
    auto layout = new QHBoxLayout(speedChart);
    layout->addWidget(speedChartView);
    speedChart->setLayout(layout);
    //
    // Find and start if there is an auto-connection
    MWFindAndStartAutoConfig();

    // If we are not connected to anything, show the MainWindow.
    if (vinstance->ConnectionStatus != STARTED) {
        this->show();
    }

    connect(requestHelper, &QvHttpRequestHelper::httpRequestFinished, this, &MainWindow::VersionUpdate);
    requestHelper->get("https://api.github.com/repos/lhy0403/Qv2ray/releases/latest");
    StartProcessingPlugins();
}

void MainWindow::SetEditWidgetEnable(bool enabled)
{
    removeConfigButton->setEnabled(enabled);
    editConfigButton->setEnabled(enabled);
    duplicateBtn->setEnabled(enabled);
    editJsonBtn->setEnabled(enabled);
    shareBtn->setEnabled(enabled);
    // Allow ping all.
    pingTestBtn->setText(enabled ? tr("Ping") : tr("Ping All"));
}

void MainWindow::mouseReleaseEvent(QMouseEvent *e)
{
    Q_UNUSED(e)

    // If mouse is on the logLabel, switch log source.
    if (logLabel->underMouse()) {
        //auto layout = masterLogBrowser->document()->setDocumentLayout()
        currentLogBrowserId = (currentLogBrowserId + 1) % logTextBrowsers.count();
        masterLogBrowser->setDocument(currentLogBrowser->document());
        masterLogBrowser->document()->setDocumentMargin(4);
        masterLogBrowser->document()->adjustSize();
        masterLogBrowser->setLineWrapMode(QTextBrowser::LineWrapMode::NoWrap);
        auto bar = masterLogBrowser->verticalScrollBar();
        bar->setValue(bar->maximum());
    }
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return) {
        // If pressed enter or return on connectionListWidget.
        // Try to connect to the selected connection.
        if (focusWidget() == connectionListWidget) {
            if (!IsSelectionConnectable) return;

            auto selections = connectionListWidget->selectedItems();
            auto connectionName = selections.first()->text(0);
            ShowAndSetConnection(connectionName, true, true);
        }
    }
}

void MainWindow::on_action_StartThis_triggered()
{
    if (!IsSelectionConnectable) {
        QvMessageBox(this, tr("No connection selected!"), tr("Please select a config from the list."));
        return;
    }

    CurrentConnectionName = connectionListWidget->selectedItems().first()->text(0);
    on_reconnectButton_clicked();
}
void MainWindow::VersionUpdate(QByteArray &data)
{
    // Version update handler.
    QJsonObject root = JsonFromString(QString(data));
    //
    QVersionNumber newversion = QVersionNumber::fromString(root["tag_name"].toString("v").remove(0, 1));
    QVersionNumber current = QVersionNumber::fromString(QSTRING(QV2RAY_VERSION_STRING).remove(0, 1));
    QVersionNumber ignored = QVersionNumber::fromString(QSTRING(currentConfig.ignoredVersion));
    LOG(MODULE_UPDATE, "Received update info, Latest: " + newversion.toString().toStdString() + " Current: " + current.toString().toStdString() + " Ignored: " + ignored.toString().toStdString())

    // If the version is newer than us.
    // And new version is newer than the ignored version.
    if (newversion > current && newversion > ignored) {
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
            currentConfig.ignoredVersion = newversion.toString().toStdString();
            SetGlobalConfig(currentConfig);
        }
    }
}
void MainWindow::OnConfigListChanged(bool need_restart)
{
    bool isRunning = vinstance->ConnectionStatus == STARTED;

    if (isRunning && need_restart) on_stopButton_clicked();

    LOG(MODULE_UI, "Loading new GlobalConfig")
    SetEditWidgetEnable(false);
    currentConfig = GetGlobalConfig();
    //
    // Store the latency test value.
    QMap<QString, double> latencyValueCache;

    for (auto i = 0; i < connections.count(); i++) {
        latencyValueCache[connections.keys()[i]] = connections.values()[i].latency;
    }

    connections.clear();
    connectionListWidget->clear();
    auto _regularConnections = GetRegularConnections(currentConfig.configs);
    auto _subsConnections = GetSubscriptionConnections(toStdList(QMap<string, string>(currentConfig.subscribes).keys()));

    for (auto i = 0; i < _regularConnections.count(); i++) {
        ConnectionObject _o;
        _o.configType = CON_REGULAR;
        _o.subscriptionName = "";
        _o.connectionName = _regularConnections.keys()[i];
        _o.config = _regularConnections.values()[i];
        _o.latency = latencyValueCache.contains(_o.connectionName) ? latencyValueCache[_o.connectionName] : 0; // restore latency values
        connections[_o.connectionName] = _o;
        connectionListWidget->addTopLevelItem(new QTreeWidgetItem(QStringList() << _o.connectionName));
    }

    for (auto i = 0; i < _subsConnections.count(); i++) {
        auto subName = _subsConnections.keys()[i];
        auto subTopLevel = new QTreeWidgetItem(QStringList() << tr("Subscription:") + " " + subName);
        connectionListWidget->addTopLevelItem(subTopLevel);

        for (auto j = 0; j < _subsConnections.values()[i].count(); j++) {
            ConnectionObject _o;
            _o.configType = CON_SUBSCRIPTION;
            _o.subscriptionName = subName;
            _o.connectionName = _subsConnections.values()[i].keys()[j];
            _o.config = _subsConnections.values()[i].values()[j];
            // connection name generated from subscription name and connection name.
            auto connName = _o.connectionName + " (" + tr("Subscription:") + " " + _o.subscriptionName + ")";
            _o.latency = latencyValueCache.contains(connName) ? latencyValueCache[connName] : 0;
            connections[connName] = _o;
            subTopLevel->addChild(new QTreeWidgetItem(QStringList() << connName));
        }
    }

    // We set the current selected item back...
    if (!CurrentConnectionName.isEmpty() && connections.contains(CurrentConnectionName)) {
        auto items = connectionListWidget->findItems(CurrentConnectionName, Qt::MatchExactly | Qt::MatchRecursive);

        if (items.count() > 0 && IsConnectableItem(items.first())) {
            connectionListWidget->setCurrentItem(items.first());
            connectionListWidget->scrollToItem(items.first());
        }

        ShowAndSetConnection(CurrentConnectionName, false, false);
    }

    connectionListWidget->sortItems(0, Qt::AscendingOrder);

    if (isRunning && need_restart) on_startButton_clicked();
}
MainWindow::~MainWindow()
{
    killTimer(logTimerId);
    hTray->hide();
    delete this->hTray;
    delete this->vinstance;
}
void MainWindow::UpdateVCoreLog(const QString &log)
{
    vCoreLogBrowser->append(log);
    setMasterLogHBar();
}
void MainWindow::setMasterLogHBar()
{
    auto bar = masterLogBrowser->verticalScrollBar();
    auto max = bar->maximum();
    auto val = bar->value();

    if (val >= max * 0.8 || val >= max - 20)
        bar->setValue(max);
}
void MainWindow::on_startButton_clicked()
{
    if (vinstance->ConnectionStatus != STARTED) {
        // Reset the graph
        for (int i = 0; i < 30 ; i++) {
            uploadList[i] = 0;
            downloadList[i] = 0;
            uploadSerie->replace(i, 0, 0);
            downloadSerie->replace(i, 0, 0);
        }

        // Check Selection
        if (CurrentConnectionName.isEmpty()) {
            QvMessageBox(this, tr("No connection selected!"), tr("Please select a config from the list."));
            return;
        }

        LOG(MODULE_VCORE, ("Connecting to: " + CurrentConnectionName).toStdString())
        vCoreLogBrowser->clear();
        bool startFlag = MWtryStartConnection();

        if (startFlag) {
            MWTryPingConnection(CurrentConnectionName);
            speedTimerId = startTimer(1000);
            pingTimerId = startTimer(60000);
            this->hTray->showMessage("Qv2ray", tr("Connected: ") + CurrentConnectionName, this->windowIcon());
            hTray->setToolTip(TRAY_TOOLTIP_PREFIX NEWLINE + tr("Connected: ") + CurrentConnectionName);
            statusLabel->setText(tr("Connected: ") + CurrentConnectionName);
        } else {
            // If failed, show mainwindow
            this->show();
        }

        // Menu actions
        action_Tray_Start->setEnabled(!startFlag);
        action_Tray_Stop->setEnabled(startFlag);
        action_Tray_Reconnect->setEnabled(startFlag);
        tray_SystemProxyMenu->setEnabled(startFlag);
        // Buttons
        startButton->setEnabled(!startFlag);
        stopButton->setEnabled(startFlag);
    } else {
        LOG(MODULE_UI, "vCore already started.")
    }
}

void MainWindow::on_stopButton_clicked()
{
    if (vinstance->ConnectionStatus != STOPPED) {
        // Is running or starting
        killTimer(speedTimerId);
        killTimer(pingTimerId);
        //
        MWStopConnection();
        //
        hTray->setToolTip(TRAY_TOOLTIP_PREFIX);
        vCoreLogBrowser->clear();
        statusLabel->setText(tr("Disconnected"));
        action_Tray_Start->setEnabled(true);
        action_Tray_Stop->setEnabled(false);
        action_Tray_Reconnect->setEnabled(false);
        // Set to false as the system proxy has been cleared in the StopConnection function.
        tray_SystemProxyMenu->setEnabled(false);
        startButton->setEnabled(true);
        stopButton->setEnabled(false);
        //
        netspeedLabel->setText("0.00 B/s\r\n0.00 B/s");
        dataamountLabel->setText("0.00 B\r\n0.00 B");
        LOG(MODULE_UI, "Stopped successfully.")
    }
}
void MainWindow::closeEvent(QCloseEvent *event)
{
    this->hide();
    tray_RootMenu->actions()[0]->setText(tr("Show"));
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

        case QSystemTrayIcon::MiddleClick:
            if (this->vinstance->ConnectionStatus == STARTED) {
                on_stopButton_clicked();
            } else {
                on_startButton_clicked();
            }

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
void MainWindow::quit()
{
    StopProcessingPlugins();
    tcpingModel->StopAllPing();
    on_stopButton_clicked();
    ExitQv2ray();
}
void MainWindow::on_actionExit_triggered()
{
    quit();
}
void MainWindow::ShowAndSetConnection(QString guiConnectionName, bool SetConnection, bool ApplyConnection)
{
    // Check empty again...
    if (guiConnectionName.isEmpty()) return;

    SetEditWidgetEnable(true);
    //
    // --------- BRGIN Show Connection
    currentSelectedName = guiConnectionName;
    auto conf = connections[guiConnectionName];
    //
    auto isComplexConfig = CheckIsComplexConfig(conf.config);
    routeCountLabel->setText(isComplexConfig ? tr("Complex") : tr("Simple"));

    if (conf.latency == 0.0) {
        latencyLabel->setText(tr("No data"));
    } else {
        latencyLabel->setText(QString::number(conf.latency) + " " + tr("ms"));
    }

    if (conf.configType == CON_SUBSCRIPTION) {
        routeCountLabel->setText(routeCountLabel->text().append(" (" + tr("From subscription") + ":" + conf.subscriptionName + ")"));
    }

    // Get Connection info
    auto host_port = MWGetConnectionPortNumber(currentSelectedName);
    _hostLabel->setText(get<0>(host_port));
    _portLabel->setText(QString::number(get<1>(host_port)));
    _OutBoundTypeLabel->setText(get<2>(host_port));

    // Set to currentConnection
    if (SetConnection) {
        CurrentConnectionName = guiConnectionName;
    }

    // Restart Connection
    if (ApplyConnection) {
        on_reconnectButton_clicked();
    }
}
void MainWindow::on_preferencesBtn_clicked()
{
    PreferencesWindow *w = new PreferencesWindow(this);
    connect(w, &PreferencesWindow::s_reload_config, this, &MainWindow::OnConfigListChanged);
    w->show();
}
void MainWindow::on_connectionListWidget_doubleClicked(const QModelIndex &index)
{
    Q_UNUSED(index)

    if (!IsSelectionConnectable) return;

    QString currentText = connectionListWidget->currentItem()->text(0);
    ShowAndSetConnection(currentText, true, false);
    on_reconnectButton_clicked();
}
void MainWindow::on_clearlogButton_clicked()
{
    vCoreLogBrowser->clear();
}
void MainWindow::on_connectionListWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    Q_UNUSED(previous)
    isRenamingInProgress = false;

    if (!IsConnectableItem(current)) return;

    QString currentText = current->text(0);
    bool canSetConnection = !isRenamingInProgress && vinstance->ConnectionStatus != STARTED;
    ShowAndSetConnection(currentText, canSetConnection, false);
    //on_connectionListWidget_itemClicked(current, 0);
}
void MainWindow::on_connectionListWidget_customContextMenuRequested(const QPoint &pos)
{
    Q_UNUSED(pos)
    auto _pos = QCursor::pos();
    auto item = connectionListWidget->itemAt(connectionListWidget->mapFromGlobal(_pos));

    if (IsConnectableItem(item)) {
        connectionListMenu->popup(_pos);
    }
}
void MainWindow::on_action_RCM_RenameConnection_triggered()
{
    auto item = connectionListWidget->currentItem();
    SUBSCRIPTION_CONFIG_MODIFY_DENY(item->text(0))
    item->setFlags(item->flags() | Qt::ItemIsEditable);
    connectionListWidget->editItem(item);
    renameOriginalName = item->text(0);
    isRenamingInProgress = true;
}
void MainWindow::on_connectionListWidget_itemChanged(QTreeWidgetItem *item, int)
{
    DEBUG(MODULE_UI, "A connection ListViewItem is changed.")

    if (!isRenamingInProgress) return;

    isRenamingInProgress = false;
    // In this case it's after we entered the name.
    // and tell user you should not rename a config from subscription.
    auto newName = item->text(0);
    LOG(MODULE_CONNECTION, "RENAME: " + renameOriginalName.toStdString() + " -> " + newName.toStdString())

    // If I really did some changes.
    if (renameOriginalName != newName) {
        bool canContinueRename = true;

        if (newName.trimmed().isEmpty()) {
            QvMessageBox(this, tr("Rename a Connection"), tr("The name cannot be empty"));
            canContinueRename = false;
        }

        if (contains(currentConfig.configs, newName.toStdString())) {
            QvMessageBox(this, tr("Rename a Connection"), tr("The name has been used already, Please choose another."));
            canContinueRename = false;
        }

        if (!IsValidFileName(newName + QV2RAY_CONFIG_FILE_EXTENSION)) {
            QvMessageBox(this, tr("Rename a Connection"), tr("The name you suggested is not valid, please try another."));
            canContinueRename = false;
        }

        if (!canContinueRename) {
            // Set the item text back
            assert(item != nullptr); // Let's say the item should not be null
            item->setText(0, renameOriginalName);
            return;
        }

        // Change auto start config.
        //  |--------------=== In case it's not in a subscription --|
        if (currentConfig.autoStartConfig.subscriptionName.empty() && renameOriginalName.toStdString() == currentConfig.autoStartConfig.connectionName) {
            currentConfig.autoStartConfig.connectionName = newName.toStdString();
        }

        // Replace the items in the current loaded config list and settings.
        currentConfig.configs.remove(renameOriginalName.toStdString());
        currentConfig.configs.push_back(newName.toStdString());
        connections[newName] = connections.take(renameOriginalName);
        RenameConnection(renameOriginalName, newName);
        LOG(MODULE_UI, "Saving a global config")
        SetGlobalConfig(currentConfig);

        if (CurrentConnectionName == renameOriginalName)
            CurrentConnectionName = newName;

        // Trigger change of the connection list will summon a SegFault
        //OnConfigListChanged(running);
    }
}
void MainWindow::on_removeConfigButton_clicked()
{
    QStringList connlist;

    for (auto item : connectionListWidget->selectedItems()) {
        if (IsConnectableItem(item)) {
            connlist.append(item->text(0));
        }
    }

    LOG(MODULE_UI, "Selected " + to_string(connlist.count()) + " items")

    if (connlist.isEmpty()) {
        // Remove nothing means doing nothing.
        return;
    }

    if (QvMessageBoxAsk(this, tr("Removing Connection(s)"), tr("Are you sure to remove selected connection(s)?")) != QMessageBox::Yes) {
        return;
    }

    // A triple-state flag which indicates if the user wants to remove the configs loaded from a subscription.
    int subscriptionRemovalCheckStatus = -1;

    for (auto name : connlist) {
        if (name == CurrentConnectionName) {
            on_stopButton_clicked();
            CurrentConnectionName.clear();
        }

        auto connData = connections[name];

        // Remove auto start config.
        if (currentConfig.autoStartConfig.subscriptionName == connData.subscriptionName.toStdString() &&
            currentConfig.autoStartConfig.connectionName == connData.connectionName.toStdString())
            // If all those settings match.
        {
            currentConfig.autoStartConfig.subscriptionName.clear();
            currentConfig.autoStartConfig.connectionName.clear();
        }

        if (IsRegularConfig(name)) {
            // Just remove the regular configs.
            if (!connData.subscriptionName.isEmpty()) {
                LOG(MODULE_UI, "Unexpected subscription name in a single regular config.")
                connData.subscriptionName.clear();
            }

            currentConfig.configs.remove(name.toStdString());

            if (!RemoveConnection(name)) {
                QvMessageBox(this, tr("Removing this Connection"), tr("Failed to delete connection file, please delete manually."));
            }
        } else if (IsSubscription(name)) {
            if (subscriptionRemovalCheckStatus == -1) {
                subscriptionRemovalCheckStatus = (QvMessageBoxAsk(this, tr("Removing a subscription config"), tr("Do you want to remove the config loaded from a subscription?")) == QMessageBox::Yes)
                                                 ? 1 // Yes i want
                                                 : 0; // No please keep
            }

            if (subscriptionRemovalCheckStatus == 1) {
                if (!RemoveSubscriptionConnection(connData.subscriptionName, connData.connectionName)) {
                    QvMessageBox(this, tr("Removing this Connection"), tr("Failed to delete connection file, please delete manually."));
                }
            }
        } else {
            LOG(MODULE_CONFIG, "Unknown config type -> Not regular nor subscription...")
        }
    }

    LOG(MODULE_UI, "Saving GlobalConfig")
    SetGlobalConfig(currentConfig);
    OnConfigListChanged(false);
    ShowAndSetConnection(CurrentConnectionName, false, false);
}

void MainWindow::on_importConfigButton_clicked()
{
    ImportConfigWindow *w = new ImportConfigWindow(this);
    auto configs = w->OpenImport();

    if (!configs.isEmpty()) {
        for (auto conf : configs) {
            auto name = configs.key(conf, "");

            if (name.isEmpty())
                continue;

            SaveConnectionConfig(conf, &name, false);
            currentConfig.configs.push_back(name.toStdString());
        }

        SetGlobalConfig(currentConfig);
        OnConfigListChanged(false);
    }
}
void MainWindow::on_editConfigButton_clicked()
{
    // Check if we have a connection selected...
    if (!IsSelectionConnectable) {
        QvMessageBox(this, tr("No Config Selected"), tr("Please Select a Config"));
        return;
    }

    auto alias = connectionListWidget->selectedItems().first()->text(0);
    SUBSCRIPTION_CONFIG_MODIFY_ASK(alias)
    //
    auto outBoundRoot = connections[alias].config;
    CONFIGROOT root;
    bool isChanged = false;

    if (CheckIsComplexConfig(outBoundRoot)) {
        LOG(MODULE_UI, "INFO: Opening route editor.")
        RouteEditor *routeWindow = new RouteEditor(outBoundRoot, this);
        root = routeWindow->OpenEditor();
        isChanged = routeWindow->result() == QDialog::Accepted;
    } else {
        LOG(MODULE_UI, "INFO: Opening single connection edit window.")
        OutboundEditor *w = new OutboundEditor(OUTBOUND(outBoundRoot["outbounds"].toArray().first().toObject()), this);
        auto outboundEntry = w->OpenEditor();
        isChanged = w->result() == QDialog::Accepted;
        QJsonArray outboundsList;
        outboundsList.push_back(outboundEntry);
        root.insert("outbounds", outboundsList);
    }

    if (isChanged) {
        if (IsSubscription(alias)) {
            SaveSubscriptionConfig(root, connections[alias].subscriptionName, connections[alias].connectionName);
        } else {
            connections[alias].config = root;
            // true indicates the alias will NOT change
            SaveConnectionConfig(root, &alias, true);
        }

        OnConfigListChanged(alias == CurrentConnectionName);
    }
}
void MainWindow::on_reconnectButton_clicked()
{
    on_stopButton_clicked();
    on_startButton_clicked();
}

void MainWindow::on_action_RCM_ConvToComplex_triggered()
{
    // Check if we have a connection selected...
    if (!IsSelectionConnectable) {
        QvMessageBox(this, tr("No Config Selected"), tr("Please Select a Config"));
        return;
    }

    auto alias = connectionListWidget->currentItem()->text(0);
    SUBSCRIPTION_CONFIG_MODIFY_ASK(alias)
    //
    auto outBoundRoot = connections[alias].config;
    CONFIGROOT root;
    bool isChanged = false;
    //
    LOG(MODULE_UI, "INFO: Opening route editor.")
    RouteEditor *routeWindow = new RouteEditor(outBoundRoot, this);
    root = routeWindow->OpenEditor();
    isChanged = routeWindow->result() == QDialog::Accepted;

    if (isChanged) {
        connections[alias].config = root;
        // true indicates the alias will NOT change
        SaveConnectionConfig(root, &alias, true);
        OnConfigListChanged(alias == CurrentConnectionName);
        ShowAndSetConnection(CurrentConnectionName, false, false);
    }
}

void MainWindow::on_action_RCM_EditJson_triggered()
{
    // Check if we have a connection selected...
    if (!IsSelectionConnectable) {
        QvMessageBox(this, tr("No Config Selected"), tr("Please Select a Config"));
        return;
    }

    auto alias = connectionListWidget->currentItem()->text(0);
    SUBSCRIPTION_CONFIG_MODIFY_ASK(alias)
    JsonEditor *w = new JsonEditor(connections[alias].config, this);
    auto root = CONFIGROOT(w->OpenEditor());
    bool isChanged = w->result() == QDialog::Accepted;
    delete w;

    if (isChanged) {
        connections[alias].config = root;
        // Alias here will not change.
        SaveConnectionConfig(root, &alias, true);
        ShowAndSetConnection(CurrentConnectionName, false, false);
    }
}
void MainWindow::on_editJsonBtn_clicked()
{
    // See above.
    on_action_RCM_EditJson_triggered();
}
void MainWindow::on_pingTestBtn_clicked()
{
    // Get data from UI
    QStringList aliases;
    auto selection = connectionListWidget->selectedItems();

    if (selection.count() == 0) {
        // Ping ALL connections, warning.
        if (QvMessageBoxAsk(this, tr("Latency Test"), tr("You are about to run latency test on all servers, do you want to continue?")) == QMessageBox::Yes) {
            aliases.append(connections.keys());
        }
    } else if (selection.count() == 1) {
        if (IsSelectionConnectable) {
            // Current selection is a config
            aliases.append(selection.first()->text(0));
        } else {
            // Current selection is a subscription or... something else strange.
            // So we add another check to make sure the selected one is a subscription entry.
            if (selection.first()->childCount() > 0) {
                // Loop to add all sub-connections to the list.
                for (auto i = 0; i < selection.first()->childCount(); i++) {
                    aliases.append(selection.first()->child(i)->text(0));
                }
            }
        }
    }

    LOG(MODULE_UI, "Will perform latency test on " + to_string(aliases.count()) + " hosts.")
    latencyLabel->setText(tr("Testing..."));

    for (auto alias : aliases) {
        MWTryPingConnection(alias);
    }
}
void MainWindow::on_shareBtn_clicked()
{
    // Share QR
    if (!IsSelectionConnectable) {
        return;
    }

    auto alias = connectionListWidget->currentItem()->text(0);
    auto root = connections[alias].config;
    auto outBoundRoot = root["outbounds"].toArray().first().toObject();
    auto outboundType = outBoundRoot["protocol"].toString();

    if (!CheckIsComplexConfig(root) && outboundType == "vmess") {
        auto vmessServer = StructFromJsonString<VMessServerObject>(JsonToString(outBoundRoot["settings"].toObject()["vnext"].toArray().first().toObject()));
        auto transport = StructFromJsonString<StreamSettingsObject>(JsonToString(outBoundRoot["streamSettings"].toObject()));
        auto vmess = ConvertConfigToVMessString(transport, vmessServer, alias);
        ConfigExporter v(vmess, this);
        v.OpenExport();
    } else {
        QvMessageBox(this, tr("Share Connection"), tr("There're no support of sharing configs other than vmess"));
    }
}
void MainWindow::on_action_RCM_ShareQR_triggered()
{
    on_shareBtn_clicked();
}
void MainWindow::timerEvent(QTimerEvent *event)
{
    // Calling base class
    QMainWindow::timerEvent(event);

    if (event->timerId() == speedTimerId) {
        auto _totalSpeedUp = vinstance->getAllSpeedUp();
        auto _totalSpeedDown = vinstance->getAllSpeedDown();
        auto _totalDataUp = vinstance->getAllDataUp();
        auto _totalDataDown = vinstance->getAllDataDown();
        //
        double max = 0;
        double historyMax = 0;
        auto graphVUp  = _totalSpeedUp / 1024;
        auto graphVDown  = _totalSpeedDown / 1024;

        for (auto i = 0; i < 29; i++) {
            historyMax = MAX(historyMax, MAX(uploadList[i + 1], downloadList[i + 1]));
            uploadList[i] = uploadList[i + 1];
            downloadList[i] = downloadList[i + 1];
            uploadSerie->replace(i, i, uploadList[i + 1]);
            downloadSerie->replace(i, i, downloadList[i + 1]);
        }

        uploadList[uploadList.count() - 1] = graphVUp;
        downloadList[uploadList.count() - 1] = graphVDown;
        uploadSerie->replace(29, 29, graphVUp);
        downloadSerie->replace(29, 29, graphVDown);
        //
        max = MAX(MAX(graphVUp, graphVDown), historyMax);
        speedChartObj->axes(Qt::Vertical).first()->setRange(0, max * 1.2);
        //
        auto totalSpeedUp = FormatBytes(_totalSpeedUp) + "/s";
        auto totalSpeedDown = FormatBytes(_totalSpeedDown) + "/s";
        auto totalDataUp = FormatBytes(_totalDataUp);
        auto totalDataDown = FormatBytes(_totalDataDown);
        //
        netspeedLabel->setText(totalSpeedUp + NEWLINE + totalSpeedDown);
        dataamountLabel->setText(totalDataUp + NEWLINE + totalDataDown);
        //
        hTray->setToolTip(TRAY_TOOLTIP_PREFIX NEWLINE +
                          tr("Connected: ") + CurrentConnectionName + NEWLINE "Up: " + totalSpeedUp + " Down: " + totalSpeedDown);
    } else if (event->timerId() == logTimerId) {
        QString lastLog = readLastLog();

        if (!lastLog.isEmpty()) {
            qvAppLogBrowser->append(lastLog);
        }
    } else if (event->timerId() == pingTimerId) {
        MWTryPingConnection(CurrentConnectionName);
    }
}
void MainWindow::on_duplicateBtn_clicked()
{
    if (!IsSelectionConnectable) {
        return;
    }

    auto alias = connectionListWidget->currentItem()->text(0);
    SUBSCRIPTION_CONFIG_MODIFY_ASK(alias)
    CONFIGROOT conf;
    auto connData = connections[alias];

    if (connData.configType == CON_REGULAR) {
        conf = ConvertConfigFromFile(QV2RAY_CONFIG_DIR + alias + QV2RAY_CONFIG_FILE_EXTENSION, false);
    } else {
        conf = ConvertConfigFromFile(QV2RAY_SUBSCRIPTION_DIR + connData.subscriptionName + "/" + connData.connectionName  + QV2RAY_CONFIG_FILE_EXTENSION, false);
        alias = connData.subscriptionName + "_" + connData.connectionName;
    }

    // Alias may change.
    SaveConnectionConfig(conf, &alias, false);
    currentConfig.configs.push_back(alias.toStdString());
    SetGlobalConfig(currentConfig);
    this->OnConfigListChanged(false);
}

void MainWindow::on_subsButton_clicked()
{
    SubscribeEditor w;
    w.exec();
    OnConfigListChanged(false);
}

void MainWindow::on_connectionListWidget_itemSelectionChanged()
{
    if (!isRenamingInProgress && !IsSelectionConnectable) {
        SetEditWidgetEnable(false);
        routeCountLabel->setText(tr("Subscription"));
        _OutBoundTypeLabel->setText(tr("N/A"));
        _hostLabel->setText(tr("N/A"));
        _portLabel->setText(tr("N/A"));
        latencyLabel->setText(tr("N/A"));
    }
}

void MainWindow::onPingFinished(QvTCPingData data)
{
    if (!connections.contains(data.connectionName)) {
        return;
    }

    connections[data.connectionName].latency = data.avg;

    if (data.connectionName == currentSelectedName) {
        ShowAndSetConnection(currentSelectedName, false, false);
    }
}
