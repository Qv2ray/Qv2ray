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

#ifdef _WIN32
#include <windows.h>
#endif

#include "w_PrefrencesWindow.h"
#include "w_ImportConfig.h"
#include "w_ConnectionEditWindow.h"
#include "w_MainWindow.h"
#include "w_RouteEditor.h"
#include "w_SubscribeEditor.h"

#define TRAY_TOOLTIP_PREFIX "Qv2ray " QV2RAY_VERSION_STRING "\r\n"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      HTTPRequestHelper(),
      hTray(new QSystemTrayIcon(this)),
      vinstance(new Qv2Instance(this))
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/icons/Qv2ray.ico"));
    hTray->setIcon(this->windowIcon());
    hTray->setToolTip(TRAY_TOOLTIP_PREFIX);
    //
    QAction *action_Tray_ShowHide = new QAction(this->windowIcon(), tr("Hide"), this);
    QAction *action_Tray_Quit = new QAction(tr("Quit"), this);
    QAction *action_Tray_Start = new QAction(tr("Connect"), this);
    QAction *action_Tray_Restart = new QAction(tr("Reconnect"), this);
    QAction *action_Tray_Stop = new QAction(tr("Disconnect"), this);
    //
    QAction *action_RCM_RenameConnection = new QAction(tr("Rename"), this);
    QAction *action_RCM_StartThis = new QAction(tr("ConnectSelected"), this);
    action_Tray_Start->setEnabled(true);
    action_Tray_Stop->setEnabled(false);
    action_Tray_Restart->setEnabled(false);
    trayMenu->addAction(action_Tray_ShowHide);
    trayMenu->addSeparator();
    trayMenu->addAction(action_Tray_Start);
    trayMenu->addAction(action_Tray_Stop);
    trayMenu->addAction(action_Tray_Restart);
    trayMenu->addSeparator();
    trayMenu->addAction(action_Tray_Quit);
    connect(action_Tray_ShowHide, &QAction::triggered, this, &MainWindow::ToggleVisibility);
    connect(action_Tray_Start, &QAction::triggered, this, &MainWindow::on_startButton_clicked);
    connect(action_Tray_Stop, &QAction::triggered, this, &MainWindow::on_stopButton_clicked);
    connect(action_Tray_Restart, &QAction::triggered, this, &MainWindow::on_restartButton_clicked);
    connect(action_Tray_Quit, &QAction::triggered, this, &MainWindow::quit);
    connect(hTray, &QSystemTrayIcon::activated, this, &MainWindow::on_activatedTray);
    connect(ui->logText, &QTextBrowser::textChanged, this, &MainWindow::QTextScrollToBottom);
    connect(action_RCM_RenameConnection, &QAction::triggered, this, &MainWindow::on_action_RenameConnection_triggered);
    connect(action_RCM_StartThis, &QAction::triggered, this, &MainWindow::on_action_StartThis_triggered);
    //
    hTray->setContextMenu(trayMenu);
    hTray->show();
    //
    listMenu.addAction(action_RCM_StartThis);
    listMenu.addAction(action_RCM_RenameConnection);
    //
    LoadConnections();
    QObject::connect(&HTTPRequestHelper, &QvHttpRequestHelper::httpRequestFinished, this, &MainWindow::VersionUpdate);
    HTTPRequestHelper.get("https://api.github.com/repos/lhy0403/Qv2ray/releases/latest");

    //
    if (!vinstance->ValidateV2rayCoreExe()) {
        on_prefrencesBtn_clicked();
    }

    auto conf = GetGlobalConfig();

    if (conf.autoStartConfig != "" && QList<string>::fromStdList(conf.configs).contains(conf.autoStartConfig)) {
        CurrentConnectionName = QSTRING(conf.autoStartConfig);
        auto item = ui->connectionListWidget->findItems(QSTRING(conf.autoStartConfig), Qt::MatchExactly).front();
        item->setSelected(true);
        ui->connectionListWidget->setCurrentItem(item);
        on_connectionListWidget_itemClicked(item);
        on_startButton_clicked();
        //ToggleVisibility();
        this->hide();
        trayMenu->actions()[0]->setText(tr("Show"));
    } else {
        this->show();
    }
}

void MainWindow::on_action_StartThis_triggered()
{
    CurrentConnectionName = ui->connectionListWidget->currentItem()->text();
    on_restartButton_clicked();
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
        auto result = QvMessageBoxAsk(this, tr("NewReleaseVersionFound"),
                                      tr("NewReleaseVersionFound") + ": " + root["tag_name"].toString("") +
                                      "\r\n" +
                                      root["name"].toString("") +
                                      "\r\n------------\r\n" +
                                      root["body"].toString("") +
                                      "\r\n------------\r\n" +
                                      tr("ReleaseDownloadLink") + ": " + link, QMessageBox::Ignore);

        if (result == QMessageBox::Yes) {
            QDesktopServices::openUrl(QUrl::fromUserInput(link));
        } else if (result == QMessageBox::Ignore) {
            conf.ignoredVersion = newversion.toString().toStdString();
            SetGlobalConfig(conf);
            save_reload_globalconfig(false);
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
}

void MainWindow::save_reload_globalconfig(bool need_restart)
{
    auto statusText = ui->statusLabel->text();
    //
    // A strange bug prevents us to change the UI language `live`ly
    //    https://github.com/lhy0403/Qv2ray/issues/34
    //
    //ui->retranslateUi(this);
    ui->statusLabel->setText(statusText);
    bool isRunning = vinstance->Status == STARTED;
    SaveGlobalConfig();

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
    if (CurrentConnectionName == "") {
        QvMessageBox(this, tr("NoConfigSelected"), tr("PleaseSelectAConfig"));
        return;
    }

    LOG(MODULE_VCORE, ("Connecting to: " + CurrentConnectionName).toStdString())
    ui->logText->clear();
    auto full_conf = GenerateRuntimeConfig(connections.value(CurrentConnectionName));
    StartPreparation(full_conf);
    bool startFlag = this->vinstance->Start();

    if (startFlag) {
        this->hTray->showMessage("Qv2ray", tr("ConnectedToServer") + " " + CurrentConnectionName);
        hTray->setToolTip(TRAY_TOOLTIP_PREFIX + tr("ConnectedToServer") + ": " + CurrentConnectionName);
        ui->statusLabel->setText(tr("Connected") + ": " + CurrentConnectionName);
    }

    trayMenu->actions()[2]->setEnabled(!startFlag);
    trayMenu->actions()[3]->setEnabled(startFlag);
    trayMenu->actions()[4]->setEnabled(startFlag);
    //
    ui->startButton->setEnabled(!startFlag);
    ui->stopButton->setEnabled(startFlag);
}

void MainWindow::on_stopButton_clicked()
{
    if (vinstance->Status != STOPPED) {
        this->vinstance->Stop();
        hTray->setToolTip(TRAY_TOOLTIP_PREFIX);
        QFile(QV2RAY_GENERATED_FILE_PATH).remove();
        ui->statusLabel->setText(tr("Disconnected"));
        ui->logText->clear();
        trayMenu->actions()[2]->setEnabled(true);
        trayMenu->actions()[3]->setEnabled(false);
        trayMenu->actions()[4]->setEnabled(false);
        //
        ui->startButton->setEnabled(true);
        ui->stopButton->setEnabled(false);
    }
}

void MainWindow::on_restartButton_clicked()
{
    on_stopButton_clicked();
    on_startButton_clicked();
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
            // So, as a hobby on common MacOS Apps, we 'don't toggle visibility on click'.
            ToggleVisibility();
#endif
            break;

        case QSystemTrayIcon::MiddleClick:
            if (this->vinstance->Status == Qv2ray::STOPPED) {
                on_startButton_clicked();
            } else {
                on_stopButton_clicked();
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

void MainWindow::ShowAndSetConnection(int index, bool SetConnection, bool ApplyConnection)
{
    if (index < 0) return;

    auto guiConnectionName = ui->connectionListWidget->item(index)->text();
    // --------- BRGIN Show Connection
    auto outBoundRoot = (connections[guiConnectionName])["outbounds"].toArray().first().toObject();
    //
    auto outboundType = outBoundRoot["protocol"].toString();
    ui->_OutBoundTypeLabel->setText(outboundType);

    if (outboundType == "vmess") {
        auto Server = StructFromJsonString<VMessServerObject>(JsonToString(outBoundRoot["settings"].toObject()["vnext"].toArray().first().toObject()));
        ui->_hostLabel->setText(QSTRING(Server.address));
        ui->_portLabel->setText(QSTRING(to_string(Server.port)));
        auto user = QList<VMessServerObject::UserObject>::fromStdList(Server.users).first();
        auto _configString = tr("UUID") + ": " + QSTRING(user.id)
                             + "\r\n"
                             + tr("AlterID") + ": " + QSTRING(to_string(user.alterId))
                             + "\r\n"
                             + tr("Transport") + ": " + outBoundRoot["streamSettings"].toObject()["network"].toString();
        ui->detailInfoTxt->setPlainText(_configString);
    } else if (outboundType == "shadowsocks") {
        auto x = JsonToString(outBoundRoot["settings"].toObject()["servers"].toArray().first().toObject());
        auto Server = StructFromJsonString<ShadowSocksServer>(x);
        ui->_hostLabel->setText(QSTRING(Server.address));
        ui->_portLabel->setText(QSTRING(to_string(Server.port)));
        auto _configString = tr("Email") + ": " + QSTRING(Server.email)
                             + "\r\n"
                             + tr("Encryption") + ": " + QSTRING(Server.method);
        ui->detailInfoTxt->setPlainText(_configString);
    } else if (outboundType == "socks") {
        auto x = JsonToString(outBoundRoot["settings"].toObject()["servers"].toArray().first().toObject());
        auto Server = StructFromJsonString<SocksServerObject>(x);
        ui->_hostLabel->setText(QSTRING(Server.address));
        ui->_portLabel->setText(QSTRING(to_string(Server.port)));
        auto _configString = tr("Username") + ": " + QSTRING(Server.users.front().user);
        ui->detailInfoTxt->setPlainText(_configString);
    }

    // --------- END Show Connection
    //
    // Set Connection
    if (SetConnection) {
        CurrentConnectionName = guiConnectionName;
    }

    // Restart Connection
    if (ApplyConnection) {
        on_restartButton_clicked();
    }
}

void MainWindow::on_connectionListWidget_itemClicked(QListWidgetItem *item)
{
    Q_UNUSED(item)
    int currentRow = ui->connectionListWidget->currentRow();
    ShowAndSetConnection(currentRow, !isRenamingInProgress && (vinstance->Status != STARTED), false);
}

void MainWindow::on_prefrencesBtn_clicked()
{
    PrefrencesWindow *w = new PrefrencesWindow(this);
    connect(w, &PrefrencesWindow::s_reload_config, this, &MainWindow::save_reload_globalconfig);
    w->show();
}

void MainWindow::on_connectionListWidget_doubleClicked(const QModelIndex &index)
{
    ShowAndSetConnection(index.row(), true, true);
}

void MainWindow::on_clearlogButton_clicked()
{
    ui->logText->clear();
}

void MainWindow::on_connectionListWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    Q_UNUSED(previous)
    isRenamingInProgress = true;
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
    isRenamingInProgress = false;
}

void MainWindow::on_connectionListWidget_itemChanged(QListWidgetItem *item)
{
    LOG(MODULE_UI, "A connection ListViewItem is changed.")

    if (!isRenamingInProgress) {
        // In this case it's after we entered the name.
        LOG(MODULE_CONNECTION, "RENAME: " + originalName.toStdString() + " -> " + item->text().toStdString())
        auto newName = item->text();
        auto config = GetGlobalConfig();
        auto configList = QList<string>::fromStdList(config.configs);

        if (newName.trimmed().isEmpty()) {
            QvMessageBox(this, tr("RenameConnection"), tr("CannotUseEmptyName"));
            return;
        }

        // If I really did some changes.
        LOG("RENAME", "ORIGINAL: " + originalName.toStdString() + ", NEW: " + newName.toStdString())

        if (originalName != newName) {
            if (configList.contains(newName.toStdString())) {
                QvMessageBox(this, tr("RenameConnection"), tr("DuplicatedConnectionName"));
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

            save_reload_globalconfig(running);
            auto newItem = ui->connectionListWidget->findItems(newName, Qt::MatchExactly).front();
            ui->connectionListWidget->setCurrentItem(newItem);
        }
    }
}

void MainWindow::on_removeConfigButton_clicked()
{
    if (QvMessageBoxAsk(this, tr("RemoveConnection"), tr("RemoveConnectionConfirm")) == QMessageBox::Yes) {
        auto conf = GetGlobalConfig();
        QList<string> list = QList<string>::fromStdList(conf.configs);
        auto currentSelected = ui->connectionListWidget->currentIndex().row();

        if (currentSelected < 0) return;

        bool isRemovingItemRunning = ui->connectionListWidget->item(currentSelected)->text() == CurrentConnectionName;

        if (isRemovingItemRunning) {
            CurrentConnectionName  = "";
        }

        list.removeOne(ui->connectionListWidget->item(currentSelected)->text().toStdString());
        conf.configs = list.toStdList();
        SetGlobalConfig(conf);
        save_reload_globalconfig(isRemovingItemRunning);
    }
}

void MainWindow::on_importConfigButton_clicked()
{
    ImportConfigWindow *w = new ImportConfigWindow(this);
    connect(w, &ImportConfigWindow::s_reload_config, this, &MainWindow::save_reload_globalconfig);
    w->exec();
}

void MainWindow::on_addConfigButton_clicked()
{
    ConnectionEditWindow *w = new ConnectionEditWindow(this);
    connect(w, &ConnectionEditWindow::s_reload_config, this, &MainWindow::save_reload_globalconfig);
    w->exec();
    LOG(MODULE_UI, "WARNING:")
    LOG(MODULE_UI, "THIS FEATURE IS NOT IMPLEMENTED YET!")
    auto outboundEntry = w->Result;
    auto alias = w->Alias;
    delete w;
}
void MainWindow::on_editConfigButton_clicked()
{
    // Check if we have a connection selected...
    auto index = ui->connectionListWidget->currentIndex().row();

    if (index < 0) {
        QvMessageBox(this, tr("NoConfigSelected"), tr("PleaseSelectAConfig"));
        return;
    }

    auto alias = ui->connectionListWidget->currentItem()->text();
    auto outBoundRoot = connections[ui->connectionListWidget->currentItem()->text()]["outbounds"].toArray().first().toObject();
    ConnectionEditWindow *w = new ConnectionEditWindow(outBoundRoot, &alias, this);
    //connect(w, &ConnectionEditWindow::s_reload_config, this, &MainWindow::save_reload_globalconfig);
    w->exec();
    auto outboundEntry = w->Result;
    LOG(MODULE_UI, "WARNING:")
    LOG(MODULE_UI, "THIS FEATURE IS NOT IMPLEMENTED YET!")
    delete w;
}
void MainWindow::on_editConfigAdvButton_clicked()
{
    // Check if we have a connection selected...
    auto index = ui->connectionListWidget->currentIndex().row();

    if (index < 0) {
        QvMessageBox(this, tr("#NoConfigSelected"), tr("#PleaseSelectAConfig"));
        return;
    }

    auto alias = ui->connectionListWidget->currentItem()->text();
    RouteEditor *w = new RouteEditor(connections[ui->connectionListWidget->currentItem()->text()], alias, this);
    w->exec();
}

void MainWindow::on_pushButton_clicked()
{
    SubscribeEditor *w = new SubscribeEditor(this);
    w->show();
}
