#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "confedit.h"
#include "importconf.h"
#include <QHeaderView>
#include <QStandardItemModel>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QStandardItemModel* model = new QStandardItemModel(10, 4);
    ui->configTable->setModel(model);
    model->setHeaderData(0, Qt::Horizontal, "Alias");
    model->setHeaderData(1, Qt::Horizontal, "Host");
    model->setHeaderData(2, Qt::Horizontal, "Port");
    model->setHeaderData(3, Qt::Horizontal, "Checked");
    ui->configTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionEdit_triggered()
{
    ConfEdit *e = new ConfEdit();
    e->show();
}

void MainWindow::on_actionExisting_config_triggered()
{
    importConf *f = new importConf();
    f->show();
}
