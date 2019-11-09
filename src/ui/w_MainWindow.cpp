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

#include "QvNetSpeedPlugin.hpp"

#define TRAY_TOOLTIP_PREFIX "Qv2ray " QV2RAY_VERSION_STRING

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      vinstance(),
      ui(new Ui::MainWindow),
      uploadList(),
      downloadList(),
      HTTPRequestHelper(),
      hTray(new QSystemTrayIcon(this))
{
    auto conf = GetGlobalConfig();
    vinstance = new Qv2Instance(this);
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/icons/qv2ray.ico"));
    hTray->setIcon(this->windowIcon());
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
    QAction *action_RCM_EditJson = new QAction(tr("Edit as Json"), this);
    QAction *action_RCM_ShareLink = new QAction(tr("Share as vmess://"), this);
    QAction *action_RCM_ShareQR = new QAction(tr("Share as QRCore"), this);
    //
    action_Tray_Start->setEnabled(true);
    action_Tray_Stop->setEnabled(false);
    action_Tray_Reconnect->setEnabled(false);
    trayMenu->addAction(action_Tray_ShowHide);
    trayMenu->addSeparator();
    trayMenu->addAction(action_Tray_Start);
    trayMenu->addAction(action_Tray_Stop);
    trayMenu->addAction(action_Tray_Reconnect);
    trayMenu->addSeparator();
    trayMenu->addAction(action_Tray_Quit);
    connect(action_Tray_ShowHide, &QAction::triggered, this, &MainWindow::ToggleVisibility);
    connect(action_Tray_Start, &QAction::triggered, this, &MainWindow::on_startButton_clicked);
    connect(action_Tray_Stop, &QAction::triggered, this, &MainWindow::on_stopButton_clicked);
    connect(action_Tray_Reconnect, &QAction::triggered, this, &MainWindow::on_reconnectButton_clicked);
    connect(action_Tray_Quit, &QAction::triggered, this, &MainWindow::quit);
    connect(hTray, &QSystemTrayIcon::activated, this, &MainWindow::on_activatedTray);
    connect(ui->logText, &QTextBrowser::textChanged, this, &MainWindow::QTextScrollToBottom);
    connect(action_RCM_RenameConnection, &QAction::triggered, this, &MainWindow::on_action_RenameConnection_triggered);
    connect(action_RCM_StartThis, &QAction::triggered, this, &MainWindow::on_action_StartThis_triggered);
    connect(action_RCM_EditJson, &QAction::triggered, this, &MainWindow::on_action_RCM_EditJson_triggered);
    // TODO: UNCOMMENT THIS....
    LOG(MODULE_UI, "SHARE OPTION TODO...")
    //connect(action_RCM_ShareLink, &QAction::triggered, this, &MainWindow::on_action_RCM_ShareLink_triggered);
    //connect(action_RCM_ShareQR, &QAction::triggered, this, &MainWindow::on_action_RCM_ShareQR_triggered);
    //
    connect(this, &MainWindow::Connect, this, &MainWindow::on_startButton_clicked);
    connect(this, &MainWindow::DisConnect, this, &MainWindow::on_stopButton_clicked);
    connect(this, &MainWindow::ReConnect, this, &MainWindow::on_reconnectButton_clicked);
    //
    hTray->setContextMenu(trayMenu);
    hTray->show();
    //
    listMenu.addAction(action_RCM_RenameConnection);
    listMenu.addAction(action_RCM_StartThis);
    listMenu.addAction(action_RCM_EditJson);
    listMenu.addAction(action_RCM_ShareLink);
    listMenu.addAction(action_RCM_ShareQR);
    //
    LoadConnections();
    QObject::connect(&HTTPRequestHelper, &QvHttpRequestHelper::httpRequestFinished, this, &MainWindow::VersionUpdate);
    HTTPRequestHelper.get("https://api.github.com/repos/lhy0403/Qv2ray/releases/latest");
    bool hasAutoStart  = false;
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

    speedChart = new QChart();
    speedChart->setTheme(conf.UISettings.useDarkChartStyle ? QChart::ChartThemeDark : QChart::ChartThemeLight);
    speedChart->setTitle("Qv2ray Speed Chart");
    speedChart->legend()->hide();
    speedChart->createDefaultAxes();
    speedChart->addSeries(uploadSerie);
    speedChart->addSeries(downloadSerie);
    speedChart->createDefaultAxes();
    speedChart->axes(Qt::Vertical).first()->setRange(0, 512);
    static_cast<QValueAxis>(speedChart->axes(Qt::Horizontal).first()).setLabelFormat("dd.dd");
    speedChart->axes(Qt::Horizontal).first()->setRange(0, 30);
    speedChart->setContentsMargins(-20, -45, -20, -25);
    speedChartView = new QChartView(speedChart, this);
    speedChartView->setRenderHint(QPainter::RenderHint::HighQualityAntialiasing, true);
    auto layout = new QHBoxLayout(ui->speedChart);
    layout->addWidget(speedChartView);
    ui->speedChart->setLayout(layout);

    //
    if (vinstance->ValidateKernal()) {
        if (conf.autoStartConfig != "" && QList<string>::fromStdList(conf.configs).contains(conf.autoStartConfig)) {
            CurrentConnectionName = QSTRING(conf.autoStartConfig);
            auto item = ui->connectionListWidget->findItems(QSTRING(conf.autoStartConfig), Qt::MatchExactly).front();
            item->setSelected(true);
            ui->connectionListWidget->setCurrentItem(item);
            on_connectionListWidget_itemClicked(item);
            on_startButton_clicked();
            hasAutoStart = true;
            trayMenu->actions()[0]->setText(tr("Show"));
        } else {
            if (ui->connectionListWidget->count() != 0) {
                // The first one is default.
                ui->connectionListWidget->setCurrentRow(0);
                ShowAndSetConnection(ui->connectionListWidget->item(0)->text(), true, false);
            }
        }
    }

    if (hasAutoStart) {
        this->hide();
    } else {
        this->show();
    }

    Utils::NetSpeedPlugin::StartProcessingPlugins(this);
}

void MainWindow::on_action_StartThis_triggered()
{
    if (ui->connectionListWidget->selectedItems().empty()) {
        QvMessageBox(this, tr("No connection selected!"), tr("Please select a config from the list."));
        return;
    }

    CurrentConnectionName = ui->connectionListWidget->currentItem()->text();
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
    ui->connectionListWidget->clear();

    for (int i = 0; i < connections.count(); i++) {
        ui->connectionListWidget->addItem(connections.keys()[i]);
    }

    ui->connectionListWidget->sortItems();
    ui->removeConfigButton->setEnabled(false);
    ui->editConfigButton->setEnabled(false);
    ui->editJsonBtn->setEnabled(false);
    ui->duplicateBtn->setEnabled(false);

    // We set the current item back...
    if (vinstance->VCoreStatus == STARTED && !CurrentConnectionName.isEmpty()) {
        auto items = ui->connectionListWidget->findItems(CurrentConnectionName, Qt::MatchFlag::MatchExactly);

        if (items.count() > 0) {
            ui->connectionListWidget->setCurrentItem(items.first());
        }

        ShowAndSetConnection(CurrentConnectionName, false, false);
    }
}
void MainWindow::OnConfigListChanged(bool need_restart)
{
    auto statusText = ui->statusLabel->text();
    //
    // A strange bug prevents us to change the UI language `live`ly
    //    https://github.com/lhy0403/Qv2ray/issues/34
    //
    //ui->retranslateUi(this);
    ui->statusLabel->setText(statusText);
    bool isRunning = vinstance->VCoreStatus == STARTED;

    if (isRunning && need_restart) on_stopButton_clicked();

    LoadConnections();

    if (isRunning && need_restart) on_startButton_clicked();
}
MainWindow::~MainWindow()
{
    hTray->hide();
    delete this->hTray;
    delete this->vinstance;
    delete ui;
}
void MainWindow::UpdateLog()
{
    ui->logText->append(vinstance->ReadProcessOutput().trimmed());
}
void MainWindow::on_startButton_clicked()
{
    if (vinstance->VCoreStatus != STARTED) {
        // Reset the graph
        for (int i = 0; i < 30 ; i++) {
            uploadList[i] = 0;
            downloadList[i] = 0;
            uploadSerie->replace(i, 0, 0);
            downloadSerie->replace(i, 0, 0);
        }

        if (CurrentConnectionName.isEmpty()) {
            QvMessageBox(this, tr("No connection selected!"), tr("Please select a config from the list."));
            return;
        }

        LOG(MODULE_VCORE, ("Connecting to: " + CurrentConnectionName).toStdString())
        ui->logText->clear();
        CurrentFullConfig = GenerateRuntimeConfig(connections[CurrentConnectionName]);
        StartPreparation(CurrentFullConfig);
        bool startFlag = this->vinstance->StartVCore();

        if (startFlag) {
            this->hTray->showMessage("Qv2ray", tr("Connected To Server: ") + CurrentConnectionName);
            hTray->setToolTip(TRAY_TOOLTIP_PREFIX "\r\n" + tr("Connected To Server: ") + CurrentConnectionName);
            ui->statusLabel->setText(tr("Connected") + ": " + CurrentConnectionName);

            if (GetGlobalConfig().enableStats) {
                vinstance->SetAPIPort(GetGlobalConfig().statsPort);
                speedTimerId = startTimer(1000);
            }
        }

        trayMenu->actions()[2]->setEnabled(!startFlag);
        trayMenu->actions()[3]->setEnabled(startFlag);
        trayMenu->actions()[4]->setEnabled(startFlag);
        //
        ui->startButton->setEnabled(!startFlag);
        ui->stopButton->setEnabled(startFlag);
    }
}
void MainWindow::on_stopButton_clicked()
{
    if (vinstance->VCoreStatus != STOPPED) {
        this->vinstance->StopVCore();
        killTimer(speedTimerId);
        hTray->setToolTip(TRAY_TOOLTIP_PREFIX);
        QFile(QV2RAY_GENERATED_FILE_PATH).remove();
        ui->statusLabel->setText(tr("Disconnected"));
        ui->logText->setText("");
        trayMenu->actions()[2]->setEnabled(true);
        trayMenu->actions()[3]->setEnabled(false);
        trayMenu->actions()[4]->setEnabled(false);
        //
        ui->startButton->setEnabled(true);
        ui->stopButton->setEnabled(false);
        //
        ui->netspeedLabel->setText("0.00 B/s\r\n0.00 B/s");
        ui->dataamountLabel->setText("0.00 B\r\n0.00 B");
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
            if (this->vinstance->VCoreStatus == STARTED) {
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
#ifdef _WIN32
        setWindowState(Qt::WindowNoState);
        SetWindowPos(HWND(this->winId()), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
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
    Utils::NetSpeedPlugin::StopProcessingPlugins();
    on_stopButton_clicked();
    QApplication::quit();
}
void MainWindow::on_actionExit_triggered()
{
    quit();
}
void MainWindow::QTextScrollToBottom()
{
    auto bar = ui->logText->verticalScrollBar();

    if (bar->value() >= bar->maximum() - 10) bar->setValue(bar->maximum());
}
void MainWindow::ShowAndSetConnection(QString guiConnectionName, bool SetConnection, bool ApplyConnection)
{
    // Check empty again...
    if (guiConnectionName.isEmpty()) return;

    // --------- BRGIN Show Connection
    auto outBoundRoot = (connections[guiConnectionName])["outbounds"].toArray().first().toObject();
    //
    auto outboundType = outBoundRoot["protocol"].toString();
    ui->_OutBoundTypeLabel->setText(outboundType);
    ui->removeConfigButton->setEnabled(true);
    ui->editConfigButton->setEnabled(true);
    ui->editJsonBtn->setEnabled(true);
    ui->duplicateBtn->setEnabled(true);

    if (outboundType == "vmess") {
        auto Server = StructFromJsonString<VMessServerObject>(JsonToString(outBoundRoot["settings"].toObject()["vnext"].toArray().first().toObject()));
        ui->_hostLabel->setText(QSTRING(Server.address));
        ui->_portLabel->setText(QSTRING(to_string(Server.port)));
    } else if (outboundType == "shadowsocks") {
        auto x = JsonToString(outBoundRoot["settings"].toObject()["servers"].toArray().first().toObject());
        auto Server = StructFromJsonString<ShadowSocksServerObject>(x);
        ui->_hostLabel->setText(QSTRING(Server.address));
        ui->_portLabel->setText(QSTRING(to_string(Server.port)));
    } else if (outboundType == "socks") {
        auto x = JsonToString(outBoundRoot["settings"].toObject()["servers"].toArray().first().toObject());
        auto Server = StructFromJsonString<SocksServerObject>(x);
        ui->_hostLabel->setText(QSTRING(Server.address));
        ui->_portLabel->setText(QSTRING(to_string(Server.port)));
    }

    ui->routeCountLabel->setText(QString::number((connections[guiConnectionName])["routing"].toArray().count()));

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
    int currentRow = ui->connectionListWidget->currentRow();

    if (currentRow < 0) return;

    QString currentText = ui->connectionListWidget->currentItem()->text();
    bool canSetConnection = !isRenamingInProgress && vinstance->VCoreStatus != STARTED;
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
    int currentRow = ui->connectionListWidget->currentRow();

    if (currentRow < 0) return;

    QString currentText = ui->connectionListWidget->currentItem()->text();
    ShowAndSetConnection(currentText, true, true);
}
void MainWindow::on_clearlogButton_clicked()
{
    ui->logText->clear();
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
    listMenu.popup(QCursor::pos());
}
void MainWindow::on_action_RenameConnection_triggered()
{
    auto item = ui->connectionListWidget->currentItem();
    item->setFlags(item->flags() | Qt::ItemIsEditable);
    ui->connectionListWidget->editItem(item);
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
            auto newItem = ui->connectionListWidget->findItems(newName, Qt::MatchExactly).front();
            ui->connectionListWidget->setCurrentItem(newItem);
        }
    }
}
void MainWindow::on_removeConfigButton_clicked()
{
    if (ui->connectionListWidget->currentIndex().row() < 0) return;

    if (QvMessageBoxAsk(this, tr("Removing this Connection"), tr("Are you sure to remove this connection?")) == QMessageBox::Yes) {
        auto connectionName = ui->connectionListWidget->currentItem()->text();

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
    // TODO
    ImportConfigWindow *w = new ImportConfigWindow(this);
    //w->OpenImporter();
    OnConfigListChanged(false);
}
void MainWindow::on_addConfigButton_clicked()
{
    OutboundEditor *w = new OutboundEditor(this);
    connect(w, &OutboundEditor::s_reload_config, this, &MainWindow::OnConfigListChanged);
    auto outboundEntry = w->OpenEditor();
    bool isChanged = w->result() == QDialog::Accepted;
    QString alias = w->GetFriendlyName();
    delete w;

    if (isChanged) {
        QJsonArray outboundsList;
        outboundsList.push_back(outboundEntry);
        QJsonObject root;
        root.insert("outbounds", outboundsList);
        //
        // WARN This one will change the connection name, because of some duplicates.
        SaveConnectionConfig(root, &alias, false);
        //
        auto conf = GetGlobalConfig();
        auto connectionList = conf.configs;
        connectionList.push_back(alias.toStdString());
        conf.configs = connectionList;
        SetGlobalConfig(conf);
        OnConfigListChanged(false);
        ShowAndSetConnection(CurrentConnectionName, false, false);
    }
}
void MainWindow::on_editConfigButton_clicked()
{
    // Check if we have a connection selected...
    if (ui->connectionListWidget->currentIndex().row() < 0) {
        QvMessageBox(this, tr("No Config Selected"), tr("Please Select a Config"));
        return;
    }

    auto alias = ui->connectionListWidget->currentItem()->text();
    auto outBoundRoot = connections[alias];
    QJsonObject root;
    bool isChanged = false;

    if (outBoundRoot["outbounds"].toArray().count() > 1) {
        LOG(MODULE_UI, "INFO: Opening route editor.")
        RouteEditor *routeWindow = new RouteEditor(outBoundRoot, this);
        root = routeWindow->OpenEditor();
        isChanged = routeWindow->result() == QDialog::Accepted;
    } else {
        LOG(MODULE_UI, "INFO: Opening single connection edit window.")
        OutboundEditor *w = new OutboundEditor(outBoundRoot["outbounds"].toArray().first().toObject(), this);
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
void MainWindow::on_action_RCM_EditJson_triggered()
{
    // Check if we have a connection selected...
    if (ui->connectionListWidget->currentIndex().row() < 0) {
        QvMessageBox(this, tr("No Config Selected"), tr("Please Select a Config"));
        return;
    }

    auto alias = ui->connectionListWidget->currentItem()->text();
    JsonEditor *w = new JsonEditor(connections[alias], this);
    auto root = w->OpenEditor();
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
void MainWindow::on_shareQRButton_clicked()
{
    // Share QR
}
void MainWindow::on_shareVMessButton_clicked()
{
    // Share vmess://
}
void MainWindow::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event)
    auto inbounds = CurrentFullConfig["inbounds"].toArray();
    long _totalSpeedUp = 0, _totalSpeedDown = 0, _totalDataUp = 0, _totalDataDown = 0;

    foreach (auto inbound, inbounds) {
        auto tag = inbound.toObject()["tag"].toString();

        // TODO: A proper scheme...
        if (tag == API_TAG_INBOUND) {
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
    speedChart->axes(Qt::Vertical).first()->setRange(0, max * 1.2);
    //
    //
    totalSpeedUp = FormatBytes(_totalSpeedUp);
    totalSpeedDown = FormatBytes(_totalSpeedDown);
    totalDataUp = FormatBytes(_totalDataUp);
    totalDataDown = FormatBytes(_totalDataDown);
    //
    ui->netspeedLabel->setText(totalSpeedUp + "/s\r\n" + totalSpeedDown + "/s");
    ui->dataamountLabel->setText(totalDataUp + "\r\n" + totalDataDown);
    //
    hTray->setToolTip(TRAY_TOOLTIP_PREFIX "\r\n" + tr("Connected To Server: ") + CurrentConnectionName + "\r\nUp: " + totalSpeedUp + "/s Down: " + totalSpeedDown + "/s");
}
void MainWindow::on_duplicateBtn_clicked()
{
    if (ui->connectionListWidget->currentRow() < 0) {
        return;
    }

    auto alias = ui->connectionListWidget->currentItem()->text();
    auto conf = ConvertConfigFromFile(QV2RAY_CONFIG_DIR + alias + QV2RAY_CONFIG_FILE_EXTENSION, false);
    // Alias may change.
    SaveConnectionConfig(conf, &alias, false);
    auto config = GetGlobalConfig();
    config.configs.push_back(alias.toStdString());
    SetGlobalConfig(config);
    this->OnConfigListChanged(false);
}
