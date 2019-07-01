#include <QProcess>
#include <QDebug>
#include <QProcess>
#include <QDir>

#include "utils.h"
#include "w_MainWindow.h"
#include "vinteract.h"

bool v2Instance::checkConfigFile(QString path)
{
    if(checkVCoreExes()) {
        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
        env.insert("V2RAY_LOCATION_ASSET", QDir::currentPath());

        QProcess process;
        process.setProcessEnvironment(env);
        process.start("v2ray", QStringList() << "-test" << "-config" << path, QIODevice::ReadWrite | QIODevice::Text);

        if(!process.waitForFinished()) {
            qDebug() << "v2ray core failed with exit code " << process.exitCode();
            return false;
        }

        QString output = QString(process.readAllStandardOutput());

        if (!output.contains("Configuration OK")) {
            showWarnMessageBox(nullptr, QObject::tr("ConfigurationError"), output.mid(output.indexOf("anti-censorship.") + 17));
            return false;
        }
        else return true;
    }
    else return false;
}

v2Instance::v2Instance(QWidget *parent)
{
    this->vProcess = new QProcess();
    QObject::connect(vProcess, SIGNAL(readyReadStandardOutput()), parent, SLOT(updateLog()));
}

bool v2Instance::checkVCoreExes()
{
    if (QFileInfo("v2ray").exists() && QFileInfo("geoip.dat").exists() && QFileInfo("geosite.dat").exists() && QFileInfo("v2ctl").exists()) {
        return true;
    }
    else {
        showWarnMessageBox(nullptr, QObject::tr("CoreNotFound"), QObject::tr("CoreFileNotFoundExplaination"));
        return false;
    }
}

bool v2Instance::start()
{
    if(this->vProcess->state() == QProcess::Running) {
        this->stop();
    }
    if (checkVCoreExes()) {
        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
        env.insert("V2RAY_LOCATION_ASSET", QDir::currentPath());
        this->vProcess->setProcessEnvironment(env);
        this->vProcess->start("./v2ray", QStringList() << "-config" << "config.json", QIODevice::ReadWrite | QIODevice::Text);
        this->vProcess->waitForStarted();
        processStatus = STARTED;
        return true;
    }
    else return false;
}

void v2Instance::stop()
{
    this->vProcess->close();
    processStatus = STOPPED;
}

v2Instance::~v2Instance()
{
    this->stop();
}
