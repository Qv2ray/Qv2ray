#include "vinteract.h"
#include <QProcess>
#include <QDebug>
#include <QProcess>
#include "mainwindow.h"
#include <QDir>
#include "utils.h"

bool validationCheck(QString path)
{
    if(testCoreFiles()) {
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
                alterMessage("Error in configuration", output.mid(output.indexOf("anti-censorship.") + 17));
                return false;
            }
        }
    }
}
v2Instance::v2Instance()
{
    this->v2Process = new QProcess();
}
v2Instance::~v2Instance()
{
    this->stop();
}

void v2Instance::start(MainWindow *parent)
{
    if(this->v2Process->state() == QProcess::Running) {
        this->stop();
    }
    if (testCoreFiles()) {
        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
        env.insert("V2RAY_LOCATION_ASSET", QDir::currentPath());
        this->v2Process->setProcessEnvironment(env);
        this->v2Process->start("./v2ray", QStringList() << "-config" << "config.json", QIODevice::ReadWrite | QIODevice::Text);
        this->v2Process->waitForStarted();
        QObject::connect(v2Process, SIGNAL(readyReadStandardOutput()), parent, SLOT(updateLog()));
    }
}

void v2Instance::stop()
{
    this->v2Process->close();
}
