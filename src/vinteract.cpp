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
    this->v2Process = new QProcess();
}

v2Instance::~v2Instance()
{
    this->stop();
}

bool v2Instance::start(MainWindow *parent)
{
    if(this->v2Process->state() == QProcess::Running) {
        this->stop();
    }
    if (checkVCoreExes()) {
        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
        env.insert("V2RAY_LOCATION_ASSET", QDir::currentPath());
        this->v2Process->setProcessEnvironment(env);
        this->v2Process->start("./v2ray", QStringList() << "-config" << "config.json", QIODevice::ReadWrite | QIODevice::Text);
        this->v2Process->waitForStarted();
        QObject::connect(v2Process, SIGNAL(readyReadStandardOutput()), parent, SLOT(updateLog()));
        return true;
    }
    else return false;
}

void v2Instance::stop()
{
    this->v2Process->close();
}
