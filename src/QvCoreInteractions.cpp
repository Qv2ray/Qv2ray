#include <QObject>
#include <QWidget>
#include "QvCoreInteractions.h"
#include "QvUtils.h"

namespace Qv2ray
{
    bool Qv2Instance::checkConfigFile(const QString path)
    {
        if (checkCoreExe()) {
            QProcess process;
            process.start(QString::fromStdString(Utils::GetGlobalConfig().v2Path), QStringList() << "-test"
                          << "-config" << path,
                          QIODevice::ReadWrite | QIODevice::Text);

            if (!process.waitForFinished()) {
                qDebug() << "v2ray core failed with exit code " << process.exitCode();
                return false;
            }

            QString output = QString(process.readAllStandardOutput());

            if (!output.contains("Configuration OK")) {
                Utils::QvMessageBox(nullptr, QObject::tr("ConfigurationError"), output.mid(output.indexOf("anti-censorship.") + 17));
                return false;
            } else
                return true;
        } else
            return false;
    }

    Qv2Instance::Qv2Instance(QWidget *parent): _config(Utils::GetGlobalConfig())
    {
        QProcess *proc = new QProcess();
        vProcess = proc;
        QObject::connect(vProcess, SIGNAL(readyReadStandardOutput()), parent, SLOT(parent->updateLog()));
        Status = STOPPED;
    }

    bool Qv2Instance::checkCoreExe()
    {
        auto path = QString::fromStdString(Utils::GetGlobalConfig().v2Path);

        if (!QFile::exists(path)) {
            Utils::QvMessageBox(nullptr, QObject::tr("CoreNotFound"), QObject::tr("CoreFileNotFoundExplainationAt:") + path);
            return false;
        } else return true;
    }

    bool Qv2Instance::start()
    {
        if (Status != STOPPED) {
            return false;
        }

        Status = STARTING;

        if (checkCoreExe()) {
            if (checkConfigFile(QV2RAY_GENERATED_CONFIG_DIRPATH + "config.json")) {
            }

            vProcess->start(QString::fromStdString(_config.v2Path), QStringList() << "-config"
                            << QV2RAY_GENERATED_CONFIG_DIRPATH + "config.json",
                            QIODevice::ReadWrite | QIODevice::Text);
            vProcess->waitForStarted();
            Status = STARTED;
            return true;
        } else {
            Status = STOPPED;
            return false;
        }
    }

    void Qv2Instance::stop()
    {
        vProcess->close();
        Status = STOPPED;
    }

    QString Qv2Instance::readOutput()
    {
        return vProcess->readAll();
    }

    Qv2Instance::~Qv2Instance()
    {
        stop();
        delete vProcess;
    }
}
