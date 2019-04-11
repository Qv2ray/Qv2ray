#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "confedit.h"
#include "importconf.h"
#include <QHeaderView>
#include <QStandardItemModel>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    updateConfTable();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionEdit_triggered()
{
    ConfEdit *e = new ConfEdit(this);
    e->show();
}

void MainWindow::on_actionExisting_config_triggered()
{
    importConf *f = new importConf(this);
    f->show();
}
void MainWindow::updateConfTable()
{
    QSqlDatabase database;
    if (QSqlDatabase::contains("qt_sql_default_connection")) {
        database = QSqlDatabase::database("qt_sql_default_connection");
    } else {
        database = QSqlDatabase::addDatabase("QSQLITE");
        database.setDatabaseName(confDatabase);
    }
    if (!database.open()) {
        qDebug() << "Failed to open database while querying.";
    } else {
        QSqlQuery myQuery(database);
        myQuery.exec("select COUNT(*) from confs;");
        myQuery.first();
        int rows = myQuery.value(0).toInt();
        QStandardItemModel* model = new QStandardItemModel(rows, 5);
        ui->configTable->setModel(model);
        model->setHeaderData(0, Qt::Horizontal, "Alias");
        model->setHeaderData(1, Qt::Horizontal, "Host");
        model->setHeaderData(2, Qt::Horizontal, "Port");
        model->setHeaderData(3, Qt::Horizontal, "Checked");
        model->setHeaderData(4, Qt::Horizontal, "idInTable");
        ui->configTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        ui->configTable->setColumnHidden(4, true);
        ui->configTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        ui->configTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        myQuery.exec("select * from confs");
        myQuery.first();
        for (int i = 0; i < rows; ++i) {
            model->setItem(i, 0, new QStandardItem(myQuery.value(3).toString()));
            model->setItem(i, 1, new QStandardItem(myQuery.value(1).toString()));
            model->setItem(i, 2, new QStandardItem(myQuery.value(2).toString()));
            myQuery.next();
        }
    }
}
