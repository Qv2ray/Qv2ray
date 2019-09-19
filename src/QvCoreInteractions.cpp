#include <QObject>
#include <QWidget>
#include <QDesktopServices>
#include "QvCoreInteractions.h"
#include "QvCoreConfigOperations.h"

#include "QvTinyLog.h"
#include "w_MainWindow.h"

namespace Qv2ray
{
    bool Qv2Instance::VerifyVConfigFile(const QString *path)
    {
        if (ValidateV2rayCoreExe()) {
            QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
            env.insert("V2RAY_LOCATION_ASSET", QString::fromStdString(GetGlobalConfig().v2AssetsPath));
            QProcess process;
            process.setProcessEnvironment(env);
            process.start(QV2RAY_V2RAY_CORE_PATH, QStringList() << "-test" << "-config" << *path, QIODevice::ReadWrite | QIODevice::Text);

            if (!process.waitForFinished()) {
                LOG(MODULE_VCORE, "v2ray core failed with exitcode: " << process.exitCode())
                return false;
            }

            QString output = QString(process.readAllStandardOutput());

            if (process.exitCode() != 0) {
                Utils::QvMessageBox(nullptr, QObject::tr("Configuration Error"), output.mid(output.indexOf("anti-censorship.") + 17));
                return false;
            }

            return true;
        }

        return false;
    }

    Qv2Instance::Qv2Instance(QWidget *parent)
    {
        auto proc = new QProcess();
        vProcess = proc;
        QObject::connect(vProcess, &QProcess::readyReadStandardOutput, static_cast<MainWindow *>(parent), &MainWindow::UpdateLog);
        Status = STOPPED;
    }

    QString Qv2Instance::ReadProcessOutput()
    {
        return vProcess->readAllStandardOutput();
    }

    bool Qv2Instance::ValidateV2rayCoreExe()
    {
        if (!QFile::exists(QV2RAY_V2RAY_CORE_PATH)) {
            Utils::QvMessageBox(nullptr, QObject::tr("Cannot start v2ray"), QObject::tr("v2ray core file cannot be found at:") + QV2RAY_V2RAY_CORE_PATH);
            return false;
        } else return true;
    }

    bool Qv2Instance::Start()
    {
        if (Status != STOPPED) {
            return false;
        }

        Status = STARTING;

        if (ValidateV2rayCoreExe()) {
            auto filePath = QV2RAY_GENERATED_FILE_PATH;

            if (VerifyVConfigFile(&filePath)) {
                QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
                env.insert("V2RAY_LOCATION_ASSET", QString::fromStdString(GetGlobalConfig().v2AssetsPath));
                vProcess->setProcessEnvironment(env);
                vProcess->start(QV2RAY_V2RAY_CORE_PATH, QStringList() << "-config" << filePath, QIODevice::ReadWrite | QIODevice::Text);
                vProcess->waitForStarted();
                Status = STARTED;
                return true;
            } else {
                Status = STOPPED;
                return false;
            }
        } else {
            Status = STOPPED;
            return false;
        }
    }

    void Qv2Instance::Stop()
    {
        vProcess->close();
        Status = STOPPED;
    }

    Qv2Instance::~Qv2Instance()
    {
        Stop();
        delete vProcess;
    }
}
