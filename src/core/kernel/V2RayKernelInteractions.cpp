#include "V2RayKernelInteractions.hpp"

#include "APIBackend.hpp"
#include "core/connection/ConnectionIO.hpp"
#include "utils/QvHelpers.hpp"

#include <QProcess>

#define QV2RAY_GENERATED_FILE_PATH (QV2RAY_GENERATED_DIR + "config.gen.json")
#define QV_MODULE_NAME "V2RayInteraction"

namespace Qv2ray::core::kernel
{
    std::pair<bool, std::optional<QString>> V2RayKernelInstance::CheckAndSetCoreExecutableState(const QString &vCorePath)
    {
#ifdef Q_OS_UNIX
        // For Linux/macOS users: if they cannot execute the core,
        // then we shall grant the permission to execute it.
        QFile coreFile(vCorePath);
        if (!coreFile.permissions().testFlag(QFileDevice::ExeUser))
        {
            DEBUG("Core file not executable. Trying to enable.");
            const auto result = coreFile.setPermissions(coreFile.permissions().setFlag(QFileDevice::ExeUser));
            if (!result)
            {
                DEBUG("Failed to enable executable permission.");
                const auto message = tr("Core file is lacking executable permission for the current user.") +
                                     tr("Qv2ray tried to set, but failed because permission denied.");
                return { false, message };
            }
            else
            {
                DEBUG("Core executable permission set.");
            }
        }
        else
        {
            DEBUG("Core file is executable.");
        }

        return { true, std::nullopt };
#else
        // For Windows and other users: just skip this check.
        DEBUG("Skipped check and set core executable state.");
        return { true, tr("Check is skipped") };
#endif
    }

    bool V2RayKernelInstance::ValidateKernel(const QString &vCorePath, const QString &vAssetsPath, QString *message)
    {
        QFile coreFile(vCorePath);

        if (!coreFile.exists())
        {
            DEBUG("V2Ray core file cannot be found.");
            *message = tr("V2Ray core executable not found.");
            return false;
        }

        // Use open() here to prevent `executing` a folder, which may have the
        // same name as the V2Ray core.
        if (!coreFile.open(QFile::ReadOnly))
        {
            DEBUG("V2Ray core file cannot be opened, possibly be a folder?");
            *message = tr("V2Ray core file cannot be opened, please ensure there's a file instead of a folder.");
            return false;
        }

        coreFile.close();

        // Get Core ABI.
        const auto [abi, err] = kernel::abi::deduceKernelABI(vCorePath);
        if (err)
        {
            LOG("Core ABI deduction failed: " + ACCESS_OPTIONAL_VALUE(err));
            *message = ACCESS_OPTIONAL_VALUE(err);
            return false;
        }
        LOG("Core ABI: " + kernel::abi::abiToString(ACCESS_OPTIONAL_VALUE(abi)));

        // Get Compiled ABI
        auto compiledABI = kernel::abi::COMPILED_ABI_TYPE;
        LOG("Host ABI: " + kernel::abi::abiToString(compiledABI));

        // Check ABI Compatibility.
        switch (kernel::abi::checkCompatibility(compiledABI, ACCESS_OPTIONAL_VALUE(abi)))
        {
            case kernel::abi::ABI_NOPE:
            {
                LOG("Host is incompatible with core");
                *message = tr("V2Ray core is incompatible with your platform.\r\n" //
                              "Expected core ABI is %1, but got actual %2.\r\n"    //
                              "Maybe you have downloaded the wrong core?")
                               .arg(kernel::abi::abiToString(compiledABI), kernel::abi::abiToString(ACCESS_OPTIONAL_VALUE(abi)));
                return false;
            }
            case kernel::abi::ABI_MAYBE:
            {
                LOG("WARNING: Host maybe incompatible with core");
                break;
            }
            case kernel::abi::ABI_PERFECT:
            {
                LOG("Host is compatible with core");
                break;
            }
        }

        // Check executable permissions.
        const auto [isExecutableOk, strExecutableErr] = CheckAndSetCoreExecutableState(vCorePath);
        if (!isExecutableOk)
        {
            *message = strExecutableErr.value_or("");
            return false;
        }

        //
        // Check file existance.
        // From: https://www.v2fly.org/chapter_02/env.html#asset-location
        //
        bool hasGeoIP = FileExistsIn(QDir(vAssetsPath), "geoip.dat");
        bool hasGeoSite = FileExistsIn(QDir(vAssetsPath), "geosite.dat");

        if (!hasGeoIP && !hasGeoSite)
        {
            DEBUG("V2Ray assets path contains none of those two files.");
            *message = tr("V2Ray assets path is not valid.");
            return false;
        }

        if (!hasGeoIP)
        {
            DEBUG("No geoip.dat in assets path, aborting.");
            *message = tr("No geoip.dat in assets path.");
            return false;
        }

        if (!hasGeoSite)
        {
            DEBUG("No geosite.dat in assets path, aborting.");
            *message = tr("No geosite.dat in assets path.");
            return false;
        }

        // Check if V2Ray core returns a version number correctly.
        QProcess proc;
#ifdef Q_OS_WIN32
        // nativeArguments are required for Windows platform, without a
        // reason...
        proc.setProcessChannelMode(QProcess::MergedChannels);
        proc.setProgram(vCorePath);
        proc.setNativeArguments("--version");
        proc.start();
#else
        proc.start(vCorePath, { "--version" });
#endif
        proc.waitForStarted();
        proc.waitForFinished();
        auto exitCode = proc.exitCode();

        if (exitCode != 0)
        {
            DEBUG("VCore failed with an exit code: " + QSTRN(exitCode));
            *message = tr("V2Ray core failed with an exit code: ") + QSTRN(exitCode);
            return false;
        }

        const auto output = proc.readAllStandardOutput();
        LOG("V2Ray output: " + SplitLines(output).join(";"));

        if (SplitLines(output).isEmpty())
        {
            *message = tr("V2Ray core returns empty string.");
            return false;
        }

        *message = SplitLines(output).first();
        return true;
    }

    bool V2RayKernelInstance::ValidateConfig(const QString &path)
    {
        QString V2RayCheckResult;
        auto kernelPath = GlobalConfig.kernelConfig.KernelPath();
        auto assetsPath = GlobalConfig.kernelConfig.AssetsPath();
        if (ValidateKernel(kernelPath, assetsPath, &V2RayCheckResult))
        {
            DEBUG("V2Ray version: " + V2RayCheckResult);
            // Append assets location env.
            QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
            env.insert("V2RAY_LOCATION_ASSET", assetsPath);
            env.insert("XRAY_LOCATION_ASSET", assetsPath);
            //
            QProcess process;
            process.setProcessEnvironment(env);
            DEBUG("Starting V2Ray core with test options");
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
                DEBUG("Config file check passed.");
                return true;
            }
        }
        else
        {
            QvMessageBoxWarn(nullptr, tr("Cannot start V2Ray"),                            //
                             tr("V2Ray core settings is incorrect.") + NEWLINE + NEWLINE + //
                                 tr("The error is: ") + NEWLINE + V2RayCheckResult);
            return false;
        }
    }

    V2RayKernelInstance::V2RayKernelInstance(QObject *parent) : QObject(parent)
    {
        vProcess = new QProcess();
        connect(vProcess, &QProcess::readyReadStandardOutput, this,
                [&]() { emit OnProcessOutputReadyRead(vProcess->readAllStandardOutput().trimmed()); });
        connect(vProcess, &QProcess::stateChanged, [&](QProcess::ProcessState state) {
            DEBUG("V2Ray kernel process status changed: " + QVariant::fromValue(state).toString());

            // If V2Ray crashed AFTER we start it.
            if (KernelStarted && state == QProcess::NotRunning)
            {
                LOG("V2Ray kernel crashed.");
                StopConnection();
                emit OnProcessErrored("V2Ray kernel crashed.");
            }
        });
        apiWorker = new APIWorker();
        qRegisterMetaType<StatisticsType>();
        qRegisterMetaType<QMap<StatisticsType, QvStatsSpeed>>();
        connect(apiWorker, &APIWorker::onAPIDataReady, this, &V2RayKernelInstance::OnNewStatsDataArrived);
        KernelStarted = false;
    }

    std::optional<QString> V2RayKernelInstance::StartConnection(const CONFIGROOT &root)
    {
        if (KernelStarted)
        {
            LOG("Status is invalid, expect STOPPED when calling StartConnection");
            return tr("Invalid V2Ray Instance Status.");
        }

        // Write the final configuration to the disk.
        QString json = JsonToString(root);
        StringToFile(json, QV2RAY_GENERATED_FILE_PATH);
        //
        auto filePath = QV2RAY_GENERATED_FILE_PATH;

        if (ValidateConfig(filePath))
        {
            auto env = QProcessEnvironment::systemEnvironment();
            env.insert("V2RAY_LOCATION_ASSET", GlobalConfig.kernelConfig.AssetsPath());
            env.insert("XRAY_LOCATION_ASSET", GlobalConfig.kernelConfig.AssetsPath());
            vProcess->setProcessEnvironment(env);
            vProcess->start(GlobalConfig.kernelConfig.KernelPath(), { "-config", filePath }, QIODevice::ReadWrite | QIODevice::Text);
            vProcess->waitForStarted();
            KernelStarted = true;

            QMap<bool, QMap<QString, QString>> tagProtocolMap;
            for (const auto isOutbound : { GlobalConfig.uiConfig.graphConfig.useOutboundStats, false })
            {
                for (const auto &item : root[isOutbound ? "outbounds" : "inbounds"].toArray())
                {
                    const auto tag = item.toObject()["tag"].toString("");
                    if (tag == API_TAG_INBOUND)
                        continue;
                    if (tag.isEmpty())
                    {
                        LOG("Ignored inbound with empty tag.");
                        continue;
                    }
                    tagProtocolMap[isOutbound][tag] = item.toObject()["protocol"].toString();
                }
            }

            apiEnabled = false;
            if (QvCoreApplication->StartupArguments.noAPI)
            {
                LOG("API has been disabled by the command line arguments");
            }
            else if (!GlobalConfig.kernelConfig.enableAPI)
            {
                LOG("API has been disabled by the global config option");
            }
            else if (tagProtocolMap.isEmpty())
            {
                LOG("RARE: API is disabled since no inbound tags configured. This is usually caused by a bad complex config.");
            }
            else
            {
                DEBUG("Starting API");
                apiWorker->StartAPI(tagProtocolMap);
                apiEnabled = true;
            }

            return std::nullopt;
        }
        else
        {
            KernelStarted = false;
            return tr("V2Ray kernel failed to start.");
        }
    }

    void V2RayKernelInstance::StopConnection()
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
        // Block until V2Ray core exits
        // Should we use -1 instead of waiting for 30secs?
        vProcess->waitForFinished();
    }

    V2RayKernelInstance::~V2RayKernelInstance()
    {
        if (KernelStarted)
        {
            StopConnection();
        }

        delete apiWorker;
        delete vProcess;
    }

} // namespace Qv2ray::core::kernel
