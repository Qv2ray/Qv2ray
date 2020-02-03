#include <QObject>
#include <QWidget>
#include <QDesktopServices>
#include "common/QvHelpers.hpp"
#include "KernelInteractions.hpp"
#include "core/connection/ConnectionIO.hpp"

namespace Qv2ray::core::kernel
{
    bool V2rayKernelInstance::ValidateKernel(const QString &vCorePath, const QString &vAssetsPath, QString *message)
    {
        QFile coreFile(vCorePath);

        if (!coreFile.exists()) {
            DEBUG(VCORE, "V2ray core file cannot be found.")
            *message = tr("V2ray core executable not found.");
            return false;
        }

        // Use open() here to prevent `executing` a folder, which may have the same name as the V2ray core.
        if (!coreFile.open(QFile::ReadOnly)) {
            DEBUG(VCORE, "V2ray core file cannot be opened, possibly be a folder?")
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
            DEBUG(VCORE, "V2ray assets path contains none of those two files.")
            *message = tr("V2ray assets path is not valid.");
            return false;
        }

        if (!hasGeoIP) {
            DEBUG(VCORE, "No geoip.dat in assets path, aborting.")
            *message =  tr("No geoip.dat in assets path.");
            return false;
        }

        if (!hasGeoSite) {
            DEBUG(VCORE, "No geosite.dat in assets path, aborting.")
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
            DEBUG(VCORE, "VCore failed with an exit code: " + QSTRN(exitCode))
            *message = tr("V2ray core failed with an exit code: ") + QSTRN(exitCode);
            return false;
        }

        QString output = proc.readAllStandardOutput();
        LOG(VCORE, "V2ray output: " + SplitLines(output).join(";"))

        if (SplitLines(output).isEmpty()) {
            *message = tr("V2ray core returns empty string.");
            return false;
        }

        *message = SplitLines(output).first();
        return true;
    }


    bool V2rayKernelInstance::ValidateConfig(const QString &path)
    {
        QString v2rayCheckResult;

        if (ValidateKernel(GlobalConfig.v2CorePath, GlobalConfig.v2AssetsPath, &v2rayCheckResult)) {
            DEBUG(VCORE, "V2ray version: " + v2rayCheckResult)
            // Append assets location env.
            QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
            env.insert("V2RAY_LOCATION_ASSET", GlobalConfig.v2AssetsPath);
            //
            QProcess process;
            process.setProcessEnvironment(env);
            DEBUG(VCORE, "Starting V2ray core with test options")
            process.start(GlobalConfig.v2CorePath, QStringList() << "-test" << "-config" << path, QIODevice::ReadWrite | QIODevice::Text);
            process.waitForFinished();

            if (process.exitCode() != 0) {
                QString output = QString(process.readAllStandardOutput());
                QvMessageBoxWarn(nullptr, tr("Configuration Error"), output.mid(output.indexOf("anti-censorship.") + 17));
                return false;
            } else {
                DEBUG(VCORE, "Config file check passed.")
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
            DEBUG(VCORE, "V2ray kernel process status changed: " + QVariant::fromValue(state).toString())

            // If V2ray crashed AFTER we start it.
            if (KernelStarted && state == QProcess::NotRunning) {
                LOG(VCORE, "V2ray kernel crashed.")
                StopConnection();
                emit onProcessErrored();
            }
        });
        apiWorker = new APIWorkder();
        connect(apiWorker, &APIWorkder::OnDataReady, this, &V2rayKernelInstance::onAPIDataReady);
        KernelStarted = false;
    }

    bool V2rayKernelInstance::StartConnection(CONFIGROOT root)
    {
        if (KernelStarted) {
            LOG(VCORE, "Status is invalid, expect STOPPED when calling StartConnection")
            return false;
        }

        // Write the final configuration to the disk.
        QString json = JsonToString(root);
        StringToFile(&json, new QFile(QV2RAY_GENERATED_FILE_PATH));
        //
        auto filePath = QV2RAY_GENERATED_FILE_PATH;

        if (ValidateConfig(filePath)) {
            QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
            env.insert("V2RAY_LOCATION_ASSET", GlobalConfig.v2AssetsPath);
            vProcess->setProcessEnvironment(env);
            vProcess->start(GlobalConfig.v2CorePath, QStringList() << "-config" << filePath, QIODevice::ReadWrite | QIODevice::Text);
            vProcess->waitForStarted();
            DEBUG(VCORE, "V2ray core started.")
            KernelStarted = true;
            QStringList inboundTags;

            for (auto item : root["inbounds"].toArray()) {
                auto tag = item.toObject()["tag"].toString("");

                if (tag.isEmpty() || tag == API_TAG_INBOUND) {
                    // Ignore API tag and empty tags.
                    continue;
                }

                inboundTags.append(tag);
            }

            DEBUG(VCORE, "Found inbound tags: " + inboundTags.join(";"))
            apiEnabled = false;

            //
            if (StartupOption.noAPI) {
                LOG(VCORE, "API has been disabled by the command line argument \"-noAPI\"")
            } else if (!GlobalConfig.apiConfig.enableAPI) {
                LOG(VCORE, "API has been disabled by the global config option")
            } else if (inboundTags.isEmpty()) {
                LOG(VCORE, "API is disabled since no inbound tags configured. This is probably caused by a bad complex config.")
            } else {
                apiWorker->StartAPI(inboundTags);
                apiEnabled = true;
                DEBUG(VCORE, "Qv2ray API started")
            }

            return true;
        } else {
            KernelStarted = false;
            return false;
        }
    }

    void V2rayKernelInstance::StopConnection()
    {
        if (apiEnabled) {
            apiWorker->StopAPI();
            apiEnabled = false;
        }

        // Set this to false BEFORE close the Process, since we need this flag to capture the real kernel CRASH
        KernelStarted = false;
        vProcess->close();
        // Block until V2ray core exits
        // Should we use -1 instead of waiting for 30secs?
        vProcess->waitForFinished();
        //
        transferDataUp.clear();
        transferDataDown.clear();
        transferSpeedUp.clear();
        transferSpeedDown.clear();
    }

    V2rayKernelInstance::~V2rayKernelInstance()
    {
        if (KernelStarted) {
            StopConnection();
        }

        delete apiWorker;
        delete vProcess;
    }

    void V2rayKernelInstance::onAPIDataReady(QString tag, long totalUp, long totalDown)
    {
        auto dataup = totalUp - transferDataUp[tag];
        transferDataUp[tag] = totalUp;
        transferSpeedUp[tag] = dataup;
        // Download
        auto datadown = totalDown - transferDataDown[tag];
        transferDataDown[tag] = totalDown;
        transferSpeedDown[tag] = datadown;
    }

    // ------------------------------------------------------------- API FUNCTIONS --------------------------
    long V2rayKernelInstance::getTagSpeedUp(const QString &tag)
    {
        return transferSpeedUp[tag];
    }
    long V2rayKernelInstance::getTagSpeedDown(const QString &tag)
    {
        return transferSpeedDown[tag];
    }
    long V2rayKernelInstance::getTagDataUp(const QString &tag)
    {
        return transferDataUp[tag];
    }
    long V2rayKernelInstance::getTagDataDown(const QString &tag)
    {
        return transferDataDown[tag];
    }
    long V2rayKernelInstance::getAllDataUp()
    {
        long val = 0;

        for (auto _val : transferDataUp.values()) {
            val += _val;
        }

        return val;
    }
    long V2rayKernelInstance::getAllDataDown()
    {
        long val = 0;

        for (auto _val : transferDataDown.values()) {
            val += _val;
        }

        return val;
    }
    long V2rayKernelInstance::getAllSpeedUp()
    {
        long val = 0;

        for (auto _val : transferSpeedUp.values()) {
            val += _val;
        }

        return val;
    }
    long V2rayKernelInstance::getAllSpeedDown()
    {
        long val = 0;

        for (auto _val : transferSpeedDown.values()) {
            val += _val;
        }

        return val;
    }
}
