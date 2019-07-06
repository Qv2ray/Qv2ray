#include <QAction>
#include <QCloseEvent>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QHeaderView>
#include <QInputDialog>
#include <QMenu>
#include <QStandardItemModel>

#include "QvUtils.h"

#include "w_MainWindow.h"

#include "QvCoreInteractions.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      hTray(new QSystemTrayIcon(this)),
      vinstance(new Qv2Instance(this)),
      connectionEditWindow(new ConnectionEditWindow(this)),
      importConfigWindow(new ImportConfigWindow(this)),
      prefrenceWindow(new PrefrencesWindow(this))
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/icons/Qv2ray.ico"));
    hTray->setIcon(this->windowIcon());
    bar = ui->logText->verticalScrollBar();
    //
    importConfigWindow->setModal(true);
    connectionEditWindow->setModal(true);
    prefrenceWindow->setModal(true);
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
    connect(actionShowHide, &QAction::triggered, this, &MainWindow::toggleMainWindowVisibility);
    connect(actionStart, &QAction::triggered, this, &MainWindow::on_startButton_clicked);
    connect(actionStop, &QAction::triggered, this, &MainWindow::on_stopButton_clicked);
    connect(actionRestart, &QAction::triggered, this, &MainWindow::on_restartButton_clicked);
    connect(actionQuit, &QAction::triggered, this, &MainWindow::quit);
    connect(connectionEditWindow, &ConnectionEditWindow::s_reload_config, this, &MainWindow::reload_config);
    connect(importConfigWindow, &ImportConfigWindow::s_reload_config, this, &MainWindow::reload_config);
    connect(prefrenceWindow, &PrefrencesWindow::s_reload_config, this, &MainWindow::reload_config);
    connect(hTray, &QSystemTrayIcon::activated, this, &MainWindow::on_activatedTray);
    connect(ui->logText, &QTextBrowser::textChanged, this, &MainWindow::scrollToBottom);
    hTray->setContextMenu(trayMenu);
    hTray->show();
}
void MainWindow::reload_config()
{
    SaveGlobalConfig();
}
MainWindow::~MainWindow()
{
    hTray->hide();
    delete this->connectionEditWindow;
    delete this->importConfigWindow;
    delete this->prefrenceWindow;
    delete this->hTray;
    delete this->vinstance;
    delete ui;
}

void MainWindow::on_actionEdit_triggered()
{
    connectionEditWindow->show();
}

void MainWindow::on_actionExisting_config_triggered()
{
    importConfigWindow->show();
}

void MainWindow::showMenu(QPoint pos)
{
    Q_UNUSED(pos)
    //    if(ui->configTable->indexAt(pos).column() != -1) {
    //        popMenu->move(cursor().pos());
    //        popMenu->show();
    //    }
}
void MainWindow::UpdateLog()
{
    ui->logText->insertPlainText(vinstance->readOutput());
}

void MainWindow::on_startButton_clicked()
{
    ui->logText->clear();
    bool startFlag = this->vinstance->start();
    trayMenu->actions()[2]->setEnabled(!startFlag);
    trayMenu->actions()[3]->setEnabled(startFlag);
    trayMenu->actions()[4]->setEnabled(startFlag);
}

void MainWindow::on_stopButton_clicked()
{
    this->vinstance->stop();
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
            toggleMainWindowVisibility();
#endif
            break;

        case QSystemTrayIcon::DoubleClick:
            if (this->isHidden()) {
                this->show();
            }

            break;

        case QSystemTrayIcon::MiddleClick:

            // TODO: Check if an alert message box is present.
            // If so, do nothing but please wait for the message box to be closed.
            if (this->vinstance->Status == Qv2ray::STOPPED) {
                on_startButton_clicked();
                LOG("Start!")
            } else {
                on_stopButton_clicked();
                LOG("Stop!")
            }

            break;

        default:
            break;
    }
}

void MainWindow::toggleMainWindowVisibility()
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
    QApplication::quit();
}

void MainWindow::on_actionExit_triggered()
{
    quit();
}

void MainWindow::scrollToBottom()
{
    bar->setValue(bar->maximum());
}

void MainWindow::on_actionPreferences_triggered()
{
    prefrenceWindow->show();
}

void MainWindow::on_pushButton_clicked()
{
    connectionEditWindow->show();
}
