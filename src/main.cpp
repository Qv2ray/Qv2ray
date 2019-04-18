#include "mainwindow.h"
#include "confedit.h"
#include <QApplication>
#include <QDir>
#include <iostream>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QFileInfo>
#include <QMessageBox>
#include "runguard.h"

void init()
{
    if(!QDir("conf").exists()) {
        QDir().mkdir("conf");
        qDebug() << "Conf directory created.";
    }
    QFileInfo confdb("conf/conf.db");
    if (!confdb.exists()) {
        QSqlDatabase database;
        if (QSqlDatabase::contains("qt_sql_default_connection")) {
            database = QSqlDatabase::database("qt_sql_default_connection");
        } else {
            database = QSqlDatabase::addDatabase("QSQLITE");
            database.setDatabaseName("conf/conf.db");
            if(!database.open()) {
                qDebug() << "Failed to open database while creating.";
            }
        }
        QSqlQuery query(database);
        bool bsuccess =
            query.exec("create table confs(id INTEGER primary key AUTOINCREMENT, host char(50), port char(5), "
                       "alias char(80), uuid char(36), alterid char(5), security char(12), isCustom int, selected int);");
        if(!bsuccess) {
            qDebug() << "Failed to create table.";
        }
    }
}
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    RunGuard guard("Hv2ray");
    if(!guard.tryToRun()) {
        QMessageBox::critical(0, "Already running", "Another instance of Hv2ray is already running!", QMessageBox::Ok | QMessageBox::Default);
        return 0;
    }
    init();
    MainWindow w;
    w.show();
    return a.exec();
}
