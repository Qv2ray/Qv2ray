#include "V2RayKernelInteractions.hpp"

#include "APIBackend.hpp"
#include "core/connection/ConnectionIO.hpp"
#include "utils/QvHelpers.hpp"

#include <QProcess>

#define QV2RAY_GENERATED_FILE_PATH (QV2RAY_GENERATED_DIR + "config.gen.json")
#define QV_MODULE_NAME "V2RayInteraction"

#ifdef QV2RAY_USE_V5_CORE
#define V2RAY_CORE_VERSION_ARGV "version"
#define V2RAY_CORE_CONFIG_ARGV "run", "-config"
#else
#define V2RAY_CORE_VERSION_ARGV "--version"
#define V2RAY_CORE_CONFIG_ARGV "--config"
#endif

namespace Qv2ray::core::kernel
{
#if QV2RAY_FEATURE(kernel_check_permission)
    std::pair<bool, std::optional<QString>> V2RayKernelInstance::CheckAndSetCoreExecutableState(const QString &vCorePath)
    {
#ifdef Q_OS_UNIX
        // For Linux/macOS users: if they cannot execute the core,
        // then we shall grant the permission to execute it.
        QFile coreFile(vCorePath);
        if (!coreFile.permissions().testFlag(QFileDevice::ExeUser))
        {
#if QV2RAY_FEATURE(kernel_set_permission)
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
#endif
            LOG("Core file not executable.");
            return { false, tr("Core file not executable.") };
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
#endif

    std::pair<bool, std::optional<QString>> V2RayKernelInstance::ValidateKernel(const QString &corePath, const QString &assetsPath)
    {
        QFile coreFile(corePath);

        if (!coreFile.exists())
            return { false, tr("V2Ray core executable not found.") };

        // Use open() here to prevent `executing` a folder, which may have the
        // same name as the V2Ray core.
        if (!coreFile.open(QFile::ReadOnly))
            return { false, tr("V2Ray core file cannot be opened, please ensure there's a file instead of a folder.") };

        coreFile.close();

#if QV2RAY_FEATURE(kernel_check_abi)
        // Get Core ABI.
        const auto [abi, err] = kernel::abi::deduceKernelABI(corePath);
        if (err)
        {
            LOG("Core ABI deduction failed: " + *err);
            return { false, *err };
        }
        LOG("Core ABI: " + kernel::abi::abiToString(*abi));

        // Get Compiled ABI
        auto compiledABI = kernel::abi::COMPILED_ABI_TYPE;
        LOG("Host ABI: " + kernel::abi::abiToString(compiledABI));

        // Check ABI Compatibility.
        switch (kernel::abi::checkCompatibility(compiledABI, *abi))
        {
            case kernel::abi::ABI_NOPE:
            {
                LOG("Host is incompatible with core");
                const auto msg = tr("V2Ray core is incompatible with your platform.\r\n"
                                    "Expected core ABI is %1, but got actual %2.\r\n"
                                    "Maybe you have downloaded the wrong core?")
                                     .arg(kernel::abi::abiToString(compiledABI), kernel::abi::abiToString(*abi));
                return { false, msg };
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
#endif

#if QV2RAY_FEATURE(kernel_check_permission)
        // Check executable permissions.
        const auto [isExecutableOk, strExecutableErr] = CheckAndSetCoreExecutableState(corePath);
        if (!isExecutableOk)
            return { false, strExecutableErr.value_or("") };
#endif
        //
        // Check file existance.
        // From: https://www.v2fly.org/chapter_02/env.html#asset-location
        bool hasGeoIP = FileExistsIn(QDir(assetsPath), "geoip.dat");
        bool hasGeoSite = FileExistsIn(QDir(assetsPath), "geosite.dat");

        if (!hasGeoIP && !hasGeoSite)
            return { false, tr("V2Ray assets path is not valid.") };

        if (!hasGeoIP)
            return { false, tr("No geoip.dat in assets path.") };

        if (!hasGeoSite)
            return { false, tr("No geosite.dat in assets path.") };

        // Check if V2Ray core returns a version number correctly.
        QProcess proc;
#ifdef Q_OS_WIN32
        // nativeArguments are required for Windows platform, without a
        // reason...
        proc.setProcessChannelMode(QProcess::MergedChannels);
        proc.setProgram(corePath);
        proc.setNativeArguments(V2RAY_CORE_VERSION_ARGV);
        proc.start();
#else
        proc.start(corePath, { V2RAY_CORE_VERSION_ARGV });
#endif
        proc.waitForStarted();
        proc.waitForFinished();
        auto exitCode = proc.exitCode();

        if (exitCode != 0)
            return { false, tr("V2Ray core failed with an exit code: ") + QSTRN(exitCode) };

        const auto output = proc.readAllStandardOutput();
        LOG("V2Ray output: " + SplitLines(output).join(";"));

        if (SplitLines(output).isEmpty())
            return { false, tr("V2Ray core returns empty string.") };

        return { true, SplitLines(output).at(0) };
    }

    std::optional<QString> V2RayKernelInstance::ValidateConfig(const QString &path)
    {
        const auto kernelPath = GlobalConfig.kernelConfig.KernelPath();
        const auto assetsPath = GlobalConfig.kernelConfig.AssetsPath();
        if (const auto &[result, msg] = ValidateKernel(kernelPath, assetsPath); result)
        {
            DEBUG("V2Ray version: " + *msg);
            // Append assets location env.
            auto env = QProcessEnvironment::systemEnvironment();
            env.insert("v2ray.location.asset", assetsPath);
            env.insert("XRAY_LOCATION_ASSET", assetsPath);
            //
            QProcess process;
            process.setProcessEnvironment(env);
            DEBUG("Starting V2Ray core with test options");
#ifdef QV2RAY_USE_V5_CORE
            process.start(kernelPath, { "test", "-c", path }, QIODevice::ReadWrite | QIODevice::Text);
#else
            process.start(kernelPath, { "-test", "-config", path }, QIODevice::ReadWrite | QIODevice::Text);
#endif
            process.waitForFinished();

            if (process.exitCode() != 0)
            {
                QString output = QString(process.readAllStandardOutput());
                QvMessageBoxWarn(nullptr, tr("Configuration Error"), output.mid(output.indexOf("anti-censorship.") + 17));
                return std::nullopt;
            }

            DEBUG("Config file check passed.");
            return std::nullopt;
        }
        else
        {
            return msg;
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
            if (kernelStarted && state == QProcess::NotRunning)
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
        kernelStarted = false;
    }

    std::optional<QString> V2RayKernelInstance::StartConnection(const CONFIGROOT &root)
    {
        if (kernelStarted)
        {
            LOG("Status is invalid, expect STOPPED when calling StartConnection");
            return tr("Invalid V2Ray Instance Status.");
        }

        const auto json = JsonToString(root);
        StringToFile(json, QV2RAY_GENERATED_FILE_PATH);
        //
        auto filePath = QV2RAY_GENERATED_FILE_PATH;

        if (const auto &result = ValidateConfig(filePath); result)
        {
            kernelStarted = false;
            return tr("V2Ray kernel failed to start: ") + *result;
        }
        auto env = QProcessEnvironment::systemEnvironment();
        env.insert("v2ray.location.asset", GlobalConfig.kernelConfig.AssetsPath());
        env.insert("XRAY_LOCATION_ASSET", GlobalConfig.kernelConfig.AssetsPath());
        vProcess->setProcessEnvironment(env);
        vProcess->start(GlobalConfig.kernelConfig.KernelPath(), { V2RAY_CORE_CONFIG_ARGV, filePath }, QIODevice::ReadWrite | QIODevice::Text);
        vProcess->waitForStarted();
        kernelStarted = true;

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

    void V2RayKernelInstance::StopConnection()
    {
        if (apiEnabled)
        {
            apiWorker->StopAPI();
            apiEnabled = false;
        }

        // Set this to false BEFORE close the Process, since we need this flag
        // to capture the real kernel CRASH
        kernelStarted = false;
        vProcess->close();
        // Block until V2Ray core exits
        // Should we use -1 instead of waiting for 30secs?
        vProcess->waitForFinished();
    }

    V2RayKernelInstance::~V2RayKernelInstance()
    {
        if (kernelStarted)
        {
            StopConnection();
        }

        delete apiWorker;
        delete vProcess;
    }

} // namespace Qv2ray::core::kernel
