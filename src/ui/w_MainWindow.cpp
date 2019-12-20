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
#include "w_PrefrencesWindow.hpp"
#include "w_SubscriptionEditor.hpp"
#include "w_JsonEditor.hpp"
#include "w_ExportConfig.hpp"

#include "QvNetSpeedPlugin.hpp"
#include "QvPACHandler.hpp"
#include "QvSystemProxyConfigurator.hpp"

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
    masterLogBrowser->document()->setDocumentMargin(0);
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
    hTray->setToolTip(TRAY_TOOLTIP_PREFIX);
    //
    QAction *action_Tray_ShowHide = new QAction(this->windowIcon(), tr("Hide"), this);
    QAction *action_Tray_Quit = new QAction(tr("Quit"), this);
    QAction *action_Tray_Start = new QAction(tr("Connect"), this);
    QAction *action_Tray_Reconnect = new QAction(tr("Reconnect"), this);
    QAction *action_Tray_Stop = new QAction(tr("Disconnect"), this);
    //
    QAction *action_RCM_RenameConnection = new QAction(tr("Rename"), this);
    QAction *action_RCM_StartThis = new QAction(tr("Connect to this"), this);
    QAction *action_RCM_ConvToComplex = new QAction(tr("Edit as Complex Config"), this);
    QAction *action_RCM_EditJson = new QAction(QICON_R("json.png"), tr("Edit as Json"), this);
    QAction *action_RCM_ShareQR = new QAction(QICON_R("share.png"), tr("Share as QRCode/VMess URL"), this);
    //
    action_Tray_Start->setEnabled(true);
    action_Tray_Stop->setEnabled(false);
    action_Tray_Reconnect->setEnabled(false);
    //
    trayMenu->addAction(action_Tray_ShowHide);
    trayMenu->addSeparator();
    trayMenu->addAction(action_Tray_Start);
    trayMenu->addAction(action_Tray_Stop);
    trayMenu->addAction(action_Tray_Reconnect);
    trayMenu->addSeparator();
    trayMenu->addAction(action_Tray_Quit);
    //
    connect(action_Tray_ShowHide, &QAction::triggered, this, &MainWindow::ToggleVisibility);
    connect(action_Tray_Start, &QAction::triggered, this, &MainWindow::on_startButton_clicked);
    connect(action_Tray_Stop, &QAction::triggered, this, &MainWindow::on_stopButton_clicked);
    connect(action_Tray_Reconnect, &QAction::triggered, this, &MainWindow::on_reconnectButton_clicked);
    connect(action_Tray_Quit, &QAction::triggered, this, &MainWindow::quit);
    connect(hTray, &QSystemTrayIcon::activated, this, &MainWindow::on_activatedTray);
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
    hTray->setContextMenu(trayMenu);
    hTray->show();
    //
    listMenu = new QMenu(this);
    listMenu->addAction(action_RCM_RenameConnection);
    listMenu->addAction(action_RCM_StartThis);
    listMenu->addAction(action_RCM_ConvToComplex);
    listMenu->addAction(action_RCM_EditJson);
    listMenu->addAction(action_RCM_ShareQR);
    //
    ReloadConnections();
    //
    // For charts
    uploadSerie = new QSplineSeries(this);
    downloadSerie = new QSplineSeries(this);
    uploadSerie->setName("Upload");
    downloadSerie->setName("Download");

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

    if (!currentConfig.autoStartConfig.connectionName.empty()) {
        // Has auto start.
        // Try to find auto start config.
        auto name = currentConfig.autoStartConfig.subscriptionName.empty()
                    ? QSTRING(currentConfig.autoStartConfig.connectionName)
                    : QSTRING(currentConfig.autoStartConfig.connectionName) + " (" + tr("Subscription:") + " " + QSTRING(currentConfig.autoStartConfig.subscriptionName) + ")";
        LOG(MODULE_UI, "Found auto start config: " + name.toStdString())
        CurrentConnectionName = name;
        auto _list = connectionListWidget->findItems(name, Qt::MatchExactly | Qt::MatchRecursive);

        if (connections.contains(name) && !_list.empty()) {
            auto item = _list.front();
            connectionListWidget->setCurrentItem(item);
            on_connectionListWidget_itemChanged(item, 0);
            connectionListWidget->scrollToItem(item);
            trayMenu->actions()[0]->setText(tr("Show"));
            this->hide();
            on_startButton_clicked();
        } else {
            QvMessageBox(this, tr("Autostarting a config"), tr("Could not find a specified config named: ") + NEWLINE +
                         name + NEWLINE + NEWLINE +
                         tr("Please reset the settings in Prefrence Window"));
            this->show();
        }
    } else if (connectionListWidget->topLevelItemCount() > 0) {
        // The first one is default.
        connectionListWidget->setCurrentItem(connectionListWidget->topLevelItem(0));
        ShowAndSetConnection(connectionListWidget->topLevelItem(0)->text(0), true, false);
    } else {
        // ensure that the main window is shown
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
    pingTestBtn->setEnabled(enabled);
    editJsonBtn->setEnabled(enabled);
    shareBtn->setEnabled(enabled);
}

void MainWindow::mouseReleaseEvent(QMouseEvent *e)
{
    Q_UNUSED(e)

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
    QJsonObject root = JsonFromString(QString(data));
    //
    QVersionNumber newversion = QVersionNumber::fromString(root["tag_name"].toString("").remove(0, 1));
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
            currentConfig.ignoredVersion = newversion.toString().toStdString();
            SetGlobalConfig(currentConfig);
        }
    }
}
void MainWindow::ReloadConnections()
{
    LOG(MODULE_UI, "Loading new GlobalConfig")
    currentConfig = GetGlobalConfig();
    //
    // Store the latency test value.
    QMap<QString, double> latencyValueCache;

    for (auto i = 0; i < connections.count(); i++) {
        latencyValueCache[connections.keys()[i]] = connections.values()[i].latency;
    }

    connections.clear();
    SetEditWidgetEnable(false);
    //
    connectionListWidget->clear();
    auto _regularConnections = GetRegularConnections(currentConfig.configs);

    for (auto i = 0; i < _regularConnections.count(); i++) {
        ConnectionObject _o;
        _o.configType = CON_REGULAR;
        _o.subscriptionName = "";
        _o.connectionName = _regularConnections.keys()[i];
        _o.config = _regularConnections.values()[i];
        // Restore latency from cache.
        _o.latency = latencyValueCache.contains(_o.connectionName) ? latencyValueCache[_o.connectionName] : 0;
        connections[_o.connectionName] = _o;
        connectionListWidget->addTopLevelItem(new QTreeWidgetItem(QStringList() << _o.connectionName));
    }

    auto _subsConnections = GetSubscriptionConnections(QMap<string, string>(currentConfig.subscribes).keys().toStdList());

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
            //
            auto connName = _o.connectionName + " (" + tr("Subscription:") + " " + _o.subscriptionName + ")";
            _o.latency = latencyValueCache.contains(connName) ? latencyValueCache[connName] : 0;
            connections[connName] = _o;
            subTopLevel->addChild(new QTreeWidgetItem(QStringList() << connName));
        }
    }

    //// We set the current item back...
    if (!CurrentConnectionName.isEmpty() && connections.contains(CurrentConnectionName)) {
        auto items = connectionListWidget->findItems(CurrentConnectionName, Qt::MatchExactly | Qt::MatchRecursive);

        if (items.count() > 0 && IsConnectableItem(items.first())) {
            connectionListWidget->setCurrentItem(items.first());
            connectionListWidget->scrollToItem(items.first());
        }

        ShowAndSetConnection(CurrentConnectionName, false, false);
    }

    // This line will trigger Segmentation Fault
    // Uncomment it if you need bombs.
    //connectionListWidget->sortItems(0, Qt::AscendingOrder);
}
void MainWindow::OnConfigListChanged(bool need_restart)
{
    bool isRunning = vinstance->ConnectionStatus == STARTED;

    if (isRunning && need_restart) on_stopButton_clicked();

    ReloadConnections();

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
        //
        auto connectionRoot = connections[CurrentConnectionName].config;
        //
        CurrentFullConfig = GenerateRuntimeConfig(connectionRoot);
        bool startFlag = this->vinstance->StartConnection(CurrentFullConfig, currentConfig.connectionConfig.enableStats, currentConfig.connectionConfig.statsPort);

        if (startFlag) {
            on_pingTestBtn_clicked();

            if (currentConfig.connectionConfig.enableStats) {
                speedTimerId = startTimer(1000);
            }

            pingTimerId = startTimer(60000);
            this->hTray->showMessage("Qv2ray", tr("Connected: ") + CurrentConnectionName, this->windowIcon());
            hTray->setToolTip(TRAY_TOOLTIP_PREFIX NEWLINE + tr("Connected: ") + CurrentConnectionName);
            statusLabel->setText(tr("Connected: ") + CurrentConnectionName);
            //
            bool usePAC = currentConfig.inboundConfig.pacConfig.enablePAC;
            bool pacUseSocks = currentConfig.inboundConfig.pacConfig.useSocksProxy;
            bool httpEnabled = currentConfig.inboundConfig.useHTTP;
            bool socksEnabled = currentConfig.inboundConfig.useSocks;

            if (usePAC) {
                bool canStartPAC = true;
                QString pacProxyString;  // Something like this --> SOCKS5 127.0.0.1:1080; SOCKS 127.0.0.1:1080; DIRECT; http://proxy:8080
                auto pacIP = QSTRING(currentConfig.inboundConfig.pacConfig.localIP);

                if (pacIP.isEmpty()) {
                    LOG(MODULE_PROXY, "PAC Local IP is empty, default to 127.0.0.1")
                    pacIP = "127.0.0.1";
                }

                if (pacUseSocks) {
                    if (socksEnabled) {
                        pacProxyString = "SOCKS5 " + pacIP + ":" + QString::number(currentConfig.inboundConfig.socks_port);
                    } else {
                        LOG(MODULE_UI, "PAC is using SOCKS, but it is not enabled")
                        QvMessageBox(this, tr("Configuring PAC"), tr("Could not start PAC server as it is configured to use SOCKS, but it is not enabled"));
                        canStartPAC = false;
                    }
                } else {
                    if (httpEnabled) {
                        pacProxyString = "PROXY http://" + pacIP + ":" + QString::number(currentConfig.inboundConfig.http_port);
                    } else {
                        LOG(MODULE_UI, "PAC is using HTTP, but it is not enabled")
                        QvMessageBox(this, tr("Configuring PAC"), tr("Could not start PAC server as it is configured to use HTTP, but it is not enabled"));
                        canStartPAC = false;
                    }
                }

                if (canStartPAC) {
                    pacServer->SetProxyString(pacProxyString);
                    pacServer->StartListen();
                } else {
                    LOG(MODULE_PROXY, "Not starting PAC due to previous error.")
                }
            }

            //
            // Set system proxy if necessary
            bool isComplex = CheckIsComplexConfig(connectionRoot);

            if (currentConfig.inboundConfig.setSystemProxy && !isComplex) {
                // Is simple config and we will try to set system proxy.
                LOG(MODULE_UI, "Preparing to set system proxy")
                //
                QString proxyAddress;
                bool canSetSystemProxy = true;

                if (usePAC) {
                    if ((httpEnabled && !pacUseSocks) || (socksEnabled && pacUseSocks)) {
                        // If we use PAC and socks/http are properly configured for PAC
                        LOG(MODULE_PROXY, "System proxy uses PAC")
                        proxyAddress = "http://" + QSTRING(currentConfig.inboundConfig.listenip) + ":" + QString::number(currentConfig.inboundConfig.pacConfig.port) +  "/pac";
                    } else {
                        // Not properly configured
                        LOG(MODULE_PROXY, "Failed to process pac due to following reasons:")
                        LOG(MODULE_PROXY, " --> PAC is configured to use socks but socks is not enabled.")
                        LOG(MODULE_PROXY, " --> PAC is configuted to use http but http is not enabled.")
                        QvMessageBox(this, tr("PAC Processing Failed"), tr("HTTP or SOCKS inbound is not properly configured for PAC") +
                                     NEWLINE + tr("Qv2ray will continue, but will not set system proxy."));
                        canSetSystemProxy = false;
                    }
                } else {
                    // Not using PAC
                    if (httpEnabled) {
                        // Not use PAC, System proxy should use HTTP
                        LOG(MODULE_PROXY, "Using system proxy with HTTP")
                        proxyAddress = "localhost";
                    } else {
                        LOG(MODULE_PROXY, "HTTP is not enabled, cannot set system proxy.")
                        QvMessageBox(this, tr("Cannot set system proxy"), tr("HTTP inbound is not enabled"));
                        canSetSystemProxy = false;
                    }
                }

                if (canSetSystemProxy) {
                    LOG(MODULE_UI, "Setting system proxy for simple config, HTTP only")
                    // --------------------We only use HTTP here->>|=========|
                    SetSystemProxy(proxyAddress, currentConfig.inboundConfig.http_port, usePAC);
                }
            }
        } else {
            // If failed, show mainwindow
            this->show();
        }

        trayMenu->actions()[2]->setEnabled(!startFlag);
        trayMenu->actions()[3]->setEnabled(startFlag);
        trayMenu->actions()[4]->setEnabled(startFlag);
        //
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
        this->vinstance->StopConnection();
        killTimer(speedTimerId);
        killTimer(pingTimerId);
        hTray->setToolTip(TRAY_TOOLTIP_PREFIX);
        QFile(QV2RAY_GENERATED_FILE_PATH).remove();
        statusLabel->setText(tr("Disconnected"));
        vCoreLogBrowser->clear();
        trayMenu->actions()[2]->setEnabled(true);
        trayMenu->actions()[3]->setEnabled(false);
        trayMenu->actions()[4]->setEnabled(false);
        //
        startButton->setEnabled(true);
        stopButton->setEnabled(false);
        //
        netspeedLabel->setText("0.00 B/s\r\n0.00 B/s");
        dataamountLabel->setText("0.00 B\r\n0.00 B");

        //
        if (currentConfig.inboundConfig.pacConfig.enablePAC) {
            pacServer->StopServer();
            LOG(MODULE_UI, "Stopping PAC server")
        }

        // BUG If unset from the prefrences. this will not work...
        if (currentConfig.inboundConfig.setSystemProxy) {
            ClearSystemProxy();
            LOG(MODULE_UI, "Clearing System Proxy")
        }

        LOG(MODULE_UI, "Stopped successfully.")
    }
}
void MainWindow::closeEvent(QCloseEvent *event)
{
    this->hide();
    trayMenu->actions()[0]->setText(tr("Show"));
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
        trayMenu->actions()[0]->setText(tr("Hide"));
    } else {
        this->hide();
        trayMenu->actions()[0]->setText(tr("Show"));
    }
}
void MainWindow::quit()
{
    StopProcessingPlugins();
    on_stopButton_clicked();
    QApplication::quit();
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
    currentGUIShownConnectionName = guiConnectionName;
    auto conf = connections[guiConnectionName];
    auto root = conf.config;
    //
    auto isComplexConfig = CheckIsComplexConfig(root);
    routeCountLabel->setText(isComplexConfig ? tr("Complex") : tr("Simple"));

    if (conf.latency == 0.0) {
        latencyLabel->setText(tr("No data"));
    } else {
        latencyLabel->setText(QString::number(conf.latency) + " " + tr("ms"));
    }

    if (conf.configType == CON_SUBSCRIPTION) {
        routeCountLabel->setText(routeCountLabel->text().append(" (" + tr("From subscription") + ":" + conf.subscriptionName + ")"));
    }

    bool validOutboundFound = false;

    for (auto item : root["outbounds"].toArray()) {
        OUTBOUND outBoundRoot = OUTBOUND(item.toObject());
        auto outboundType = outBoundRoot["protocol"].toString();

        if (outboundType == "vmess") {
            auto Server = StructFromJsonString<VMessServerObject>(JsonToString(outBoundRoot["settings"].toObject()["vnext"].toArray().first().toObject()));
            _hostLabel->setText(QSTRING(Server.address));
            _portLabel->setText(QSTRING(to_string(Server.port)));
            validOutboundFound = true;
        } else if (outboundType == "shadowsocks") {
            auto x = JsonToString(outBoundRoot["settings"].toObject()["servers"].toArray().first().toObject());
            auto Server = StructFromJsonString<ShadowSocksServerObject>(x);
            _hostLabel->setText(QSTRING(Server.address));
            _portLabel->setText(QSTRING(to_string(Server.port)));
            validOutboundFound = true;
        } else if (outboundType == "socks") {
            auto x = JsonToString(outBoundRoot["settings"].toObject()["servers"].toArray().first().toObject());
            auto Server = StructFromJsonString<SocksServerObject>(x);
            _hostLabel->setText(QSTRING(Server.address));
            _portLabel->setText(QSTRING(to_string(Server.port)));
            validOutboundFound = true;
        }

        if (validOutboundFound) {
            _OutBoundTypeLabel->setText(outboundType);

            if (isComplexConfig) {
                _OutBoundTypeLabel->setText(_OutBoundTypeLabel->text() + " (" + tr("Guessed") + ")");
            }

            break;
        }
    }

    if (!validOutboundFound) {
        _hostLabel->setText(tr("N/A"));
        _portLabel->setText(tr("N/A"));
        latencyLabel->setText(tr("N/A"));
        LOG(MODULE_UI, "Unknown outbound entry, possible very strange config file.")
    }

    // --------- END Show Connection
    //
    // Set Connection
    if (SetConnection) {
        CurrentConnectionName = guiConnectionName;
    }

    // Restart Connection
    if (ApplyConnection) {
        on_reconnectButton_clicked();
    }
}
void MainWindow::on_prefrencesBtn_clicked()
{
    PrefrencesWindow *w = new PrefrencesWindow(this);
    connect(w, &PrefrencesWindow::s_reload_config, this, &MainWindow::OnConfigListChanged);
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
        listMenu->popup(_pos);
    }
}
void MainWindow::on_action_RCM_RenameConnection_triggered()
{
    auto item = connectionListWidget->currentItem();
    SUBSCRIPTION_CONFIG_MODIFY_DENY(item->text(0))
    item->setFlags(item->flags() | Qt::ItemIsEditable);
    connectionListWidget->editItem(item);
    originalName = item->text(0);
    isRenamingInProgress = true;
}
void MainWindow::on_connectionListWidget_itemChanged(QTreeWidgetItem *item, int)
{
    DEBUG(MODULE_UI, "A connection ListViewItem is changed.")

    if (isRenamingInProgress) {
        // Should not rename a config from subscription?
        // In this case it's after we entered the name.
        LOG(MODULE_CONNECTION, "RENAME: " + originalName.toStdString() + " -> " + item->text(0).toStdString())
        auto newName = item->text(0);

        // If I really did some changes.
        if (originalName != newName) {
            bool canGo = true;

            if (newName.trimmed().isEmpty()) {
                QvMessageBox(this, tr("Rename a Connection"), tr("The name cannot be empty"));
                canGo = false;
            }

            if (contains(currentConfig.configs, newName.toStdString())) {
                QvMessageBox(this, tr("Rename a Connection"), tr("The name has been used already, Please choose another."));
                canGo = false;
            }

            if (!IsValidFileName(newName + QV2RAY_CONFIG_FILE_EXTENSION)) {
                QvMessageBox(this, tr("Rename a Connection"), tr("The name you suggested is not valid, please try another."));
                canGo = false;
            }

            if (!canGo) {
                item->setText(0, originalName);
                return;
            }

            //
            // Change auto start config.
            if (originalName.toStdString() == currentConfig.autoStartConfig.connectionName && currentConfig.autoStartConfig.subscriptionName.empty()) {
                currentConfig.autoStartConfig.connectionName = newName.toStdString();
            }

            //configList[configList.indexOf(originalName.toStdString())] = newName.toStdString();
            currentConfig.configs.remove(originalName.toStdString());
            currentConfig.configs.push_back(newName.toStdString());
            //
            RenameConnection(originalName, newName);
            //
            LOG(MODULE_UI, "Saving a global config")
            SetGlobalConfig(currentConfig);
            bool running = CurrentConnectionName == originalName;

            if (running) CurrentConnectionName = newName;

            OnConfigListChanged(running);
            //auto newItem = connectionListWidget->findItems(newName, Qt::MatchExactly | Qt::MatchRecursive).front();
            //connectionListWidget->setCurrentItem(newItem);
        }

        isRenamingInProgress = false;
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
        return;
    }

    if (QvMessageBoxAsk(this, tr("Removing Connection(s)"), tr("Are you sure to remove selected connection(s)?")) != QMessageBox::Yes) {
        return;
    }

    int subscriptionRemovalCheckStatus = -1;

    for (auto name : connlist) {
        if (name == CurrentConnectionName) {
            on_stopButton_clicked();
            CurrentConnectionName.clear();
        }

        auto connData = connections[name];

        // Remove auto start config.
        if (currentConfig.autoStartConfig.subscriptionName == connData.subscriptionName.toStdString() &&
            currentConfig.autoStartConfig.connectionName == connData.connectionName.toStdString()) {
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
    // Ping
    if (!IsSelectionConnectable) {
        return;
    }

    latencyLabel->setText(tr("Testing..."));
    // We get data from UI?
    auto alias = connectionListWidget->currentItem()->text(0);

    try {
        int port = -1;
        port = stoi(_portLabel->text().isEmpty() ? "0" : _portLabel->text().toStdString());
        tcpingModel->StartPing(alias, _hostLabel->text(), port);
    }  catch (...) {
        QvMessageBox(this, tr("Latency Test"), tr("Failed to test latency for this connection."));
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
        //
        auto totalSpeedUp = FormatBytes(_totalSpeedUp) + "/s";
        auto totalSpeedDown = FormatBytes(_totalSpeedDown) + "/s";
        auto totalDataUp = FormatBytes(_totalDataUp);
        auto totalDataDown = FormatBytes(_totalDataDown);
        //
        netspeedLabel->setText(totalSpeedUp + NEWLINE + totalSpeedDown);
        dataamountLabel->setText(totalDataUp + NEWLINE + totalDataDown);
        //
        hTray->setToolTip(TRAY_TOOLTIP_PREFIX NEWLINE + tr("Connected: ") + CurrentConnectionName + NEWLINE
                          "Up: " + totalSpeedUp + " Down: " + totalSpeedDown);
    } else if (event->timerId() == logTimerId) {
        QString lastLog = readLastLog();

        if (!lastLog.isEmpty()) {
            qvAppLogBrowser->append(lastLog);
        }
    } else if (event->timerId() == pingTimerId) {
        on_pingTestBtn_clicked();
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

    if (data.connectionName == currentGUIShownConnectionName) {
        ShowAndSetConnection(currentGUIShownConnectionName, false, false);
    }
}
