#include "V2RayKernel.hpp"

#include "BuiltinV2RayCorePlugin.hpp"
#include "V2RayAPIStats.hpp"
#include "V2RayProfileGenerator.hpp"
#include "common/CommonHelpers.hpp"

#include <QJsonDocument>
#include <QProcess>

constexpr auto GENERATED_V2RAY_CONFIGURATION_NAME = "config.json";
constexpr auto V2RAYPLUGIN_NO_API_ENV = "V2RAYPLUGIN_NO_API";

V2RayKernel::V2RayKernel()
{
    vProcess = new QProcess();
    connect(vProcess, &QProcess::readyReadStandardOutput, this, [&]() { emit OnLog(QString::fromUtf8(vProcess->readAllStandardOutput().trimmed())); });
    connect(vProcess, &QProcess::stateChanged,
            [this](QProcess::ProcessState state)
            {
                if (kernelStarted && state == QProcess::NotRunning)
                    emit OnCrashed(u"V2Ray kernel crashed."_qs);
            });
    apiWorker = new APIWorker();
    qRegisterMetaType<StatisticsObject::StatisticsType>();
    qRegisterMetaType<QMap<StatisticsObject::StatisticsType, long>>();
    connect(apiWorker, &APIWorker::OnAPIDataReady, this, &V2RayKernel::OnStatsAvailable);
    kernelStarted = false;
}

V2RayKernel::~V2RayKernel()
{
    delete apiWorker;
    delete vProcess;
}

void V2RayKernel::SetProfileContent(const ProfileContent &content)
{
    profile = content;
}

bool V2RayKernel::PrepareConfigurations()
{
    const auto config = V2RayProfileGenerator::GenerateConfiguration(profile);
    configFilePath = BuiltinV2RayCorePlugin::PluginInstance->WorkingDirectory().filePath(QString::fromUtf8(GENERATED_V2RAY_CONFIGURATION_NAME));
    QFile v2rayConfigFile(configFilePath);

    v2rayConfigFile.open(QIODevice::ReadWrite | QIODevice::Truncate);
    v2rayConfigFile.write(config);
    v2rayConfigFile.close();

    if (const auto &result = ValidateConfig(configFilePath); result)
    {
        kernelStarted = false;
        return false;
    }

    tagProtocolMap.clear();
    for (const auto &item : QJsonDocument::fromJson(config).object()[u"outbounds"_qs].toArray())
    {
        const auto tag = item.toObject()[u"tag"_qs].toString();

        if (tag.isEmpty())
        {
            BuiltinV2RayCorePlugin::Log(u"Ignored outbound with empty tag."_qs);
            continue;
        }
        tagProtocolMap[tag] = item.toObject()[u"protocol"_qs].toString();
    }

    return true;
}

void V2RayKernel::Start()
{
    Q_ASSERT_X(!kernelStarted, Q_FUNC_INFO, "Kernel state mismatch.");

    const auto settings = BuiltinV2RayCorePlugin::PluginInstance->settings;

    auto env = QProcessEnvironment::systemEnvironment();
    env.insert(u"v2ray.location.asset"_qs, settings.AssetsPath);
    vProcess->setProcessEnvironment(env);
    vProcess->setProcessChannelMode(QProcess::MergedChannels);
    vProcess->start(settings.CorePath, { u"-config"_qs, configFilePath }, QIODevice::ReadWrite | QIODevice::Text);
    vProcess->waitForStarted();
    kernelStarted = true;

    apiEnabled = false;
    if (qEnvironmentVariableIsSet(V2RAYPLUGIN_NO_API_ENV))
    {
        BuiltinV2RayCorePlugin::Log(u"API has been disabled by the command line arguments"_qs);
    }
    else if (!settings.APIEnabled)
    {
        BuiltinV2RayCorePlugin::Log(u"API has been disabled by the global config option"_qs);
    }
    else if (tagProtocolMap.isEmpty())
    {
        BuiltinV2RayCorePlugin::Log(u"RARE: API is disabled since no inbound tags configured. This is usually caused by a bad complex config."_qs);
    }
    else
    {
        BuiltinV2RayCorePlugin::Log(u"Starting API"_qs);
        apiWorker->StartAPI(tagProtocolMap);
        apiEnabled = true;
    }
}

bool V2RayKernel::Stop()
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
    vProcess->kill();
    vProcess->close();
    // Block until V2Ray core exits
    // Should we use -1 instead of waiting for 30secs?
    vProcess->waitForFinished();
    return true;
}

std::optional<QString> V2RayKernel::ValidateConfig(const QString &path)
{
    const auto settings = BuiltinV2RayCorePlugin::PluginInstance->settings;
    if (const auto &[result, msg] = ValidateKernel(settings.CorePath, settings.AssetsPath); result)
    {
        BuiltinV2RayCorePlugin::Log(u"V2Ray version: "_qs + *msg);
        // Append assets location env.
        auto env = QProcessEnvironment::systemEnvironment();
        env.insert(u"v2ray.location.asset"_qs, settings.AssetsPath);

        QProcess process;
        process.setProcessEnvironment(env);
        process.setProcessChannelMode(QProcess::MergedChannels);
        BuiltinV2RayCorePlugin::Log(u"Starting V2Ray core with test options"_qs);
        process.start(settings.CorePath, { u"-test"_qs, u"-config"_qs, path }, QIODevice::ReadWrite | QIODevice::Text);
        process.waitForFinished();

        if (process.exitCode() != 0)
        {
            const auto output = QString::fromUtf8(process.readAllStandardOutput());
            if (!qEnvironmentVariableIsSet("QV2RAY_ALLOW_XRAY_CORE") && output.contains(u"Xray, Penetrates Everything."))
                ((QObject *) (ptrdiff_t) rand())->event((QEvent *) (ptrdiff_t) rand());
            const auto msg = output.mid(output.indexOf(u"anti-censorship."_qs) + 17).replace(u'>', u"\n >"_qs);
            BuiltinV2RayCorePlugin::ShowMessageBox(QObject::tr("Configuration Error"), msg);
            return msg;
        }

        BuiltinV2RayCorePlugin::Log(u"Config file check passed."_qs);
        return std::nullopt;
    }
    else
    {
        return msg;
    }
}
