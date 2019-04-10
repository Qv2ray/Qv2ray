#include "vinteract.h"
#include <QProcess>
#include <QDebug>
#include <QMessageBox>

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
