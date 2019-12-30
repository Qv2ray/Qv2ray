#include <QObject>
#include <QWidget>
#include <QDesktopServices>
#include "QvCoreInteractions.hpp"
#include "QvCoreConfigOperations.hpp"

#include "QvTinyLog.hpp"

using namespace v2ray::core::app::stats::command;
using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

// Check 10 times before telling user that API has failed.
#define QV2RAY_API_CALL_FAILEDCHECK_THRESHOLD 10

namespace Qv2ray
{
    namespace QvCoreInteration
    {
        bool ConnectionInstance::ValidateConfig(const QString &path)
        {
            auto conf = GetGlobalConfig();

            if (QFile::exists(conf.v2CorePath)) {
                QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
                env.insert("V2RAY_LOCATION_ASSET", conf.v2AssetsPath);
                QProcess process;
                process.setProcessEnvironment(env);
                process.start(conf.v2CorePath, QStringList() << "-test" << "-config" << path, QIODevice::ReadWrite | QIODevice::Text);

                if (!process.waitForFinished(1000) && process.exitCode() != 0) {
                    LOG(MODULE_VCORE, "v2ray core failed with exitcode: " + to_string(process.exitCode()))
                    QvMessageBox(nullptr, tr("Cannot start v2ray"), tr("v2ray core failed with errcode:") + QString::number(process.exitCode()));
                    return false;
                }

                QString output = QString(process.readAllStandardOutput());

                if (process.exitCode() != 0) {
                    QvMessageBox(nullptr, tr("Configuration Error"), output.mid(output.indexOf("anti-censorship.") + 17));
                    return false;
                }

                return true;
            } else {
                QvMessageBox(nullptr, tr("Cannot start v2ray"),
                             tr("v2ray core file cannot be found at:") + NEWLINE +
                             conf.v2CorePath + NEWLINE + NEWLINE  +
                             tr("Please go to Preference Window to change the location.") + NEWLINE +
                             tr("Or place your v2ray core file in the location above."));
                return false;
            }
        }

        ConnectionInstance::ConnectionInstance()
        {
            auto proc = new QProcess();
            vProcess = proc;
            connect(vProcess, &QProcess::readyReadStandardOutput, this, [this]() {
                emit onProcessOutputReadyRead(vProcess->readAllStandardOutput().trimmed());
            });
            ConnectionStatus = STOPPED;
        }

        bool ConnectionInstance::StartConnection(CONFIGROOT root, int apiPort)
        {
            inboundTags.clear();

            for (auto item : root["inbounds"].toArray()) {
                auto tag = item.toObject()["tag"].toString("");

                if (tag.isEmpty() || tag == QV2RAY_API_TAG_INBOUND)
                    continue;

                inboundTags.append(tag);
            }

            LOG(MODULE_VCORE, "Found Inbound Tags: " + Stringify(inboundTags).toStdString())
            QString json = JsonToString(root);
            // Write the final configuration to the disk.
            StringToFile(&json, new QFile(QV2RAY_GENERATED_FILE_PATH));

            if (ConnectionStatus != STOPPED) {
                LOG(MODULE_VCORE, "Status is invalid, expect STOPPED when calling StartV2rayCore")
                return false;
            }

            ConnectionStatus = STARTING;
            auto filePath = QV2RAY_GENERATED_FILE_PATH;

            if (ValidateConfig(filePath)) {
                QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
                env.insert("V2RAY_LOCATION_ASSET", GetGlobalConfig().v2AssetsPath);
                vProcess->setProcessEnvironment(env);
                vProcess->start(GetGlobalConfig().v2CorePath, QStringList() << "-config" << filePath, QIODevice::ReadWrite | QIODevice::Text);
                vProcess->waitForStarted();
                ConnectionStatus = STARTED;
                {
                    // Config API
                    apiFailedCounter = 0;
                    this->apiPort = apiPort;
                    Channel = grpc::CreateChannel("127.0.0.1:" + to_string(apiPort), grpc::InsecureChannelCredentials());
                    StatsService service;
                    Stub = service.NewStub(Channel);
                    apiTimerId = startTimer(1000);
                    LOG(MODULE_VCORE, "API Worker started.")
                }
                return true;
            } else {
                ConnectionStatus = STOPPED;
                return false;
            }
        }

        void ConnectionInstance::timerEvent(QTimerEvent *event)
        {
            QObject::timerEvent(event);

            if (event->timerId() == apiTimerId) {
                // Call API
                for (auto tag : inboundTags) {
                    // Upload
                    auto valup = CallStatsAPIByName("inbound>>>" + tag + ">>>traffic>>>uplink");
                    auto dataup = valup - transferData[tag + "_up"];
                    transferData[tag + "_up"] = valup;
                    transferSpeed[tag + "_up"] = dataup;
                    // Download
                    auto valdown = CallStatsAPIByName("inbound>>>" + tag + ">>>traffic>>>downlink");
                    auto datadown = valdown - transferData[tag + "_down"];
                    transferData[tag + "_down"] = valdown;
                    transferSpeed[tag + "_down"] = datadown;
                }
            }
        }

        void ConnectionInstance::StopConnection()
        {
            vProcess->close();
            killTimer(apiTimerId);
            apiFailedCounter = 0;
            transferData.clear();
            transferSpeed.clear();
            ConnectionStatus = STOPPED;
        }

        ConnectionInstance::~ConnectionInstance()
        {
            if (ConnectionStatus != STOPPED) {
                StopConnection();
            }

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
                transferData.clear();
                transferSpeed.clear();
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
                LOG(MODULE_VCORE, "API call returns: " + to_string(status.error_code()) + " (" + status.error_message() + ")")
                apiFailedCounter++;
            }

            return response.stat().value();
        }
        // ------------------------------------------------------------- API FUNCTIONS --------------------------
        long ConnectionInstance::getTagSpeedUp(const QString &tag)
        {
            return transferSpeed[tag + "_up"];
        }
        long ConnectionInstance::getTagSpeedDown(const QString &tag)
        {
            return transferSpeed[tag + "_down"];
        }
        long ConnectionInstance::getTagDataUp(const QString &tag)
        {
            return transferData[tag + "_up"];
        }
        long ConnectionInstance::getTagDataDown(const QString &tag)
        {
            return transferData[tag + "_down"];
        }
        long ConnectionInstance::getAllDataUp()
        {
            long val = 0;

            for (auto tag : inboundTags) {
                val += transferData[tag + "_up"];
            }

            return val;
        }
        long ConnectionInstance::getAllDataDown()
        {
            long val = 0;

            for (auto tag : inboundTags) {
                val += transferData[tag + "_down"];
            }

            return val;
        }
        long ConnectionInstance::getAllSpeedUp()
        {
            long val = 0;

            for (auto tag : inboundTags) {
                val += transferSpeed[tag + "_up"];
            }

            return val;
        }
        long ConnectionInstance::getAllSpeedDown()
        {
            long val = 0;

            for (auto tag : inboundTags) {
                val += transferSpeed[tag + "_down"];
            }

            return val;
        }
    }
}
