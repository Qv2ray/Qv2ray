#include "vinteract.h"
#include <QProcess>
#include <QDebug>
#include <QMessageBox>
#include <QProcess>
#include "mainwindow.h"
#include <QDir>

bool validationCheck(QString path)
{
    if (QFileInfo("v2ray").exists() && QFileInfo("geoip.dat").exists() && QFileInfo("geosite.dat").exists() && QFileInfo("v2ctl").exists()) {
        QString output = "";
        QProcess process;
        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
        env.insert("V2RAY_LOCATION_ASSET", QDir::currentPath());
        process.setProcessEnvironment(env);
        process.start("v2ray", QStringList() << "-test" << "-config" << path, QIODevice::ReadWrite | QIODevice::Text);
        if(!process.waitForFinished()) {
            qDebug() << "executing program failed with exit code" << process.exitCode();
        } else {
            output = QString(process.readAllStandardOutput());
            if (!output.contains("Configuration OK")) {
                QMessageBox::critical(0, "Error in configuration", output.mid(output.indexOf("anti-censorship.") + 17), QMessageBox::Ok | QMessageBox::Default, 0);
                return false;
            }
        }
        return true;
    } else {
        QMessageBox::critical(0, "v2ray core not found", "V2ray core files not found. Please download the latest version of v2ray and extract it into the current folder.", QMessageBox::Ok | QMessageBox::Default, 0);
        return false;
    }
}
v2Instance::v2Instance()
{
    this->v2Process = new QProcess();
}
v2Instance::~v2Instance()
{
    this->v2Process->close();
}

void v2Instance::start(MainWindow *parent)
{
    if(this->v2Process->state() == QProcess::Running) {
        this->v2Process->close();
    }
    if (QFileInfo("v2ray").exists() && QFileInfo("geoip.dat").exists() && QFileInfo("geosite.dat").exists() && QFileInfo("v2ctl").exists()) {
        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
        env.insert("V2RAY_LOCATION_ASSET", QDir::currentPath());
        this->v2Process->setProcessEnvironment(env);
        this->v2Process->start("./v2ray", QStringList() << "-config" << "config.json", QIODevice::ReadWrite | QIODevice::Text);
        this->v2Process->waitForStarted();
        QObject::connect(v2Process, SIGNAL(readyReadStandardOutput()), parent, SLOT(updateLog()));
    } else {
        QMessageBox::critical(0, "v2ray core not found", "V2ray core files not found. Please download the latest version of v2ray and extract it into the current folder.", QMessageBox::Ok | QMessageBox::Default, 0);
    }
}

void v2Instance::stop()
{
    this->v2Process->close();
}
