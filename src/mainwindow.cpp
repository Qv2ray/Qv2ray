#include <QHeaderView>
#include <QStandardItemModel>
#include <QDebug>
#include <QMenu>
#include <QAction>
#include <QFile>
#include <QCloseEvent>
#include <QFileInfo>
#include <QInputDialog>

#include "import_vmess.h"
#include "PrefrencesWindow.h"
#include "mainwindow.h"
#include <ui_mainwindow.h>
#include "ConnectionEditWindow.h"
#include "importconf.h"
#include "vinteract.h"
#include "utils.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    this->setWindowIcon(QIcon("Himeki.ico"));
    ui->setupUi(this);
    updateConfTable();
//    ui->configTable->setContextMenuPolicy(Qt::CustomContextMenu);
//    connect(ui->configTable, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showMenu(QPoint)));
    this->v2Inst = new v2Instance();
    hTray = new QSystemTrayIcon();
    hTray->setToolTip("Hv2ray");
    hTray->setIcon(QIcon("Himeki.ico"));
    connect(hTray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(on_activatedTray(QSystemTrayIcon::ActivationReason)));
    createTrayAction();
    if(QFileInfo("config.json").exists()) {
        v2Inst->start(this);
    }

//    QAction *select = new QAction("Select", ui->configTable);
//    QAction *del = new QAction("Delete", ui->configTable);
//    QAction *rename = new QAction("Rename", ui->configTable);
//    popMenu->addAction(select);
//    popMenu->addAction(del);
//    popMenu->addAction(rename);
//    connect(select, SIGNAL(triggered()), this, SLOT(select_triggered()));
//    connect(del, SIGNAL(triggered()), this, SLOT(delConf()));
//    connect(rename, SIGNAL(triggered()), this, SLOT(renameRow()));
//    connect(ui->logText, SIGNAL(textChanged()), this, SLOT(scrollToBottom()));
//    bar = ui->logText->verticalScrollBar();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete this->v2Inst;
}

void MainWindow::on_actionEdit_triggered()
{
    ConnectionEditWindow *e = new ConnectionEditWindow(this);
    e->setAttribute(Qt::WA_DeleteOnClose);
    e->show();
}

void MainWindow::on_actionExisting_config_triggered()
{
    importConf *f = new importConf(this);
    f->setAttribute(Qt::WA_DeleteOnClose);
    f->show();
}
void MainWindow::showMenu(QPoint pos)
{
//    if(ui->configTable->indexAt(pos).column() != -1) {
//        popMenu->move(cursor().pos());
//        popMenu->show();
//    }
}
void MainWindow::select_triggered()
{
//    int row = ui->configTable->selectionModel()->currentIndex().row();
//    int idIntable = ui->configTable->model()->data(ui->configTable->model()->index(row, 4)).toInt();
//    this->geneConf(idIntable);
//    if(this->v2Inst->v2Process->state() == QProcess::Running) {
//        this->on_restartButton_clicked();
//    }
}

void MainWindow::delConf()
{

}
void MainWindow::updateConfTable()
{

}
void MainWindow::generateConfig(int idIntable)
{
    vConfig tmpConf;
    emit updateConfTable();
    if (tmpConf.isCustom == 1) {
        QString src = "conf/" + QString::number(idIntable) + ".conf";
        overrideInbounds(src);
        if (QFile::exists("config.json")) {
            QFile::remove("config.json");
        }
        QFile::copy(src, "config.json");
    } else {
        // TODO: Config generator
    }
}
void MainWindow::updateLog()
{
    ui->logText->insertPlainText(this->v2Inst->v2Process->readAllStandardOutput());
}

void MainWindow::on_startButton_clicked()
{
    ui->logText->clear();
    bool startFlag = this->v2Inst->start(this);
    trayMenu->actions()[2]->setEnabled(!startFlag);
    trayMenu->actions()[3]->setEnabled(startFlag);
    trayMenu->actions()[4]->setEnabled(startFlag);
}

void MainWindow::on_stopButton_clicked()
{
    this->v2Inst->stop();
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

void MainWindow::on_rtButton_clicked()
{
    emit updateConfTable();
}

void MainWindow::on_actionVmess_triggered()
{
    import_vmess *inVmess = new import_vmess(this);
    inVmess->show();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    this->hide();
    event->ignore();
}

void MainWindow::createTrayAction()
{
    QAction *actionShow = new QAction(this);
    QAction *actionQuit = new QAction(this);
    QAction *actionStart = new QAction(this);
    QAction *actionRestart = new QAction(this);
    QAction *actionStop = new QAction(this);
    actionShow->setText("Hide");
    actionQuit->setText("Quit Hv2ray");
    actionStart->setText("Start v2ray");
    actionStop->setText("Stop v2ray");
    actionRestart->setText("Restart v2ray");
    actionStart->setEnabled(true);
    actionStop->setEnabled(false);
    actionRestart->setEnabled(false);
    trayMenu->addAction(actionShow);
    trayMenu->addSeparator();
    trayMenu->addAction(actionStart);
    trayMenu->addAction(actionStop);
    trayMenu->addAction(actionRestart);
    trayMenu->addSeparator();
    trayMenu->addAction(actionQuit);
    connect(actionShow, SIGNAL(triggered()), this, SLOT(toggleMainWindowVisibility()));
    connect(actionStart, SIGNAL(triggered()), this, SLOT(on_startButton_clicked()));
    connect(actionStop, SIGNAL(triggered()), this, SLOT(on_stopButton_clicked()));
    connect(actionRestart, SIGNAL(triggered()), this, SLOT(on_restartButton_clicked()));
    connect(actionQuit, SIGNAL(triggered()), this, SLOT(quit()));
    hTray->setContextMenu(trayMenu);
    hTray->show();
}

void MainWindow::on_activatedTray(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
        case QSystemTrayIcon::Trigger:
            // Toggle Show/Hide
            toggleMainWindowVisibility();
            break;
        case QSystemTrayIcon::DoubleClick:
            //if(this->isHidden()) {
            //    this->show();
            //}
            break;
        case QSystemTrayIcon::MiddleClick:
            // TODO: Check if an alert message box is present.
            // If so, do nothing but please wait for the message box to be closed.
            if(this->v2Inst->v2Process->state() == QProcess::ProcessState::Running) {
                on_stopButton_clicked();
            } else {
                on_startButton_clicked();
            }
            break;
        case QSystemTrayIcon::Unknown:
            break;
        case QSystemTrayIcon::Context:
            break;
    }
}

void MainWindow::toggleMainWindowVisibility()
{
    if(this->isHidden()) {
        this->show();
        trayMenu->actions()[0]->setText("Hide");
    } else {
        this->hide();
        trayMenu->actions()[0]->setText("Show");
    }
}

void MainWindow::quit()
{
    QCoreApplication::quit();
}

void MainWindow::on_actionExit_triggered()
{
    quit();
}

void MainWindow::renameRow()
{
//    QString text = QInputDialog::getText(this, "Rename config", "New name:", QLineEdit::Normal);
//    int row = ui->configTable->currentIndex().row();
//    int idIntable = ui->configTable->model()->data(ui->configTable->model()->index(row, 4)).toInt();
//    SQLiteDB mydb;
//    QString updateString = "update confs set alias = '" + text + "' where id = " + QString::number(idIntable);
//    mydb.DoQuery(updateString);
//    emit updateConfTable();
}

void MainWindow::scrollToBottom()
{
    bar->setValue(bar->maximum());
}

void MainWindow::on_actionPreferences_triggered()
{
    PrefrencesWindow *v = new PrefrencesWindow(this);
    v->setAttribute(Qt::WA_DeleteOnClose);
    v->show();
}

void MainWindow::on_pushButton_clicked()
{
    auto confedit = new ConnectionEditWindow();
    confedit->show();
}
