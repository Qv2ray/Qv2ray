#include <QProcess>
#include <QDebug>
#include <QProcess>
#include <QDir>

#include "utils.h"
#include "MainWindow.h"
#include "vinteract.h"

bool validationCheck(QString path)
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
            showWarnMessageBox(nullptr, "Error in configuration", output.mid(output.indexOf("anti-censorship.") + 17));
            return false;
        }
        else return true;
    }
    else return false;
}

v2Instance::v2Instance()
{
    this->vProcess = new QProcess();
}

v2Instance::~v2Instance()
{
    this->stop();
}

bool v2Instance::start(QWidget *parent)
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
        QObject::connect(vProcess, SIGNAL(readyReadStandardOutput()), parent, SLOT(updateLog()));
        return true;
    }
    else return false;
}

void v2Instance::stop()
{
    this->vProcess->close();
}
