#include "mainwindow.h"
#include "confedit.h"
#include <QApplication>
#include <QDir>
#include <iostream>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>


void init()
{
    if(!QDir(confDir).exists()) {
        QDir().mkdir(confDir);
        qDebug() << "Conf directory created.";
    }
    QFileInfo confDb(confDatabase);
    if (!confDb.exists()) {
        QSqlDatabase database;
        if (QSqlDatabase::contains("qt_sql_default_connection")) {
            database = QSqlDatabase::database("qt_sql_default_connection");
        } else {
            database = QSqlDatabase::addDatabase("QSQLITE");
            database.setDatabaseName(confDatabase);
            if(!database.open()) {
                qDebug() << "Failed to open database while creating.";
            }
        }
        QSqlQuery query(database);
        bool bsuccess =
            query.exec("create table confs(id INTEGER primary key AUTOINCREMENT, host char(50), port char(5), "
                       "alias char(80), uuid char(36), alterid char(5), security char(12), isCustom int)");
        if(!bsuccess) {
            qDebug() << "Failed to create table.";
        }
    }
}
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    init();
    return a.exec();
}
