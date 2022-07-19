#include "Kernel.hpp"

#include "ProfileGenerator.hpp"
#include "V2RayCorePluginTemplate.hpp"
#include "common/CommonHelpers.hpp"
#include "core/V2RayAPIStats.hpp"

#include <QJsonDocument>
#include <QProcess>

constexpr auto GENERATED_V2RAY_CONFIGURATION_NAME = "config.toml";
constexpr auto V2RAYPLUGIN_NO_API_ENV = "V2RAYPLUGIN_NO_API";

using namespace V2RayPluginNamespace;

V2RayRustKernel::V2RayRustKernel()
{
    vProcess = new QProcess();
    connect(vProcess, &QProcess::readyReadStandardOutput, this, [&]() { emit OnLog(QString::fromUtf8(vProcess->readAllStandardOutput().trimmed())); });
    connect(vProcess, &QProcess::stateChanged,
            [this](QProcess::ProcessState state)
            {
                if (kernelStarted && state == QProcess::NotRunning)
                    emit OnCrashed(u"v2ray-rust kernel crashed."_qs);
            });
    apiWorker = new APIWorker();
    qRegisterMetaType<StatisticsObject::StatisticsType>();
    qRegisterMetaType<QMap<StatisticsObject::StatisticsType, long>>();
    connect(apiWorker, &APIWorker::OnAPIDataReady, this, &V2RayRustKernel::OnStatsAvailable);
    kernelStarted = false;
}

V2RayRustKernel::~V2RayRustKernel()
{
    delete apiWorker;
    delete vProcess;
}

void V2RayRustKernel::SetProfileContent(const ProfileContent &content)
{
    profile = content;
}

bool V2RayRustKernel::PrepareConfigurations()
{
    const auto config = V2RayRustProfileGenerator::GenerateConfiguration(profile, tagProtocolMap);
    configFilePath = V2RayCorePluginClass::PluginInstance->WorkingDirectory().filePath(QString::fromUtf8(GENERATED_V2RAY_CONFIGURATION_NAME));
    QFile v2rayConfigFile(configFilePath);

    v2rayConfigFile.open(QIODevice::ReadWrite | QIODevice::Truncate);
    v2rayConfigFile.write(config);
    v2rayConfigFile.close();

    if (const auto &result = ValidateConfig(configFilePath); result)
    {
        kernelStarted = false;
        return false;
    }

    return true;
}

void V2RayRustKernel::Start()
{
    Q_ASSERT_X(!kernelStarted, Q_FUNC_INFO, "Kernel state mismatch.");

    const auto settings = V2RayCorePluginClass::PluginInstance->settings;

    auto env = QProcessEnvironment::systemEnvironment();
    env.insert(u"v2ray.location.asset"_qs, settings.AssetsPath);
    switch (settings.LogLevel)
    {
        case V2RayCorePluginSettings::None: env.insert(u"RUST_LOG"_qs, u"off"_qs); break;
        case V2RayCorePluginSettings::Error: env.insert(u"RUST_LOG"_qs, u"info,v2ray_rust=error"_qs); break;
        case V2RayCorePluginSettings::Warning: env.insert(u"RUST_LOG"_qs, u"info,v2ray_rust=warn"_qs); break;
        case V2RayCorePluginSettings::Info: env.insert(u"RUST_LOG"_qs, u"info"_qs); break;
        case V2RayCorePluginSettings::Debug: env.insert(u"RUST_LOG"_qs, u"info,v2ray_rust=debug"_qs); break;
        default: env.insert(u"RUST_LOG"_qs, u"info,v2ray_rust=warn"_qs); break;
    }
    vProcess->setProcessEnvironment(env);
    vProcess->setProcessChannelMode(QProcess::MergedChannels);
    vProcess->start(settings.CorePath, { u"--config"_qs, configFilePath }, QIODevice::ReadWrite | QIODevice::Text);
    vProcess->waitForStarted();
    kernelStarted = true;

    apiEnabled = false;
    if (qEnvironmentVariableIsSet(V2RAYPLUGIN_NO_API_ENV))
    {
        V2RayCorePluginClass::Log(u"API has been disabled by the command line arguments"_qs);
    }
    else if (!settings.APIEnabled)
    {
        V2RayCorePluginClass::Log(u"API has been disabled by the global config option"_qs);
    }
    else if (tagProtocolMap.isEmpty())
    {
        V2RayCorePluginClass::Log(u"RARE: API is disabled since no inbound tags configured. This is usually caused by a bad complex config."_qs);
    }
    else
    {
        V2RayCorePluginClass::Log(u"Starting API"_qs);
        apiWorker->StartAPI(tagProtocolMap);
        apiEnabled = true;
    }
}

bool V2RayRustKernel::Stop()
{
    if (apiEnabled)
    {
        apiWorker->StopAPI();
        apiEnabled = false;
    }

    // Set this to false BEFORE close the Process, since we need this flag
    // to capture the real kernel CRASH
    kernelStarted = false;
    vProcess->terminate();
    // 1000ms is enough for v2ray-rust gracefully shutdown;
    vProcess->waitForFinished(1000);
    vProcess->kill();
    vProcess->close();
    // Block until V2Ray core exits
    // Should we use -1 instead of waiting for 30secs?
    vProcess->waitForFinished();
    return true;
}

std::optional<QString> V2RayRustKernel::ValidateConfig(const QString &path)
{
    const auto settings = V2RayCorePluginClass::PluginInstance->settings;
    if (const auto &[result, msg] = ValidateKernel(settings.CorePath, settings.AssetsPath, { u"--help"_qs }); result)
    {
        V2RayCorePluginClass::Log(u"v2ray-rust version: "_qs + *msg);
        // Append assets location env.
        auto env = QProcessEnvironment::systemEnvironment();
        env.insert(u"v2ray.location.asset"_qs, settings.AssetsPath);

        QProcess process;
        process.setProcessEnvironment(env);
        process.setProcessChannelMode(QProcess::MergedChannels);
        V2RayCorePluginClass::Log(u"Starting V2Ray core with test options"_qs);
        process.start(settings.CorePath, { u"--test"_qs, u"--config"_qs, path }, QIODevice::ReadWrite | QIODevice::Text);
        process.waitForFinished();

        if (process.exitCode() != 0)
        {
            const auto output = QString::fromUtf8(process.readAllStandardOutput());
            const auto msg = output.mid(output.indexOf(u"anti-censorship."_qs) + 17).replace(u'>', u"\n >"_qs);
            V2RayCorePluginClass::ShowMessageBox(QObject::tr("Configuration Error"), msg);
            return msg;
        }

        V2RayCorePluginClass::Log(u"Config file check passed."_qs);
        return std::nullopt;
    }
    else
    {
        return msg;
    }
}
