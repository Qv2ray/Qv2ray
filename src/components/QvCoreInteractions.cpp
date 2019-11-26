#include <QObject>
#include <QWidget>
#include <QDesktopServices>
#include "QvCoreInteractions.hpp"
#include "QvCoreConfigOperations.hpp"

#include "QvTinyLog.hpp"
#include "w_MainWindow.hpp"

using namespace v2ray::core::app::stats::command;
using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

// Check 20 times before telling user that API has failed.
#define QV2RAY_API_CALL_FAILEDCHECK_THRESHOLD 10

namespace Qv2ray
{
    namespace QvCoreInteration
    {
        bool ConnectionInstance::ValidateConfig(const QString &path)
        {
            if (ValidateKernal()) {
                QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
                env.insert("V2RAY_LOCATION_ASSET", QString::fromStdString(GetGlobalConfig().v2AssetsPath));
                QProcess process;
                process.setProcessEnvironment(env);
                process.start(QSTRING(GetGlobalConfig().v2CorePath), QStringList() << "-test" << "-config" << path, QIODevice::ReadWrite | QIODevice::Text);

                if (!process.waitForFinished()) {
                    LOG(MODULE_VCORE, "v2ray core failed with exitcode: " << process.exitCode())
                    return false;
                }

                QString output = QString(process.readAllStandardOutput());

                if (process.exitCode() != 0) {
                    Utils::QvMessageBox(nullptr, tr("Configuration Error"), output.mid(output.indexOf("anti-censorship.") + 17));
                    return false;
                }

                return true;
            }

            return false;
        }

        ConnectionInstance::ConnectionInstance(QWidget *parent) : apiFailedCounter(0), port(0)
        {
            auto proc = new QProcess();
            vProcess = proc;
            connect(vProcess, &QProcess::readyReadStandardOutput, static_cast<MainWindow *>(parent), &MainWindow::UpdateLog);
            ConnectionStatus = STOPPED;
        }

        void ConnectionInstance::SetAPIPort(int port)
        {
            // Config API
            this->port = port;
            Channel = grpc::CreateChannel("127.0.0.1:" + to_string(port), grpc::InsecureChannelCredentials());
            StatsService service;
            Stub = service.NewStub(Channel);
        }

        QString ConnectionInstance::ReadProcessOutput()
        {
            return vProcess->readAllStandardOutput();
        }

        bool ConnectionInstance::ValidateKernal()
        {
            if (!QFile::exists(QSTRING(GetGlobalConfig().v2CorePath))) {
                Utils::QvMessageBox(nullptr, tr("Cannot start v2ray"),
                                    tr("v2ray core file cannot be found at:") + NEWLINE +
                                    QSTRING(GetGlobalConfig().v2CorePath) + NEWLINE + NEWLINE  +
                                    tr("Please go to prefrence window to change the location.") + NEWLINE +
                                    tr("Or put v2ray core file in the location above."));
                return false;
            } else return true;
        }

        bool ConnectionInstance::StartV2rayCore()
        {
            if (ConnectionStatus != STOPPED) {
                LOG(MODULE_VCORE, "Status is invalid, expect STOPPED when calling StartV2rayCore")
                return false;
            }

            ConnectionStatus = STARTING;

            if (ValidateKernal()) {
                auto filePath = QV2RAY_GENERATED_FILE_PATH;

                if (ValidateConfig(filePath)) {
                    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
                    env.insert("V2RAY_LOCATION_ASSET", QSTRING(GetGlobalConfig().v2AssetsPath));
                    vProcess->setProcessEnvironment(env);
                    vProcess->start(QSTRING(GetGlobalConfig().v2CorePath), QStringList() << "-config" << filePath, QIODevice::ReadWrite | QIODevice::Text);
                    vProcess->waitForStarted();
                    ConnectionStatus = STARTED;
                    return true;
                } else {
                    ConnectionStatus = STOPPED;
                    return false;
                }
            } else {
                ConnectionStatus = STOPPED;
                return false;
            }
        }

        void ConnectionInstance::StopV2rayCore()
        {
            vProcess->close();
            apiFailedCounter = 0;
            totalDataTransfered.clear();
            dataTransferSpeed.clear();
            ConnectionStatus = STOPPED;
        }

        ConnectionInstance::~ConnectionInstance()
        {
            StopV2rayCore();
            delete vProcess;
        }

        long ConnectionInstance::CallStatsAPIByName(QString name)
        {
            if (ConnectionStatus != STARTED) {
                LOG(MODULE_VCORE, "Invalid connection status when calling API")
                return 0;
            }

            if (apiFailedCounter == QV2RAY_API_CALL_FAILEDCHECK_THRESHOLD) {
                LOG(MODULE_VCORE, "API call failure threshold reached, cancelling further API aclls.")
                QvMessageBox(nullptr, tr("API Call Failed"), tr("Failed to get statistics data, please check if v2ray is running properly"));
                totalDataTransfered.clear();
                dataTransferSpeed.clear();
                apiFailedCounter++;
                return 0;
            } else if (apiFailedCounter > QV2RAY_API_CALL_FAILEDCHECK_THRESHOLD) {
                return 0;
            }

            GetStatsRequest request;
            request.set_name(name.toStdString());
            request.set_reset(false);
            GetStatsResponse response;
            ClientContext context;
            Status status = Stub->GetStats(&context, request, &response);

            if (!status.ok()) {
                LOG(MODULE_VCORE, "API call returns: " << status.error_code() << " (" << status.error_message() << ")")
                apiFailedCounter++;
            }

            return response.stat().value();
        }

        long ConnectionInstance::getTagLastUplink(const QString &tag)
        {
            auto val = CallStatsAPIByName("inbound>>>" + tag + ">>>traffic>>>uplink");
            auto data = val - totalDataTransfered[tag + "_up"];
            totalDataTransfered[tag + "_up"] = val;
            dataTransferSpeed[tag + "_up"] = data;
            return data;
        }

        long ConnectionInstance::getTagLastDownlink(const QString &tag)
        {
            auto val = CallStatsAPIByName("inbound>>>" + tag + ">>>traffic>>>downlink");
            auto data = val - totalDataTransfered[tag + "_down"];
            totalDataTransfered[tag + "_down"] = val;
            dataTransferSpeed[tag + "_down"] = data;
            return data;
        }

        long ConnectionInstance::getTagTotalUplink(const QString &tag)
        {
            return totalDataTransfered[tag + "_up"];
        }

        long ConnectionInstance::getTagTotalDownlink(const QString &tag)
        {
            return totalDataTransfered[tag + "_down"];
        }
    }
}
