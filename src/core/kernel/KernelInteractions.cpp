#include "KernelInteractions.hpp"

#include "APIBackend.hpp"
#include "common/QvHelpers.hpp"
#include "core/connection/ConnectionIO.hpp"

#include <QDesktopServices>
#include <QObject>
#include <QWidget>

namespace Qv2ray::core::kernel
{
    bool V2rayKernelInstance::ValidateKernel(const QString &vCorePath, const QString &vAssetsPath, QString *message)
    {
        QFile coreFile(vCorePath);

        if (!coreFile.exists())
        {
            DEBUG(MODULE_VCORE, "V2ray core file cannot be found.")
            *message = tr("V2ray core executable not found.");
            return false;
        }

        // Use open() here to prevent `executing` a folder, which may have the
        // same name as the V2ray core.
        if (!coreFile.open(QFile::ReadOnly))
        {
            DEBUG(MODULE_VCORE, "V2ray core file cannot be opened, possibly be a folder?")
            *message = tr("V2ray core file cannot be opened, please ensure there's a file instead of a folder.");
            return false;
        }

        coreFile.close();

        // Get Core ABI.
        auto [abi, err] = kernel::abi::deduceKernelABI(vCorePath);
        if (err)
        {
            LOG(MODULE_VCORE, "Core ABI deduction failed: " + err.value())
            *message = err.value();
            return false;
        }
        LOG(MODULE_VCORE, "Core ABI: " + kernel::abi::abiToString(abi.value()))

        // Get Compiled ABI
        auto compiledABI = kernel::abi::COMPILED_ABI_TYPE;
        LOG(MODULE_VCORE, "Host ABI: " + kernel::abi::abiToString(compiledABI))

        // Check ABI Compatibility.
        switch (kernel::abi::checkCompatibility(compiledABI, abi.value()))
        {
            case kernel::abi::ABI_NOPE:
                LOG(MODULE_VCORE, "Host is incompatible with core")
                *message = tr("V2Ray core is incompatible with your platform.\r\n" //
                              "Expected core ABI is %1, but got actual %2.\r\n"    //
                              "Maybe you have downloaded the wrong core?")
                               .arg(kernel::abi::abiToString(compiledABI), kernel::abi::abiToString(abi.value()));
                return false;
            case kernel::abi::ABI_MAYBE: LOG(MODULE_VCORE, "WARNING: Host maybe incompatible with core"); [[fallthrough]];
            case kernel::abi::ABI_PERFECT: LOG(MODULE_VCORE, "Host is compatible with core");
        }

        //
        // Check file existance.
        // From: https://www.v2fly.org/chapter_02/env.html#asset-location
        //
        bool hasGeoIP = FileExistsIn(QDir(vAssetsPath), "geoip.dat");
        bool hasGeoSite = FileExistsIn(QDir(vAssetsPath), "geosite.dat");

        if (!hasGeoIP && !hasGeoSite)
        {
            DEBUG(MODULE_VCORE, "V2ray assets path contains none of those two files.")
            *message = tr("V2ray assets path is not valid.");
            return false;
        }

        if (!hasGeoIP)
        {
            DEBUG(MODULE_VCORE, "No geoip.dat in assets path, aborting.")
            *message = tr("No geoip.dat in assets path.");
            return false;
        }

        if (!hasGeoSite)
        {
            DEBUG(MODULE_VCORE, "No geosite.dat in assets path, aborting.")
            *message = tr("No geosite.dat in assets path.");
            return false;
        }

        // Check if V2ray core returns a version number correctly.
        QProcess proc;
#ifdef Q_OS_WIN32
        // nativeArguments are required for Windows platform, without a
        // reason...
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

        if (exitCode != 0)
        {
            DEBUG(MODULE_VCORE, "VCore failed with an exit code: " + QSTRN(exitCode))
            *message = tr("V2ray core failed with an exit code: ") + QSTRN(exitCode);
            return false;
        }

        QString output = proc.readAllStandardOutput();
        LOG(MODULE_VCORE, "V2ray output: " + SplitLines(output).join(";"))

        if (SplitLines(output).isEmpty())
        {
            *message = tr("V2ray core returns empty string.");
            return false;
        }

        *message = SplitLines(output).first();
        return true;
    }

    bool V2rayKernelInstance::ValidateConfig(const QString &path)
    {
        QString v2rayCheckResult;
        auto kernelPath = GlobalConfig.kernelConfig.KernelPath();
        auto assetsPath = GlobalConfig.kernelConfig.AssetsPath();
        if (ValidateKernel(kernelPath, assetsPath, &v2rayCheckResult))
        {
            DEBUG(MODULE_VCORE, "V2ray version: " + v2rayCheckResult)
            // Append assets location env.
            QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
            env.insert("V2RAY_LOCATION_ASSET", assetsPath);
            //
            QProcess process;
            process.setProcessEnvironment(env);
            DEBUG(MODULE_VCORE, "Starting V2ray core with test options")
            process.start(kernelPath, QStringList{ "-test", "-config", path }, QIODevice::ReadWrite | QIODevice::Text);
            process.waitForFinished();

            if (process.exitCode() != 0)
            {
                QString output = QString(process.readAllStandardOutput());
                QvMessageBoxWarn(nullptr, tr("Configuration Error"), output.mid(output.indexOf("anti-censorship.") + 17));
                return false;
            }
            else
            {
                DEBUG(MODULE_VCORE, "Config file check passed.")
                return true;
            }
        }
        else
        {
            QvMessageBoxWarn(nullptr, tr("Cannot start V2ray"),
                             tr("V2ray core settings is incorrect.") + NEWLINE + NEWLINE + tr("The error is: ") + NEWLINE + v2rayCheckResult);
            return false;
        }
    }

    V2rayKernelInstance::V2rayKernelInstance()
    {
        vProcess = new QProcess();
        connect(vProcess, &QProcess::readyReadStandardOutput, this,
                [&]() { emit OnProcessOutputReadyRead(id, vProcess->readAllStandardOutput().trimmed()); });
        connect(vProcess, &QProcess::stateChanged, [&](QProcess::ProcessState state) {
            DEBUG(MODULE_VCORE, "V2ray kernel process status changed: " + QVariant::fromValue(state).toString())

            // If V2ray crashed AFTER we start it.
            if (KernelStarted && state == QProcess::NotRunning)
            {
                LOG(MODULE_VCORE, "V2ray kernel crashed.")
                StopConnection();
                emit OnProcessErrored(id);
            }
        });
        apiWorker = new APIWorker();
        connect(apiWorker, &APIWorker::OnDataReady, this, &V2rayKernelInstance::onAPIDataReady);
        KernelStarted = false;
    }

    optional<QString> V2rayKernelInstance::StartConnection(const ConnectionId &id, const CONFIGROOT &root)
    {
        if (KernelStarted)
        {
            LOG(MODULE_VCORE, "Status is invalid, expect STOPPED when calling StartConnection")
            return tr("Invalid V2ray Instance Status.");
        }

        // Write the final configuration to the disk.
        QString json = JsonToString(root);
        StringToFile(json, QV2RAY_GENERATED_FILE_PATH);
        //
        auto filePath = QV2RAY_GENERATED_FILE_PATH;

        if (ValidateConfig(filePath))
        {
            QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
            env.insert("V2RAY_LOCATION_ASSET", GlobalConfig.kernelConfig.AssetsPath());
            vProcess->setProcessEnvironment(env);
            vProcess->start(GlobalConfig.kernelConfig.KernelPath(), QStringList{ "-config", filePath }, QIODevice::ReadWrite | QIODevice::Text);
            vProcess->waitForStarted();
            DEBUG(MODULE_VCORE, "V2ray core started.")
            KernelStarted = true;
            // Set Connection ID
            this->id = id;
            QStringList inboundTags;

            for (auto item : root["inbounds"].toArray())
            {
                auto tag = item.toObject()["tag"].toString("");

                if (tag.isEmpty() || tag == API_TAG_INBOUND)
                {
                    // Ignore API tag and empty tags.
                    continue;
                }

                inboundTags.append(tag);
            }

            DEBUG(MODULE_VCORE, "Found inbound tags: " + inboundTags.join(";"))
            apiEnabled = false;

            //
            if (StartupOption.noAPI)
            {
                LOG(MODULE_VCORE, "API has been disabled by the command line argument \"-noAPI\"")
            }
            else if (!GlobalConfig.apiConfig.enableAPI)
            {
                LOG(MODULE_VCORE, "API has been disabled by the global config option")
            }
            else if (inboundTags.isEmpty())
            {
                LOG(MODULE_VCORE, "API is disabled since no inbound tags configured. This is probably caused by a bad complex config.")
            }
            else
            {
                apiWorker->StartAPI(inboundTags);
                apiEnabled = true;
                DEBUG(MODULE_VCORE, "Qv2ray API started")
            }

            return {};
        }
        else
        {
            KernelStarted = false;
            return tr("V2ray kernel failed to start.");
        }
    }

    void V2rayKernelInstance::StopConnection()
    {
        if (apiEnabled)
        {
            apiWorker->StopAPI();
            apiEnabled = false;
        }

        // Set this to false BEFORE close the Process, since we need this flag
        // to capture the real kernel CRASH
        KernelStarted = false;
        vProcess->close();
        // Block until V2ray core exits
        // Should we use -1 instead of waiting for 30secs?
        vProcess->waitForFinished();
    }

    V2rayKernelInstance::~V2rayKernelInstance()
    {
        if (KernelStarted)
        {
            StopConnection();
        }

        delete apiWorker;
        delete vProcess;
    }

    void V2rayKernelInstance::onAPIDataReady(const quint64 speedUp, const quint64 speedDown)
    {
        emit OnNewStatsDataArrived(id, speedUp, speedDown);
    }
} // namespace Qv2ray::core::kernel
