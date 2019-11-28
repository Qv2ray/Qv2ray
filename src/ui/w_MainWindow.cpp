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

#include "w_OutboundEditor.hpp"
#include "w_ImportConfig.hpp"
#include "w_MainWindow.hpp"
#include "w_RoutesEditor.hpp"
#include "w_PrefrencesWindow.hpp"
#include "w_SubscriptionEditor.hpp"
#include "w_JsonEditor.hpp"
#include "w_ExportConfig.hpp"

#include "QvPingModel.hpp"
#include "QvNetSpeedPlugin.hpp"
#include "QvPACHandler.hpp"
#include "QvSystemProxyConfigurator.hpp"

#define TRAY_TOOLTIP_PREFIX "Qv2ray " QV2RAY_VERSION_STRING

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      vinstance(),
      uploadList(),
      downloadList(),
      HTTPRequestHelper(),
      hTray(new QSystemTrayIcon(this)),
      highlighter()
{
    auto conf = GetGlobalConfig();
    vinstance = new ConnectionInstance(this);
    setupUi(this);
    //
    highlighter = new Highlighter(logText->document());
    pacServer = new PACHandler();
    //
    this->setWindowIcon(QIcon(":/icons/qv2ray.png"));
    hTray->setIcon(QIcon(conf.uiConfig.useDarkTrayIcon ? ":/icons/ui_dark/tray.png" : ":/icons/ui_light/tray.png"));
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
    connect(logText, &QTextBrowser::textChanged, this, &MainWindow::QTextScrollToBottom);
    connect(action_RCM_RenameConnection, &QAction::triggered, this, &MainWindow::on_action_RenameConnection_triggered);
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
    LoadConnections();
    connect(&HTTPRequestHelper, &QvHttpRequestHelper::httpRequestFinished, this, &MainWindow::VersionUpdate);
    HTTPRequestHelper.get("https://api.github.com/repos/lhy0403/Qv2ray/releases/latest");
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
    speedChartObj->setTheme(conf.uiConfig.useDarkTheme ? QChart::ChartThemeDark : QChart::ChartThemeLight);
    speedChartObj->setTitle("Qv2ray Speed Chart");
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
    bool hasAutoStart = vinstance->ValidateKernal();

    if (hasAutoStart) {
        // At least kernal is ready.
        if (!conf.autoStartConfig.empty() && QList<string>::fromStdList(conf.configs).contains(conf.autoStartConfig)) {
            // Has auto start.
            CurrentConnectionName = QSTRING(conf.autoStartConfig);
            auto item = connectionListWidget->findItems(QSTRING(conf.autoStartConfig), Qt::MatchExactly).front();
            item->setSelected(true);
            connectionListWidget->setCurrentItem(item);
            on_connectionListWidget_itemClicked(item);
            trayMenu->actions()[0]->setText(tr("Show"));
            this->hide();
            on_startButton_clicked();
        } else if (connectionListWidget->count() != 0) {
            // The first one is default.
            connectionListWidget->setCurrentRow(0);
            ShowAndSetConnection(connectionListWidget->item(0)->text(), true, false);
            this->show();
        }
    } else {
        this->show();
    }

    StartProcessingPlugins(this);
}


void MainWindow::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return) {
        if (focusWidget() == connectionListWidget) {
            auto index = connectionListWidget->currentRow();

            if (index < 0) return;

            auto connectionName = connectionListWidget->currentItem()->text();
            ShowAndSetConnection(connectionName, true, true);
        }
    }
}


void MainWindow::on_action_StartThis_triggered()
{
    if (connectionListWidget->selectedItems().empty()) {
        QvMessageBox(this, tr("No connection selected!"), tr("Please select a config from the list."));
        return;
    }

    CurrentConnectionName = connectionListWidget->currentItem()->text();
    on_reconnectButton_clicked();
}
void MainWindow::VersionUpdate(QByteArray &data)
{
    auto conf = GetGlobalConfig();
    QString jsonString(data);
    QJsonObject root = JsonFromString(jsonString);
    //
    QVersionNumber newversion = QVersionNumber::fromString(root["tag_name"].toString("").remove(0, 1));
    QVersionNumber current = QVersionNumber::fromString(QSTRING(QV2RAY_VERSION_STRING).remove(0, 1));
    QVersionNumber ignored = QVersionNumber::fromString(QSTRING(conf.ignoredVersion));
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
            CLOG(result)
            QDesktopServices::openUrl(QUrl::fromUserInput(link));
        } else if (result == QMessageBox::Ignore) {
            conf.ignoredVersion = newversion.toString().toStdString();
            SetGlobalConfig(conf);
            OnConfigListChanged(false);
        }
    }
}
void MainWindow::LoadConnections()
{
    auto conf = GetGlobalConfig();
    connections = GetConnections(conf.configs);
    connectionListWidget->clear();

    for (int i = 0; i < connections.count(); i++) {
        connectionListWidget->addItem(connections.keys()[i]);
    }

    connectionListWidget->sortItems();
    removeConfigButton->setEnabled(false);
    editConfigButton->setEnabled(false);
    editJsonBtn->setEnabled(false);
    duplicateBtn->setEnabled(false);

    // We set the current item back...
    if (vinstance->ConnectionStatus == STARTED && !CurrentConnectionName.isEmpty()) {
        auto items = connectionListWidget->findItems(CurrentConnectionName, Qt::MatchFlag::MatchExactly);

        if (items.count() > 0) {
            connectionListWidget->setCurrentItem(items.first());
        }

        ShowAndSetConnection(CurrentConnectionName, false, false);
    }
}
void MainWindow::OnConfigListChanged(bool need_restart)
{
    auto statusText = statusLabel->text();
    //
    // A strange bug prevents us to change the UI language `live`ly
    //    https://github.com/lhy0403/Qv2ray/issues/34
    //
    //retranslateUi(this);
    statusLabel->setText(statusText);
    bool isRunning = vinstance->ConnectionStatus == STARTED;

    if (isRunning && need_restart) on_stopButton_clicked();

    LoadConnections();

    if (isRunning && need_restart) on_startButton_clicked();
}
MainWindow::~MainWindow()
{
    hTray->hide();
    delete this->hTray;
    delete this->vinstance;
}
void MainWindow::UpdateLog()
{
    logText->append(vinstance->ReadProcessOutput().trimmed());
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
        logText->clear();
        //
        auto connectionRoot = connections[CurrentConnectionName];
        //
        CurrentFullConfig = GenerateRuntimeConfig(connectionRoot);
        StartPreparation(CurrentFullConfig);
        bool startFlag = this->vinstance->StartV2rayCore();

        if (startFlag) {
            this->hTray->showMessage("Qv2ray", tr("Connected To Server: ") + CurrentConnectionName);
            hTray->setToolTip(TRAY_TOOLTIP_PREFIX "\r\n" + tr("Connected To Server: ") + CurrentConnectionName);
            statusLabel->setText(tr("Connected") + ": " + CurrentConnectionName);
            //
            auto conf = GetGlobalConfig();

            if (conf.connectionConfig.enableStats) {
                vinstance->SetAPIPort(conf.connectionConfig.statsPort);
                speedTimerId = startTimer(1000);
            }

            bool usePAC = conf.inboundConfig.pacConfig.usePAC;
            bool pacUseSocks = conf.inboundConfig.pacConfig.useSocksProxy;
            bool httpEnabled = conf.inboundConfig.useHTTP;
            bool socksEnabled = conf.inboundConfig.useSocks;

            if (usePAC) {
                bool canStartPAC = true;
                QString pacProxyString;  // Something like this --> SOCKS5 127.0.0.1:1080; SOCKS 127.0.0.1:1080; DIRECT; http://proxy:8080

                if (pacUseSocks) {
                    if (socksEnabled) {
                        pacProxyString = "SOCKS5 " + QSTRING(conf.inboundConfig.pacConfig.proxyIP) + ":" + QString::number(conf.inboundConfig.socks_port);
                    } else {
                        LOG(MODULE_UI, "PAC is using SOCKS, but it is not enabled")
                        QvMessageBox(this, tr("Configuring PAC"), tr("Could not start PAC server as it is configured to use SOCKS, but it is not enabled"));
                        canStartPAC = false;
                    }
                } else {
                    if (httpEnabled) {
                        pacProxyString = "http://" + QSTRING(conf.inboundConfig.pacConfig.proxyIP) + ":" + QString::number(conf.inboundConfig.http_port);
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

            if (conf.inboundConfig.setSystemProxy && !isComplex) {
                // Is simple config and we will try to set system proxy.
                LOG(MODULE_UI, "Preparing to set system proxy")
                //
                QString proxyAddress;
                bool canSetSystemProxy = true;

                if (usePAC) {
                    if ((httpEnabled && !pacUseSocks) || (socksEnabled && pacUseSocks)) {
                        // If we use PAC and socks/http are properly configured for PAC
                        LOG(MODULE_PROXY, "System proxy uses PAC")
                        proxyAddress = "http://" + QSTRING(conf.inboundConfig.listenip) + ":" + QString::number(conf.inboundConfig.pacConfig.port) +  "/pac";
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
                    LOG(MODULE_UI, "Setting system proxy for simple config")
                    // --------------------We only use HTTP here->>|=========|
                    SetSystemProxy(proxyAddress, conf.inboundConfig.http_port, usePAC);
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
    }
}

void MainWindow::on_stopButton_clicked()
{
    if (vinstance->ConnectionStatus != STOPPED) {
        // Is running or starting
        this->vinstance->StopV2rayCore();
        killTimer(speedTimerId);
        hTray->setToolTip(TRAY_TOOLTIP_PREFIX);
        QFile(QV2RAY_GENERATED_FILE_PATH).remove();
        statusLabel->setText(tr("Disconnected"));
        logText->setText("");
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
        // Who cares the check... (inboundConfig.pacConfig.usePAC)
        pacServer->StopServer();
        ClearSystemProxy();
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
void MainWindow::QTextScrollToBottom()
{
    auto bar = logText->verticalScrollBar();

    if (bar->value() >= bar->maximum() - 10) bar->setValue(bar->maximum());
}
void MainWindow::ShowAndSetConnection(QString guiConnectionName, bool SetConnection, bool ApplyConnection)
{
    // Check empty again...
    if (guiConnectionName.isEmpty()) return;

    //
    removeConfigButton->setEnabled(true);
    editConfigButton->setEnabled(true);
    editJsonBtn->setEnabled(true);
    duplicateBtn->setEnabled(true);
    //
    // --------- BRGIN Show Connection
    auto root = connections[guiConnectionName];
    //
    auto isComplexConfig = root["routing"].toObject()["rules"].toArray().count() > 0;
    routeCountLabel->setText(isComplexConfig > 0 ? tr("Complex") : tr("Simple"));

    if (isComplexConfig) {
        _OutBoundTypeLabel->setText(tr("N/A"));
        _hostLabel->setText(tr("N/A"));
        _portLabel->setText(tr("N/A"));
    } else {
        auto outBoundRoot = root["outbounds"].toArray().first().toObject();
        auto outboundType = outBoundRoot["protocol"].toString();
        _OutBoundTypeLabel->setText(outboundType);

        if (outboundType == "vmess") {
            auto Server = StructFromJsonString<VMessServerObject>(JsonToString(outBoundRoot["settings"].toObject()["vnext"].toArray().first().toObject()));
            _hostLabel->setText(QSTRING(Server.address));
            _portLabel->setText(QSTRING(to_string(Server.port)));
        } else if (outboundType == "shadowsocks") {
            auto x = JsonToString(outBoundRoot["settings"].toObject()["servers"].toArray().first().toObject());
            auto Server = StructFromJsonString<ShadowSocksServerObject>(x);
            _hostLabel->setText(QSTRING(Server.address));
            _portLabel->setText(QSTRING(to_string(Server.port)));
        } else if (outboundType == "socks") {
            auto x = JsonToString(outBoundRoot["settings"].toObject()["servers"].toArray().first().toObject());
            auto Server = StructFromJsonString<SocksServerObject>(x);
            _hostLabel->setText(QSTRING(Server.address));
            _portLabel->setText(QSTRING(to_string(Server.port)));
        }
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
void MainWindow::on_connectionListWidget_itemClicked(QListWidgetItem *item)
{
    Q_UNUSED(item)
    int currentRow = connectionListWidget->currentRow();

    if (currentRow < 0) return;

    QString currentText = connectionListWidget->currentItem()->text();
    bool canSetConnection = !isRenamingInProgress && vinstance->ConnectionStatus != STARTED;
    ShowAndSetConnection(currentText, canSetConnection, false);
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
    int currentRow = connectionListWidget->currentRow();

    if (currentRow < 0) return;

    QString currentText = connectionListWidget->currentItem()->text();
    ShowAndSetConnection(currentText, true, true);
}
void MainWindow::on_clearlogButton_clicked()
{
    logText->clear();
}
void MainWindow::on_connectionListWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    Q_UNUSED(previous)
    isRenamingInProgress = false;
    on_connectionListWidget_itemClicked(current);
}
void MainWindow::on_connectionListWidget_customContextMenuRequested(const QPoint &pos)
{
    Q_UNUSED(pos)
    listMenu->popup(QCursor::pos());
}
void MainWindow::on_action_RenameConnection_triggered()
{
    auto item = connectionListWidget->currentItem();
    item->setFlags(item->flags() | Qt::ItemIsEditable);
    connectionListWidget->editItem(item);
    originalName = item->text();
    isRenamingInProgress = true;
}
void MainWindow::on_connectionListWidget_itemChanged(QListWidgetItem *item)
{
    DEBUG(MODULE_UI, "A connection ListViewItem is changed.")

    if (isRenamingInProgress) {
        // In this case it's after we entered the name.
        LOG(MODULE_CONNECTION, "RENAME: " + originalName.toStdString() + " -> " + item->text().toStdString())
        auto newName = item->text();
        auto config = GetGlobalConfig();
        auto configList = QList<string>::fromStdList(config.configs);

        if (newName.trimmed().isEmpty()) {
            QvMessageBox(this, tr("Rename a Connection"), tr("The name cannot be empty"));
            return;
        }

        // If I really did some changes.
        if (originalName != newName) {
            if (configList.contains(newName.toStdString())) {
                QvMessageBox(this, tr("Rename a Connection"), tr("The name has been used already, Please choose another."));
                return;
            }

            //
            // Change auto start config.
            if (originalName.toStdString() == config.autoStartConfig) config.autoStartConfig = newName.toStdString();

            configList[configList.indexOf(originalName.toStdString())] = newName.toStdString();
            config.configs = configList.toStdList();
            //
            RenameConnection(originalName, newName);
            //
            SetGlobalConfig(config);
            bool running = CurrentConnectionName == originalName;

            if (running) CurrentConnectionName = newName;

            OnConfigListChanged(running);
            auto newItem = connectionListWidget->findItems(newName, Qt::MatchExactly).front();
            connectionListWidget->setCurrentItem(newItem);
        }
    }
}
void MainWindow::on_removeConfigButton_clicked()
{
    if (connectionListWidget->currentIndex().row() < 0) return;

    if (QvMessageBoxAsk(this, tr("Removing this Connection"), tr("Are you sure to remove this connection?")) == QMessageBox::Yes) {
        auto connectionName = connectionListWidget->currentItem()->text();

        if (connectionName == CurrentConnectionName) {
            on_stopButton_clicked();
            CurrentConnectionName = "";
        }

        auto conf = GetGlobalConfig();
        QList<string> list = QList<string>::fromStdList(conf.configs);
        list.removeOne(connectionName.toStdString());
        conf.configs = list.toStdList();

        if (!RemoveConnection(connectionName)) {
            QvMessageBox(this, tr("Removing this Connection"), tr("Failed to delete connection file, please delete manually."));
        }

        SetGlobalConfig(conf);
        OnConfigListChanged(false);
        ShowAndSetConnection(CurrentConnectionName, false, false);
    }
}
void MainWindow::on_importConfigButton_clicked()
{
    // BETA
    ImportConfigWindow *w = new ImportConfigWindow(this);
    auto configs = w->OpenImport();
    auto gConf = GetGlobalConfig();

    for (auto conf : configs) {
        auto name = configs.key(conf, "");

        if (name.isEmpty())
            continue;

        SaveConnectionConfig(conf, &name, false);
        gConf.configs.push_back(name.toStdString());
    }

    SetGlobalConfig(gConf);
    OnConfigListChanged(false);
}
void MainWindow::on_editConfigButton_clicked()
{
    // Check if we have a connection selected...
    if (connectionListWidget->currentIndex().row() < 0) {
        QvMessageBox(this, tr("No Config Selected"), tr("Please Select a Config"));
        return;
    }

    auto alias = connectionListWidget->currentItem()->text();
    auto outBoundRoot = connections[alias];
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
        connections[alias] = root;
        // true indicates the alias will NOT change
        SaveConnectionConfig(root, &alias, true);
        OnConfigListChanged(alias == CurrentConnectionName);
        ShowAndSetConnection(CurrentConnectionName, false, false);
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
    if (connectionListWidget->currentIndex().row() < 0) {
        QvMessageBox(this, tr("No Config Selected"), tr("Please Select a Config"));
        return;
    }

    auto alias = connectionListWidget->currentItem()->text();
    auto outBoundRoot = connections[alias];
    CONFIGROOT root;
    bool isChanged = false;
    //
    LOG(MODULE_UI, "INFO: Opening route editor.")
    RouteEditor *routeWindow = new RouteEditor(outBoundRoot, this);
    root = routeWindow->OpenEditor();
    isChanged = routeWindow->result() == QDialog::Accepted;

    if (isChanged) {
        connections[alias] = root;
        // true indicates the alias will NOT change
        SaveConnectionConfig(root, &alias, true);
        OnConfigListChanged(alias == CurrentConnectionName);
        ShowAndSetConnection(CurrentConnectionName, false, false);
    }
}

void MainWindow::on_action_RCM_EditJson_triggered()
{
    // Check if we have a connection selected...
    if (connectionListWidget->currentIndex().row() < 0) {
        QvMessageBox(this, tr("No Config Selected"), tr("Please Select a Config"));
        return;
    }

    auto alias = connectionListWidget->currentItem()->text();
    JsonEditor *w = new JsonEditor(connections[alias], this);
    auto root = CONFIGROOT(w->OpenEditor());
    bool isChanged = w->result() == QDialog::Accepted;
    delete w;

    if (isChanged) {
        connections[alias] = root;
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
}
void MainWindow::on_shareBtn_clicked()
{
    // Share QR
    if (connectionListWidget->currentRow() < 0) {
        return;
    }

    auto alias = connectionListWidget->currentItem()->text();
    auto root = connections[alias];
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
void MainWindow::on_action_RCM_ShareQR_triggered(bool checked)
{
    Q_UNUSED(checked)
    on_shareBtn_clicked();
}
void MainWindow::timerEvent(QTimerEvent *event)
{
    // Calling base class
    QMainWindow::timerEvent(event);
    //
    Q_UNUSED(event)
    auto inbounds = CurrentFullConfig["inbounds"].toArray();
    long _totalSpeedUp = 0, _totalSpeedDown = 0, _totalDataUp = 0, _totalDataDown = 0;

    foreach (auto inbound, inbounds) {
        auto tag = inbound.toObject()["tag"].toString();

        if (tag.isEmpty()) continue;

        // TODO: A proper scheme...
        if (tag == QV2RAY_API_TAG_INBOUND) {
            continue;
        }

        _totalSpeedUp += vinstance->getTagLastUplink(tag);
        _totalSpeedDown += vinstance->getTagLastDownlink(tag);
        _totalDataUp += vinstance->getTagTotalUplink(tag);
        _totalDataDown += vinstance->getTagTotalDownlink(tag);
    }

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
    totalSpeedUp = FormatBytes(_totalSpeedUp);
    totalSpeedDown = FormatBytes(_totalSpeedDown);
    totalDataUp = FormatBytes(_totalDataUp);
    totalDataDown = FormatBytes(_totalDataDown);
    //
    netspeedLabel->setText(totalSpeedUp + "/s\r\n" + totalSpeedDown + "/s");
    dataamountLabel->setText(totalDataUp + "\r\n" + totalDataDown);
    //
    hTray->setToolTip(TRAY_TOOLTIP_PREFIX "\r\n" + tr("Connected To Server: ") + CurrentConnectionName + "\r\nUp: " + totalSpeedUp + "/s Down: " + totalSpeedDown + "/s");
}
void MainWindow::on_duplicateBtn_clicked()
{
    if (connectionListWidget->currentRow() < 0) {
        return;
    }

    auto alias = connectionListWidget->currentItem()->text();
    auto conf = ConvertConfigFromFile(QV2RAY_CONFIG_DIR + alias + QV2RAY_CONFIG_FILE_EXTENSION, false);
    // Alias may change.
    SaveConnectionConfig(conf, &alias, false);
    auto config = GetGlobalConfig();
    config.configs.push_back(alias.toStdString());
    SetGlobalConfig(config);
    this->OnConfigListChanged(false);
}
