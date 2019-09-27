#include <QObject>
#include <QWidget>
#include <QDesktopServices>
#include "QvCoreInteractions.h"
#include "QvCoreConfigOperations.h"

#include "QvTinyLog.h"
#include "w_MainWindow.h"

namespace Qv2ray
{
    bool Qv2Instance::ValidateConfig(const QString *path)
    {
        if (ValidateKernal()) {
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
        VCoreStatus = STOPPED;
    }

    void Qv2Instance::SetPort(int port)
    {
        // Config API
        this->port = port;
        Channel = grpc::CreateChannel("127.0.0.1:" + to_string(port), grpc::InsecureChannelCredentials());
        StatsService service;
        Stub = service.NewStub(Channel);
    }

    QString Qv2Instance::ReadProcessOutput()
    {
        return vProcess->readAllStandardOutput();
    }

    bool Qv2Instance::ValidateKernal()
    {
        if (!QFile::exists(QV2RAY_V2RAY_CORE_PATH)) {
            Utils::QvMessageBox(nullptr, QObject::tr("Cannot start v2ray"), QObject::tr("v2ray core file cannot be found at:") + QV2RAY_V2RAY_CORE_PATH);
            return false;
        } else return true;
    }

    bool Qv2Instance::StartVCore()
    {
        if (VCoreStatus != STOPPED) {
            return false;
        }

        VCoreStatus = STARTING;

        if (ValidateKernal()) {
            auto filePath = QV2RAY_GENERATED_FILE_PATH;

            if (ValidateConfig(&filePath)) {
                QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
                env.insert("V2RAY_LOCATION_ASSET", QString::fromStdString(GetGlobalConfig().v2AssetsPath));
                vProcess->setProcessEnvironment(env);
                vProcess->start(QV2RAY_V2RAY_CORE_PATH, QStringList() << "-config" << filePath, QIODevice::ReadWrite | QIODevice::Text);
                vProcess->waitForStarted();
                VCoreStatus = STARTED;
                return true;
            } else {
                VCoreStatus = STOPPED;
                return false;
            }
        } else {
            VCoreStatus = STOPPED;
            return false;
        }
    }

    void Qv2Instance::StopVCore()
    {
        vProcess->close();
        VCoreStatus = STOPPED;
    }

    Qv2Instance::~Qv2Instance()
    {
        StopVCore();
        delete vProcess;
    }

    long Qv2Instance::CallStatsAPIByName(QString name)
    {
        GetStatsRequest request;
        request.set_name(name.toStdString());
        request.set_reset(false);
        GetStatsResponse response;
        ClientContext context;
        Status status = Stub->GetStats(&context, request, &response);

        if (!status.ok()) {
            LOG(MODULE_VCORE, "API call returns: " + to_string(status.error_code()))
        }

        return response.stat().value();
    }

    long Qv2Instance::getTagLastUplink(QString tag)
    {
        auto val = CallStatsAPIByName("inbound>>>" + tag + ">>>traffic>>>uplink");
        auto data = val - lastData[tag + "_up"];
        lastData[tag + "_up"] = val;
        return data;
    }

    long Qv2Instance::getTagLastDownlink(QString tag)
    {
        auto val = CallStatsAPIByName("inbound>>>" + tag + ">>>traffic>>>downlink");
        auto data = val - lastData[tag + "_down"];
        lastData[tag + "_down"] = val;
        return data;
    }

    long Qv2Instance::getTagTotalUplink(QString tag)
    {
        return lastData[tag + "_up"];
    }

    long Qv2Instance::getTagTotalDownlink(QString tag)
    {
        return lastData[tag + "_down"];
    }
}
