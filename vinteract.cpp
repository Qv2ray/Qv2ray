#include "vinteract.h"
#include <QProcess>
#include <QDebug>
#include <QMessageBox>
#include <QProcess>
#include "mainwindow.h"

bool validationCheck(QString path)
{
    QString output = "";
    QProcess process;
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
}
v2Instance::v2Instance()
{
    this->v2Process = new QProcess();
}
v2Instance::~v2Instance()
{
    this->v2Process->close();
    delete this;
}

void v2Instance::start(MainWindow *parent)
{
    this->v2Process->start("v2ray", QStringList() << "-config" << "config.json", QIODevice::ReadWrite | QIODevice::Text);
    this->v2Process->waitForStarted();
    QObject::connect(v2Process, SIGNAL(readyReadStandardOutput()), parent, SLOT(updateLog()));
}

void v2Instance::stop()
{
    this->v2Process->close();
}
