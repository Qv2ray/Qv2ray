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
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include "runguard.h"
#include "utils.h"

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
    QFileInfo hvConfInfo("conf/Hv2ray.config.json");
    if(!hvConfInfo.exists()) {
        QFile confFile("conf/Hv2ray.config.json");
        if(!confFile.open(QIODevice::ReadWrite)) {
            qDebug() << "Can not open Hv2ray.conf.json for read and write.";
        }
        QJsonDocument initConf;
        QJsonObject rootObj;
        QJsonArray inbounds;
        QJsonObject socks;
        QJsonObject settings;
        socks.insert("tag", "socks-in");
        socks.insert("port", 1080);
        socks.insert("listen", "127.0.0.1");
        socks.insert("protocol", "socks");
        settings.insert("auth", "noauth");
        settings.insert("udp", true);
        settings.insert("ip", "127.0.0.1");
        socks.insert("settings", QJsonValue(settings));
        inbounds.append(socks);
        rootObj.insert("inbounds", QJsonValue(inbounds));
        rootObj.insert("v2suidEnabled", false);
        initConf.setObject(rootObj);
        QByteArray byteArray = initConf.toJson(QJsonDocument::Indented);
        confFile.write(byteArray);
        confFile.close();
    }
}
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QDir::setCurrent(QFileInfo(QCoreApplication::applicationFilePath()).path());
    RunGuard guard("Hv2ray");
    if(!guard.tryToRun()) {
        alterMessage("Already running", "Another instance of Hv2ray is already running!");
        return 0;
    }
    init();
    MainWindow w;
    w.show();
    return a.exec();
}
