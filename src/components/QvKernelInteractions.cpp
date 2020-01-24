#include <QObject>
#include <QWidget>
#include <QDesktopServices>
#include "QvKernelInteractions.hpp"
#include "QvCoreConfigOperations.hpp"
#include "QvCore/QvCommandLineArgs.hpp"

#ifdef WITH_LIB_GRPCPP
using namespace v2ray::core::app::stats::command;
using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
#else
#include "libs/libqvb/build/libqvb.h"
#endif

// Check 10 times before telling user that API has failed.
#define QV2RAY_API_CALL_FAILEDCHECK_THRESHOLD 10

namespace Qv2ray
{
    namespace QvKernelInterations
    {
        bool V2rayKernelInstance::ValidateKernel(const QString &vCorePath, const QString &vAssetsPath, QString *message)
        {
            QFile coreFile(vCorePath);

            if (!coreFile.exists()) {
                DEBUG(MODULE_VCORE, "V2ray core file cannot be found.")
                *message = tr("V2ray core executable not found.");
                return false;
            }

            // Use open() here to prevent `executing` a folder, which may have the same name as the V2ray core.
            if (!coreFile.open(QFile::ReadOnly)) {
                DEBUG(MODULE_VCORE, "V2ray core file cannot be opened, possibly be a folder?")
                *message = tr("V2ray core file cannot be opened, please ensure there's a file instead of a folder.");
                return false;
            }

            coreFile.close();
            //
            // Check file existance.
            // From: https://www.v2fly.org/chapter_02/env.html#asset-location
            //
            bool hasGeoIP = FileExistsIn(QDir(vAssetsPath), "geoip.dat");
            bool hasGeoSite = FileExistsIn(QDir(vAssetsPath), "geosite.dat");

            if (!hasGeoIP && !hasGeoSite) {
                DEBUG(MODULE_VCORE, "V2ray assets path contains none of those two files.")
                *message = tr("V2ray assets path is not valid.");
                return false;
            }

            if (!hasGeoIP) {
                DEBUG(MODULE_VCORE, "No geoip.dat in assets path, aborting.")
                *message =  tr("No geoip.dat in assets path.");
                return false;
            }

            if (!hasGeoSite) {
                DEBUG(MODULE_VCORE, "No geosite.dat in assets path, aborting.")
                *message =  tr("No geosite.dat in assets path.");
                return false;
            }

            // Check if V2ray core returns a version number correctly.
            QProcess proc;
#ifdef Q_OS_WIN32
            // nativeArguments are required for Windows platform, without a reason...
            proc.setProcessChannelMode(QProcess::MergedChannels);
            proc.setProgram(vCorePath);
            proc.setNativeArguments("--version");
            proc.start();
#else
            proc.start(vCorePath + " --version");
#endif
            proc.waitForStarted();
            proc.waitForFinished();
            auto exitCode = proc.exitCode();

            if (exitCode != 0) {
                DEBUG(MODULE_VCORE, "VCore failed with an exit code: " + QSTRN(exitCode))
                *message = tr("V2ray core failed with an exit code: ") + QSTRN(exitCode);
                return false;
            }

            QString output = proc.readAllStandardOutput();
            LOG(MODULE_VCORE, "V2ray output: " + SplitLines(output).join(";"))

            if (SplitLines(output).isEmpty()) {
                *message = tr("V2ray core returns empty string.");
                return false;
            }

            *message = SplitLines(output).first();
            return true;
        }


        bool V2rayKernelInstance::ValidateConfig(const QString &path)
        {
            auto conf = GetGlobalConfig();
            QString v2rayCheckResult;

            if (ValidateKernel(conf.v2CorePath, conf.v2AssetsPath, &v2rayCheckResult)) {
                DEBUG(MODULE_VCORE, "V2ray version: " + v2rayCheckResult)
                // Append assets location env.
                QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
                env.insert("V2RAY_LOCATION_ASSET", conf.v2AssetsPath);
                //
                QProcess process;
                process.setProcessEnvironment(env);
                DEBUG(MODULE_VCORE, "Starting V2ray core with test options")
                process.start(conf.v2CorePath, QStringList() << "-test" << "-config" << path, QIODevice::ReadWrite | QIODevice::Text);
                process.waitForFinished();

                if (process.exitCode() != 0) {
                    QString output = QString(process.readAllStandardOutput());
                    QvMessageBoxWarn(nullptr, tr("Configuration Error"), output.mid(output.indexOf("anti-censorship.") + 17));
                    return false;
                } else {
                    DEBUG(MODULE_VCORE, "Config file check passed.")
                    return true;
                }
            } else {
                QvMessageBoxWarn(nullptr, tr("Cannot start V2ray"),
                                 tr("V2ray core settings is incorrect.") + NEWLINE + NEWLINE +
                                 tr("The error is: ") + NEWLINE + v2rayCheckResult);
                return false;
            }
        }

        V2rayKernelInstance::V2rayKernelInstance()
        {
            vProcess = new QProcess();
            connect(vProcess, &QProcess::readyReadStandardOutput, this, [this]() {
                emit onProcessOutputReadyRead(vProcess->readAllStandardOutput().trimmed());
            });
            connect(vProcess, &QProcess::stateChanged, [this](QProcess::ProcessState state) {
                DEBUG(MODULE_VCORE, "V2ray kernel process status changed: " + QVariant::fromValue(state).toString())

                // If V2ray crashed AFTER we start it.
                if (KernelStarted && state == QProcess::NotRunning) {
                    LOG(MODULE_VCORE, "V2ray kernel crashed.")
                    emit onProcessErrored();
                }
            });
            KernelStarted = false;
        }

        bool V2rayKernelInstance::StartConnection(CONFIGROOT root)
        {
            if (KernelStarted) {
                LOG(MODULE_VCORE, "Status is invalid, expect STOPPED when calling StartConnection")
                return false;
            }

            inboundTags.clear();

            for (auto item : root["inbounds"].toArray()) {
                auto tag = item.toObject()["tag"].toString("");

                if (tag.isEmpty() || tag == API_TAG_INBOUND) {
                    // Ignore API tag and empty tags.
                    continue;
                }

                inboundTags.append(tag);
            }

            DEBUG(MODULE_VCORE, "Found inbound tags: " + inboundTags.join(";"))
            // Write the final configuration to the disk.
            QString json = JsonToString(root);
            StringToFile(&json, new QFile(QV2RAY_GENERATED_FILE_PATH));
            //
            auto filePath = QV2RAY_GENERATED_FILE_PATH;

            if (ValidateConfig(filePath)) {
                QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
                env.insert("V2RAY_LOCATION_ASSET", GetGlobalConfig().v2AssetsPath);
                vProcess->setProcessEnvironment(env);
                vProcess->start(GetGlobalConfig().v2CorePath, QStringList() << "-config" << filePath, QIODevice::ReadWrite | QIODevice::Text);
                vProcess->waitForStarted();
                DEBUG(MODULE_VCORE, "V2ray core started.")
                KernelStarted = true;
                auto conf = GetGlobalConfig();

                if (StartupOption.noAPI) {
                    LOG(MODULE_VCORE, "API has been disabled by the command line argument \"-noAPI\"")
                } else if (!conf.apiConfig.enableAPI) {
                    LOG(MODULE_VCORE, "API has been disabled by the global config option")
                } else if (inboundTags.isEmpty()) {
                    LOG(MODULE_VCORE, "API is disabled since no inbound tags configured. This is probably caused by a bad complex config.")
                } else {
                    // Config API
                    apiFailedCounter = 0;
                    this->apiPort = conf.apiConfig.statsPort;
                    auto channelAddress = "127.0.0.1:" + QString::number(apiPort);
#ifdef WITH_LIB_GRPCPP
                    Channel = grpc::CreateChannel(channelAddress.toStdString(), grpc::InsecureChannelCredentials());
                    StatsService service;
                    Stub = service.NewStub(Channel);
#else
                    auto str = Dial(const_cast<char *>(channelAddress.toStdString().c_str()), 10000);
                    LOG(MODULE_VCORE, str)
#endif
                    apiTimerId = startTimer(1000);
                    DEBUG(MODULE_VCORE, "API Worker started.")
                }

                return true;
            } else {
                KernelStarted = false;
                return false;
            }
        }

        void V2rayKernelInstance::timerEvent(QTimerEvent *event)
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

        void V2rayKernelInstance::StopConnection()
        {
            KernelStarted = false;
            vProcess->close();
            killTimer(apiTimerId);
            apiFailedCounter = 0;
            transferData.clear();
            transferSpeed.clear();
        }

        V2rayKernelInstance::~V2rayKernelInstance()
        {
            if (KernelStarted) {
                StopConnection();
            }

            delete vProcess;
        }

        long V2rayKernelInstance::CallStatsAPIByName(QString name)
        {
            if (!KernelStarted) {
                LOG(MODULE_VCORE, "Invalid connection status when calling API")
                return 0;
            }

            if (apiFailedCounter == QV2RAY_API_CALL_FAILEDCHECK_THRESHOLD) {
                LOG(MODULE_VCORE, "API call failure threshold reached, cancelling further API aclls.")
                QvMessageBoxWarn(nullptr, tr("API Call Failed"), tr("Failed to get statistics data, please check if V2ray is running properly"));
                transferData.clear();
                transferSpeed.clear();
                apiFailedCounter++;
                return 0;
            } else if (apiFailedCounter > QV2RAY_API_CALL_FAILEDCHECK_THRESHOLD) {
                return 0;
            }

#ifdef WITH_LIB_GRPCPP
            GetStatsRequest request;
            request.set_name(name.toStdString());
            request.set_reset(false);
            GetStatsResponse response;
            ClientContext context;
            Status status = Stub->GetStats(&context, request, &response);

            if (!status.ok()) {
                LOG(MODULE_VCORE, "API call returns: " + QSTRN(status.error_code()) + " (" + QString::fromStdString(status.error_message()) + ")")
                apiFailedCounter++;
            }

            auto data = response.stat().value();
#else
            auto data = GetStats(const_cast<char *>(name.toStdString().c_str()), 1000);
#endif

            if (data < 0) {
                LOG(MODULE_VCORE, "API call returns: " + QSTRN(data))
                apiFailedCounter++;
                return 0;
            }

            return data;
        }
        // ------------------------------------------------------------- API FUNCTIONS --------------------------
        long V2rayKernelInstance::getTagSpeedUp(const QString &tag)
        {
            return transferSpeed[tag + "_up"];
        }
        long V2rayKernelInstance::getTagSpeedDown(const QString &tag)
        {
            return transferSpeed[tag + "_down"];
        }
        long V2rayKernelInstance::getTagDataUp(const QString &tag)
        {
            return transferData[tag + "_up"];
        }
        long V2rayKernelInstance::getTagDataDown(const QString &tag)
        {
            return transferData[tag + "_down"];
        }
        long V2rayKernelInstance::getAllDataUp()
        {
            long val = 0;

            for (auto tag : inboundTags) {
                val += transferData[tag + "_up"];
            }

            return val;
        }
        long V2rayKernelInstance::getAllDataDown()
        {
            long val = 0;

            for (auto tag : inboundTags) {
                val += transferData[tag + "_down"];
            }

            return val;
        }
        long V2rayKernelInstance::getAllSpeedUp()
        {
            long val = 0;

            for (auto tag : inboundTags) {
                val += transferSpeed[tag + "_up"];
            }

            return val;
        }
        long V2rayKernelInstance::getAllSpeedDown()
        {
            long val = 0;

            for (auto tag : inboundTags) {
                val += transferSpeed[tag + "_down"];
            }

            return val;
        }
    }
}
