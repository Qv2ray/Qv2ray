#include <QApplication>
#include <QDir>
#include <iostream>
#include <QDebug>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QTranslator>

#include "runguard.h"
#include "utils.h"
#include "MainWindow.h"
#include "ConnectionEditWindow.h"

using namespace std;

void firstRunCheck()
{
    if(!QDir("conf").exists()) {
        QDir().mkdir("conf");
        qDebug() << "Config directory created.";
    }

    QFileInfo hvConfInfo("conf/Hv2ray.config.json");

    // First Run?
    if(!hvConfInfo.exists()) {
        QFile confFile("conf/Hv2ray.config.json");
        if(!confFile.open(QIODevice::ReadWrite)) {
            qDebug() << "Can not open Hv2ray.conf.json for read and write.";
        }

        QJsonObject settings;
        settings.insert("auth", "noauth");
        settings.insert("udp", true);
        settings.insert("ip", "127.0.0.1");

        QJsonObject socks;
        socks.insert("settings", QJsonValue(settings));
        socks.insert("tag", "socks-in");
        socks.insert("port", 1080);
        socks.insert("listen", "127.0.0.1");
        socks.insert("protocol", "socks");

        QJsonArray inbounds;
        inbounds.append(socks);

        QJsonObject rootObj;
        rootObj.insert("inbounds", QJsonValue(inbounds));
        rootObj.insert("v2suidEnabled", false);

        QJsonDocument defaultConf;
        defaultConf.setObject(rootObj);

        QByteArray byteArray = defaultConf.toJson(QJsonDocument::Indented);
        confFile.write(byteArray);
        confFile.close();
    }
}

int main(int argc, char *argv[])
{
    QApplication _qApp(argc, argv);

    QTranslator translator;
    if (translator.load(QString("zh-CN.qm"), QString("translations")))
    {
        cout << "Loaded Chinese translations" << endl;
    }
    _qApp.installTranslator(&translator);

    RunGuard guard("Hv2ray");
     if(!guard.isSingleInstance()) {
         showWarnMessageBox(nullptr, "Hv2Ray", "Another instance of Hv2ray is already running!");
         return -1;
     }

    QDir::setCurrent(QFileInfo(QCoreApplication::applicationFilePath()).path());

    firstRunCheck();
    MainWindow w;

    w.show();
    return _qApp.exec();
}
