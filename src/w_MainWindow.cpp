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

#include "w_PrefrencesWindow.h"
#include "w_ImportConfig.h"
#include "w_ConnectionEditWindow.h"
#include "w_MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      HTTPRequestHelper(this),
      ui(new Ui::MainWindow),
      hTray(new QSystemTrayIcon(this)),
      vinstance(new Qv2Instance(this))
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/icons/Qv2ray.ico"));
    hTray->setIcon(this->windowIcon());
    //
    QAction *actionShowHide = new QAction(this->windowIcon(), tr("#Hide"), this);
    QAction *actionQuit = new QAction(tr("#Quit"), this);
    QAction *actionStart = new QAction(tr("#Start"), this);
    QAction *actionRestart = new QAction(tr("#Restart"), this);
    QAction *actionStop = new QAction(tr("#Stop"), this);
    actionStart->setEnabled(true);
    actionStop->setEnabled(false);
    actionRestart->setEnabled(false);
    trayMenu->addAction(actionShowHide);
    trayMenu->addSeparator();
    trayMenu->addAction(actionStart);
    trayMenu->addAction(actionStop);
    trayMenu->addAction(actionRestart);
    trayMenu->addSeparator();
    trayMenu->addAction(actionQuit);
    connect(actionShowHide, &QAction::triggered, this, &MainWindow::ToggleVisibility);
    connect(actionStart, &QAction::triggered, this, &MainWindow::on_startButton_clicked);
    connect(actionStop, &QAction::triggered, this, &MainWindow::on_stopButton_clicked);
    connect(actionRestart, &QAction::triggered, this, &MainWindow::on_restartButton_clicked);
    connect(actionQuit, &QAction::triggered, this, &MainWindow::quit);
    connect(hTray, &QSystemTrayIcon::activated, this, &MainWindow::on_activatedTray);
    connect(ui->logText, &QTextBrowser::textChanged, this, &MainWindow::QTextScrollToBottom);
    hTray->setContextMenu(trayMenu);
    hTray->show();
    LoadConnections();
    QObject::connect(&HTTPRequestHelper, &QvHttpRequestHelper::httpRequestFinished, this, &MainWindow::VersionUpdate);
    HTTPRequestHelper.get("https://api.github.com/repos/lhy0403/Qv2ray/releases/latest");

    //
    if (!vinstance->ValidateV2rayCoreExe()) {
        on_prefrencesBtn_clicked();
    }

    auto conf = GetGlobalConfig();

    if (conf.autoStartConfig != "" && QList<string>::fromStdList(conf.configs).contains(conf.autoStartConfig)) {
        CurrentConnectionName = QString::fromStdString(conf.autoStartConfig);
        auto item = ui->connectionListWidget->findItems(QString::fromStdString(conf.autoStartConfig), Qt::MatchExactly).front();
        item->setSelected(true);
        ui->connectionListWidget->setCurrentItem(item);
        on_connectionListWidget_itemClicked(item);
        on_startButton_clicked();
        ToggleVisibility();
        this->hide();
        trayMenu->actions()[0]->setText(tr("#Show"));
    } else {
        this->show();
    }
}

void MainWindow::VersionUpdate(QByteArray &data)
{
    auto conf = GetGlobalConfig();
    QString jsonString(data);
    QJsonObject root = JSONFromString(jsonString);
    //
    QVersionNumber newversion = QVersionNumber::fromString(root["tag_name"].toString("").remove(0, 1));
    QVersionNumber current = QVersionNumber::fromString(QString::fromStdString(QV2RAY_VERSION_STRING).remove(0, 1));
    QVersionNumber ignored = QVersionNumber::fromString(QString::fromStdString(conf.ignoredVersion));
    LOG("Received update info, Latest: " + newversion.toString().toStdString() + " Current: " + current.toString().toStdString() + " Ignored: " + ignored.toString().toStdString())

    // If the version is newer than us.
    // And new version is newer than the ignored version.
    if (newversion > current && newversion > ignored) {
        LOG("New version detected.")
        auto link = root["html_url"].toString("");
        auto result = QvMessageBoxAsk(this, tr("#NewReleaseVersionFound"),
                                      tr("#NewReleaseVersionFound") + ": " + root["tag_name"].toString("") +
                                      "\r\n" +
                                      root["name"].toString("") +
                                      "\r\n------------\r\n" +
                                      root["body"].toString("") +
                                      "\r\n------------\r\n" +
                                      tr("#ReleaseDownloadLink") + ": " + link, QMessageBox::Ignore);

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
}

void MainWindow::save_reload_globalconfig(bool need_restart)
{
    ui->retranslateUi(this);
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
        QvMessageBox(this, tr("#NoConfigSelected"), tr("#PleaseSelectAConfig"));
        return;
    }

    LOG(("Now start a connection: " + CurrentConnectionName).toStdString())
    ui->logText->clear();
    auto full_conf = GenerateRuntimeConfig(connections.value(CurrentConnectionName));
    StartPreparation(full_conf);
    bool startFlag = this->vinstance->Start();

    if (startFlag) {
        this->hTray->showMessage(tr("Qv2ray"), tr("#ConnectedToServer ") + CurrentConnectionName);
        ui->statusLabel->setText(tr("#Started") + ": " + CurrentConnectionName);
    }

    trayMenu->actions()[2]->setEnabled(!startFlag);
    trayMenu->actions()[3]->setEnabled(startFlag);
    trayMenu->actions()[4]->setEnabled(startFlag);
    //
    ui->startButton->setEnabled(!startFlag);
    ui->stopButton->setEnabled(startFlag);
    ui->restartButton->setEnabled(startFlag);
}

void MainWindow::on_stopButton_clicked()
{
    LOG("Stop connection!")
    this->vinstance->Stop();
    QFile(QV2RAY_GENERATED_CONFIG_FILE_PATH).remove();
    ui->statusLabel->setText(tr("#Stopped"));
    ui->logText->clear();
    trayMenu->actions()[2]->setEnabled(true);
    trayMenu->actions()[3]->setEnabled(false);
    trayMenu->actions()[4]->setEnabled(false);
    //
    ui->startButton->setEnabled(true);
    ui->stopButton->setEnabled(false);
    ui->restartButton->setEnabled(false);
}

void MainWindow::on_restartButton_clicked()
{
    on_stopButton_clicked();
    on_startButton_clicked();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    this->hide();
    trayMenu->actions()[0]->setText(tr("#Show"));
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
        trayMenu->actions()[0]->setText(tr("#Hide"));
    } else {
        this->hide();
        trayMenu->actions()[0]->setText(tr("#Show"));
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

    // --------- BRGIN Show Connection
    auto obj = (connections.values()[index])["outbounds"].toArray().first().toObject();
    //
    auto Server = StructFromJSONString<VMessServerObject>(JSONToString(obj["settings"].toObject()["vnext"].toArray().first().toObject()));
    ui->_hostLabel->setText(QString::fromStdString(Server.address));
    ui->_portLabel->setText(QString::fromStdString(to_string(Server.port)));
    auto user = QList<VMessServerObject::UserObject>::fromStdList(Server.users).first();
    ui->_uuidLabel->setText(QString::fromStdString(user.id));
    //
    ui->_transportLabel->setText(obj["streamSettings"].toObject()["network"].toString());

    // --------- END Show Connection
    //
    // Set Connection
    if (SetConnection) {
        CurrentConnectionName = connections.keys()[index];
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
    ShowAndSetConnection(currentRow, vinstance->Status != STARTED, false);
}

void MainWindow::on_importConfigBtn_clicked()
{
    ImportConfigWindow *w = new ImportConfigWindow(this);
    connect(w, &ImportConfigWindow::s_reload_config, this, &MainWindow::save_reload_globalconfig);
    w->show();
}

void MainWindow::on_addConfigBtn_clicked()
{
    ConnectionEditWindow *w = new ConnectionEditWindow(this);
    connect(w, &ConnectionEditWindow::s_reload_config, this, &MainWindow::save_reload_globalconfig);
    w->show();
}

void MainWindow::on_delConfigBtn_clicked()
{
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

void MainWindow::on_editConnectionSettingsBtn_clicked()
{
    // Check if we have a connection selected...
    if (CurrentConnectionName != "") {
        ConnectionEditWindow *w = new ConnectionEditWindow(connections.value(CurrentConnectionName), CurrentConnectionName, this);
        connect(w, &ConnectionEditWindow::s_reload_config, this, &MainWindow::save_reload_globalconfig);
        w->show();
    }
}

void MainWindow::on_clearlogButton_clicked()
{
    ui->logText->clear();
}
