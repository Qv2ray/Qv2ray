#include "Qv2rayBaseApplication.hpp"

#ifdef QT_DEBUG
const static inline QString QV2RAY_URL_SCHEME = "qv2ray-debug";
#else
const static inline QString QV2RAY_URL_SCHEME = "qv2ray";
#endif

Qv2rayApplicationManagerInterface::Qv2rayApplicationManagerInterface(int, char *[])
{
    qvApplicationInstance = this;
    LOG(MODULE_INIT, "Qv2ray " QV2RAY_VERSION_STRING " on " + QSysInfo::prettyProductName() + " " + QSysInfo::currentCpuArchitecture())
    DEBUG(MODULE_INIT, "Qv2ray Start Time: " + QSTRN(QTime::currentTime().msecsSinceStartOfDay()))
    DEBUG("QV2RAY_BUILD_INFO", QV2RAY_BUILD_INFO)
    DEBUG("QV2RAY_BUILD_EXTRA_INFO", QV2RAY_BUILD_EXTRA_INFO)
    DEBUG("QV2RAY_BUILD_NUMBER", QSTRN(QV2RAY_VERSION_BUILD))
}

Qv2rayApplicationManagerInterface::~Qv2rayApplicationManagerInterface()
{
    qvApplicationInstance = nullptr;
}

Qv2rayPreInitResult Qv2rayApplicationManagerInterface::PreInitialize(int argc, char **argv)
{
    QString errorMessage;
    Qv2rayPreInitResult result;
    {
        QCoreApplication coreApp(argc, argv);
        const auto &args = coreApp.arguments();
        Qv2rayProcessArgument.version = QV2RAY_VERSION_STRING;
        Qv2rayProcessArgument.fullArgs = args;
        result = ParseCommandLine(&errorMessage, args);
        LOG(MODULE_INIT, "Qv2ray PreInitialization: " + errorMessage)
        if (result != PRE_INIT_RESULT_CONTINUE)
            return result;
#ifdef Q_OS_WIN
        const auto appPath = QDir::toNativeSeparators(coreApp.applicationFilePath());
        const auto regPath = "HKEY_CURRENT_USER\\Software\\Classes\\" + QV2RAY_URL_SCHEME;

        QSettings reg(regPath, QSettings::NativeFormat);

        reg.setValue("Default", "Qv2ray");
        reg.setValue("URL Protocol", "");

        reg.beginGroup("DefaultIcon");
        reg.setValue("Default", QString("%1,1").arg(appPath));
        reg.endGroup();

        reg.beginGroup("shell");
        reg.beginGroup("open");
        reg.beginGroup("command");
        reg.setValue("Default", appPath + " %1");
#endif
    }
    return result;
}

Qv2rayPreInitResult Qv2rayApplicationManagerInterface::ParseCommandLine(QString *errorMessage, const QStringList &_argx_)
{
    QStringList filteredArgs;
    for (const auto &arg : _argx_)
    {
#ifdef Q_OS_MACOS
        if (arg.contains("-psn"))
            continue;
#endif
        filteredArgs << arg;
    }
    QCommandLineParser parser;
    //
    QCommandLineOption noAPIOption("noAPI", QObject::tr("Disable gRPC API subsystem"));
    QCommandLineOption noPluginsOption("noPlugin", QObject::tr("Disable plugins feature"));
    QCommandLineOption noScaleFactorOption("noScaleFactor", QObject::tr("Disable Qt UI scale factor"));
    QCommandLineOption debugOption("debug", QObject::tr("Enable debug output"));
    QCommandLineOption noAutoConnectionOption("noAutoConnection", QObject::tr("Do not automatically connect"));
    QCommandLineOption disconnectOption("disconnect", QObject::tr("Stop current connection"));
    QCommandLineOption reconnectOption("reconnect", QObject::tr("Reconnect last connection"));
    QCommandLineOption exitOption("exit", QObject::tr("Exit Qv2ray"));
    //
    parser.setApplicationDescription(QObject::tr("Qv2ray - A cross-platform Qt frontend for V2Ray."));
    parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
    //
    parser.addOption(noAPIOption);
    parser.addOption(noPluginsOption);
    parser.addOption(noScaleFactorOption);
    parser.addOption(debugOption);
    parser.addOption(noAutoConnectionOption);
    parser.addOption(disconnectOption);
    parser.addOption(reconnectOption);
    parser.addOption(exitOption);
    //
    auto helpOption = parser.addHelpOption();
    auto versionOption = parser.addVersionOption();

    if (!parser.parse(filteredArgs))
    {
        *errorMessage = parser.errorText();
        return PRE_INIT_RESULT_CONTINUE;
    }

    if (parser.isSet(versionOption))
    {
        parser.showVersion();
        return PRE_INIT_RESULT_QUIT;
    }

    if (parser.isSet(helpOption))
    {
        parser.showHelp();
        return PRE_INIT_RESULT_QUIT;
    }

    for (const auto &arg : parser.positionalArguments())
    {
        if (arg.startsWith(QV2RAY_URL_SCHEME + "://"))
        {
            Qv2rayProcessArgument.arguments << Qv2rayProcessArguments::QV2RAY_LINK;
            Qv2rayProcessArgument.links << arg;
        }
    }

    if (parser.isSet(exitOption))
    {
        DEBUG(MODULE_INIT, "disconnectOption is set.")
        Qv2rayProcessArgument.arguments << Qv2rayProcessArguments::EXIT;
    }

    if (parser.isSet(disconnectOption))
    {
        DEBUG(MODULE_INIT, "disconnectOption is set.")
        Qv2rayProcessArgument.arguments << Qv2rayProcessArguments::DISCONNECT;
    }

    if (parser.isSet(reconnectOption))
    {
        DEBUG(MODULE_INIT, "reconnectOption is set.")
        Qv2rayProcessArgument.arguments << Qv2rayProcessArguments::RECONNECT;
    }

    if (parser.isSet(noAPIOption))
    {
        DEBUG(MODULE_INIT, "noAPIOption is set.")
        StartupOption.noAPI = true;
    }

    if (parser.isSet(debugOption))
    {
        DEBUG(MODULE_INIT, "debugOption is set.")
        StartupOption.debugLog = true;
    }

    if (parser.isSet(noScaleFactorOption))
    {
        DEBUG(MODULE_INIT, "noScaleFactorOption is set.")
        StartupOption.noScaleFactor = true;
    }

    if (parser.isSet(noAutoConnectionOption))
    {
        DEBUG(MODULE_INIT, "noAutoConnectOption is set.")
        StartupOption.noAutoConnection = true;
    }

    if (parser.isSet(noPluginsOption))
    {
        DEBUG(MODULE_INIT, "noPluginOption is set.")
        StartupOption.noPlugins = true;
    }
    *errorMessage = "OK";
    return PRE_INIT_RESULT_CONTINUE;
}
