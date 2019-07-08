#include <QAction>
#include <QCloseEvent>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QHeaderView>
#include <QInputDialog>
#include <QMenu>
#include <QStandardItemModel>

#include "w_PrefrencesWindow.h"
#include "w_ImportConfig.h"
#include "w_ConnectionEditWindow.h"
#include "w_MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
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
    QAction *actionRestart = new QAction(tr("#Stop"), this);
    QAction *actionStop = new QAction(tr("#Restart"), this);
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
}

void MainWindow::LoadConnections()
{
    connections = LoadAllConnectionList(GetGlobalConfig().configs);
    ui->connectionListWidget->clear();

    for (int i = 0; i < connections.count(); i++) {
        ui->connectionListWidget->addItem(connections.keys()[i]);
    }
}
void MainWindow::reload_config()
{
    bool isRunning = vinstance->Status == STARTED;
    SaveGlobalConfig();

    if (isRunning) on_stopButton_clicked();

    LoadConnections();

    if (isRunning) on_startButton_clicked();
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
    ui->logText->insertPlainText(vinstance->ReadProcessOutput());
}

void MainWindow::on_startButton_clicked()
{
    LOG(("Now start a connection: " + CurrentConnectionName).toStdString())
    ui->logText->clear();
    auto full_conf = GenerateRuntimeConfig(connections.value(CurrentConnectionName));
    StartPreparation(full_conf);
    bool startFlag = this->vinstance->Start();
    ui->statusLabel->setText(tr("#Started") + ": " + CurrentConnectionName);
    trayMenu->actions()[2]->setEnabled(!startFlag);
    trayMenu->actions()[3]->setEnabled(startFlag);
    trayMenu->actions()[4]->setEnabled(startFlag);
}

void MainWindow::on_stopButton_clicked()
{
    LOG("Stop connection!")
    this->vinstance->Stop();
    ui->statusLabel->setText(tr("#Stopped"));
    ui->logText->clear();
    trayMenu->actions()[2]->setEnabled(true);
    trayMenu->actions()[3]->setEnabled(false);
    trayMenu->actions()[4]->setEnabled(false);
}

void MainWindow::on_restartButton_clicked()
{
    on_stopButton_clicked();
    on_startButton_clicked();
}

void MainWindow::on_clbutton_clicked()
{
    ui->logText->clear();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    this->hide();
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

        case QSystemTrayIcon::DoubleClick:
            if (this->isHidden()) {
                this->show();
            }

            break;

        case QSystemTrayIcon::MiddleClick:
            if (this->vinstance->Status == Qv2ray::STOPPED) {
                on_startButton_clicked();
            } else {
                on_stopButton_clicked();
            }

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
    bar->setValue(bar->maximum());
}

void MainWindow::ShowAndSetConnection(int index, bool SetConnection, bool ApplyConnection)
{
    if (index < 0) return;

    // --------- BRGIN Show Connection
    auto obj = (connections.values()[index])["outbounds"].toArray().first().toObject();
    //
    auto vmess = ConvertOutBoundJSONToStruct(obj["settings"].toObject());
    auto Server = QList<VMessOut::ServerObject>::fromStdList(vmess.vnext).first();
    ui->_hostLabel->setText(QString::fromStdString(Server.address));
    ui->_portLabel->setText(QString::fromStdString(to_string(Server.port)));
    auto user = QList<VMessOut::ServerObject::UserObject>::fromStdList(Server.users).first();
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
    connect(w, &ImportConfigWindow::s_reload_config, this, &MainWindow::reload_config);
    w->show();
}

void MainWindow::on_addConfigBtn_clicked()
{
    ConnectionEditWindow *w = new ConnectionEditWindow(this);
    connect(w, &ConnectionEditWindow::s_reload_config, this, &MainWindow::reload_config);
    w->show();
}

void MainWindow::on_delConfigBtn_clicked()
{
}

void MainWindow::on_prefrencesBtn_clicked()
{
    PrefrencesWindow *w = new PrefrencesWindow(this);
    connect(w, &PrefrencesWindow::s_reload_config, this, &MainWindow::reload_config);
    w->ReloadCurrentConfig();
    w->show();
}

void MainWindow::on_connectionListWidget_doubleClicked(const QModelIndex &index)
{
    ShowAndSetConnection(index.row(), true, true);
}

void MainWindow::on_editConnectionSettingsBtn_clicked()
{
    ConnectionEditWindow *w = new ConnectionEditWindow(connections.value(CurrentConnectionName), this);
    connect(w, &ConnectionEditWindow::s_reload_config, this, &MainWindow::reload_config);
    w->show();
}
