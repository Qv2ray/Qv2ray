#include <QApplication>
#include <QDir>
#include <iostream>
#include <QDebug>
#include <QFileInfo>
#include <QStandardPaths>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QTranslator>

#include "runguard.h"
#include "utils.h"
#include "w_MainWindow.h"
#include "w_ConnectionEditWindow.h"
#include "constants.h"

using namespace std;
using namespace Hv2rayUtils;

void firstRunCheck()
{
    ConfigDir = QDir(QDir::homePath() + HV2RAY_CONFIG_DIR_NAME);

    if(!ConfigDir.exists()) {
        QDir(QDir::homePath()).mkdir(".hv2ray");
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
    if (translator.load(":/translations/zh-CN.qm", "translations"))
    {
        cout << "Loaded Chinese translations" << endl;
    }
    else if (translator.load(":/translations/en-US.qm", "translations")){
        cout << "Loaded English translations" << endl;
    } else {
        showWarnMessageBox(nullptr, "Failed to load translations",
                           "Failed to load translations, user experience may be downgraded. \r\n \
                            无法加载语言文件，用户体验可能会降级.");
    }

    _qApp.installTranslator(&translator);

    RunGuard guard("Hv2ray-Instance-Identifier");
    if(!guard.isSingleInstance()) {
        showWarnMessageBox(nullptr, QObject::tr("Hv2Ray"), QObject::tr("AnotherInstanceRunning"));
        return -1;
    }

    QDir::setCurrent(QFileInfo(QCoreApplication::applicationFilePath()).path());

    firstRunCheck();
    MainWindow w;

    w.show();
    return _qApp.exec();
}
