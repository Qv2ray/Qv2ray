#include "Qv2rayApplication.hpp"

#include "GuiPluginHost/GuiPluginHost.hpp"
#include "Qv2rayBase/Common/Utils.hpp"
#include "Qv2rayBase/Interfaces/IStorageProvider.hpp"
#include "Qv2rayBase/Profile/KernelManager.hpp"
#include "Qv2rayBase/Profile/ProfileManager.hpp"
#include "StyleManager/StyleManager.hpp"
#include "models/SettingsModels.hpp"
#include "ui/windows/w_MainWindow.hpp"

#include <QCommandLineParser>
#include <QDesktopServices>
#include <QSessionManager>
#include <QSettings>
#include <QSslSocket>
#include <QUrl>
#include <QUrlQuery>
#include <QVersionNumber>
#include <QtPlugin>
#include <openssl/ssl.h>

constexpr auto QV2RAY_GUI_EXTRASETTINGS_KEY = "qv2ray-gui-settings";

#ifdef QT_DEBUG
const static inline auto QV2RAY_URL_SCHEME = "qv2ray-debug";
#else
const static inline auto QV2RAY_URL_SCHEME = "qv2ray";
#endif

Q_IMPORT_PLUGIN(Qv2rayInternalPlugin);

Qv2rayApplication::Qv2rayApplication(int &argc, char *argv[]) : SingleApplication(argc, argv, true, User | ExcludeAppPath | ExcludeAppVersion)
{
    // These no-op function calls ensures libssl and libcrypto are linked against Qv2ray.
    // Forcing the deployment of libssl and libcrypto libraries.
    // So that the OpenSSL TLS backend can be used by Qt Network.
    // TODO Find a workaround without this hack.
    Q_UNUSED(SSL_new(nullptr));

    baseLibrary = new Qv2rayBase::Qv2rayBaseLibrary;
    Qv2rayLogo = QPixmap{ u":/qv2ray.png"_qs };
    installEventFilter(this);
}

Qv2rayApplication::~Qv2rayApplication()
{
    delete baseLibrary;
}

Qv2rayExitReason Qv2rayApplication::GetExitReason() const
{
    return exitReason;
}

bool Qv2rayApplication::Initialize()
{
    QString errorMessage;
    bool canContinue;
    const auto hasError = parseCommandLine(&errorMessage, &canContinue);
    qInfo() << "Command line:" << errorMessage;
    if (hasError && !canContinue)
    {
        qInfo() << "Fatal, Qv2ray cannot continue.";
        exitReason = EXIT_INITIALIZATION_FAILED;
        return false;
    }

    Qv2rayBase::Interfaces::StorageContext ctx;
#ifdef QT_DEBUG
    ctx << Qv2rayBase::Interfaces::StorageContextFlags::STORAGE_CTX_IS_DEBUG;
#endif

#ifdef Q_OS_WINDOWS
    ctx << Qv2rayBase::Interfaces::StorageContextFlags::STORAGE_CTX_HAS_ASIDE_CONFIGURATION;
#endif

    const auto result = baseLibrary->Initialize(StartupArguments.noPlugins ? Qv2rayBase::START_NO_PLUGINS : Qv2rayBase::START_NORMAL, ctx, this);
    if (result != Qv2rayBase::NORMAL)
    {
        exitReason = EXIT_INITIALIZATION_FAILED;
        return false;
    }

#ifdef Q_OS_WIN
    const auto appPath = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
    const auto regPath = "HKEY_CURRENT_USER\\Software\\Classes\\" + QString::fromUtf8(QV2RAY_URL_SCHEME);
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

    connect(this, &QApplication::aboutToQuit, this, &Qv2rayApplication::quitInternal);
    connect(this, &SingleApplication::receivedMessage, this, &Qv2rayApplication::onMessageReceived, Qt::QueuedConnection);
    if (isSecondary())
    {
        StartupArguments.version = QV2RAY_VERSION_STRING;
        StartupArguments.fullArgs = arguments();
        if (StartupArguments.arguments.isEmpty())
            StartupArguments.arguments << Qv2rayStartupArguments::NORMAL;
        bool status = sendMessage(JsonToString(StartupArguments.toJson(), QJsonDocument::Compact).toUtf8());
        if (!status)
            qInfo() << "Cannot send message.";
        exitReason = EXIT_SECONDARY_INSTANCE;
        return false;
    }

    connect(this, &QGuiApplication::commitDataRequest, this, &Qv2rayApplication::SaveQv2raySettings, Qt::DirectConnection);

#ifdef Q_OS_WIN
    SetCurrentDirectory(QCoreApplication::applicationDirPath().toStdWString().c_str());
#endif

    GlobalConfig = new Qv2rayApplicationConfigObject;
    GlobalConfig->loadJson(QvStorageProvider->GetExtraSettings(QString::fromUtf8(QV2RAY_GUI_EXTRASETTINGS_KEY)));

    GUIPluginHost = new GuiPluginHost::GuiPluginAPIHost;
    UIMessageBus = new MessageBus::QvMessageBusObject;
    StyleManager = new QvStyleManager::QvStyleManager;
    StyleManager->ApplyStyle(GlobalConfig->appearanceConfig->UITheme);

    setQuitOnLastWindowClosed(false);
    return true;
}

Qv2rayExitReason Qv2rayApplication::RunQv2ray()
{
    trayManager = new Qv2ray::ui::TrayManager;
    mainWindow = new MainWindow();

    connect(trayManager, &TrayManager::TrayActivated, mainWindow, &MainWindow::OnTrayIconActivated);
    connect(trayManager, &TrayManager::VisibilityToggled, mainWindow, &MainWindow::MWToggleVisibility);

    if (StartupArguments.arguments.contains(Qv2rayStartupArguments::QV2RAY_LINK))
    {
        for (const auto &link : StartupArguments.links)
        {
            const auto url = QUrl::fromUserInput(link);
            const auto command = url.host();
            auto subcommands = url.path().split(u"/"_qs);
            subcommands.removeAll("");
            QMap<QString, QString> args;
            for (const auto &kvp : QUrlQuery(url).queryItems())
            {
                args.insert(kvp.first, kvp.second);
            }
            if (command == u"open"_qs)
            {
                mainWindow->ProcessCommand(command, subcommands, args);
            }
        }
    }
    return (Qv2rayExitReason) exec();
}

void Qv2rayApplication::quitInternal()
{
    delete mainWindow;
    delete trayManager;
    delete StyleManager;
    delete GUIPluginHost;
    SaveQv2raySettings();
    QvBaselib->Shutdown();
}

bool Qv2rayApplication::parseCommandLine(QString *errorMessage, bool *canContinue)
{
    *canContinue = true;
    QStringList filteredArgs;
    filteredArgs.reserve(QCoreApplication::arguments().size());
    for (const auto &arg : QCoreApplication::arguments())
    {
#ifdef Q_OS_MACOS
        if (arg.contains("-psn"))
            continue;
#endif
        filteredArgs << arg;
    }
    QCommandLineParser parser;
    QCommandLineOption noAPIOption(u"noAPI"_qs, QObject::tr("Disable gRPC API subsystem"));
    QCommandLineOption noPluginsOption(u"noPlugin"_qs, QObject::tr("Disable plugins feature"));
    QCommandLineOption debugLogOption(u"debug"_qs, QObject::tr("Enable debug output"));
    QCommandLineOption noAutoConnectionOption(u"noAutoConnection"_qs, QObject::tr("Do not automatically connect"));
    QCommandLineOption disconnectOption(u"disconnect"_qs, QObject::tr("Stop current connection"));
    QCommandLineOption reconnectOption(u"reconnect"_qs, QObject::tr("Reconnect last connection"));
    QCommandLineOption exitOption(u"exit"_qs, QObject::tr("Exit Qv2ray"));

    parser.setApplicationDescription(QObject::tr("Qv2ray - A cross-platform Qt frontend for V2Ray."));
    parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);

    parser.addOption(noAPIOption);
    parser.addOption(noPluginsOption);
    parser.addOption(debugLogOption);
    parser.addOption(noAutoConnectionOption);
    parser.addOption(disconnectOption);
    parser.addOption(reconnectOption);
    parser.addOption(exitOption);

    const auto helpOption = parser.addHelpOption();
    const auto versionOption = parser.addVersionOption();

    if (!parser.parse(filteredArgs))
    {
        *canContinue = true;
        *errorMessage = parser.errorText();
        return false;
    }

    if (parser.isSet(versionOption))
    {
        parser.showVersion();
        return true;
    }

    if (parser.isSet(helpOption))
    {
        parser.showHelp();
        return true;
    }

    for (const auto &arg : parser.positionalArguments())
    {
        if (arg.startsWith(QString(QV2RAY_URL_SCHEME) + "://"))
        {
            StartupArguments.arguments << Qv2rayStartupArguments::QV2RAY_LINK;
            StartupArguments.links << arg;
        }
    }

    if (parser.isSet(exitOption))
    {
        qDebug() << "exitOption is set.";
        StartupArguments.arguments << Qv2rayStartupArguments::EXIT;
    }

    if (parser.isSet(disconnectOption))
    {
        qDebug() << "disconnectOption is set.";
        StartupArguments.arguments << Qv2rayStartupArguments::DISCONNECT;
    }

    if (parser.isSet(reconnectOption))
    {
        qDebug() << "reconnectOption is set.";
        StartupArguments.arguments << Qv2rayStartupArguments::RECONNECT;
    }

#define ProcessExtraStartupOptions(option)                                                                                                                               \
    qDebug() << "Startup Options:" << #option << parser.isSet(option##Option);                                                                                          \
    StartupArguments.option = parser.isSet(option##Option);

    ProcessExtraStartupOptions(noAPI);
    ProcessExtraStartupOptions(debugLog);
    ProcessExtraStartupOptions(noAutoConnection);
    ProcessExtraStartupOptions(noPlugins);
    return true;
}

void Qv2rayApplication::p_OpenURL(const QUrl &url)
{
    QDesktopServices::openUrl(url);
}

bool Qv2rayApplication::eventFilter(QObject *watched, QEvent *event)
{
#ifdef Q_OS_MACOS
    static Qt::ApplicationState _prevAppState;
    if (watched == this && event->type() == QEvent::ApplicationStateChange)
    {
        auto ev = static_cast<QApplicationStateChangeEvent *>(event);
        if (_prevAppState == Qt::ApplicationActive && ev->applicationState() == Qt::ApplicationActive)
        {
            mainWindow->MWShowWindow();
        }
        _prevAppState = ev->applicationState();
    }
#endif // Q_OS_MACOS
    return QApplication::eventFilter(watched, event);
}

void Qv2rayApplication::p_MessageBoxWarn(const QString &title, const QString &text)
{
    QMessageBox::warning(nullptr, title, text, QMessageBox::Ok);
}

void Qv2rayApplication::p_MessageBoxInfo(const QString &title, const QString &text)
{
    QMessageBox::information(nullptr, title, text, QMessageBox::Ok);
}

Qv2rayBase::MessageOpt Qv2rayApplication::p_MessageBoxAsk(const QString &title, const QString &text, const QList<Qv2rayBase::MessageOpt> &buttons)
{
    QFlags<QMessageBox::StandardButton> btns;
    for (const auto &b : buttons)
    {
        btns.setFlag(MessageBoxButtonMap.value(b));
    }
    return MessageBoxButtonMap.key(QMessageBox::question(nullptr, title, text, btns));
}

MainWindow *Qv2rayApplication::GetMainWindow() const
{
    return mainWindow;
}

TrayManager *Qv2rayApplication::GetTrayManager() const
{
    return trayManager;
}

void Qv2rayApplication::SaveQv2raySettings()
{
    QvBaselib->SaveConfigurations();
    QvStorageProvider->StoreExtraSettings(QString::fromUtf8(QV2RAY_GUI_EXTRASETTINGS_KEY), GlobalConfig->toJson());
}

void Qv2rayApplication::onMessageReceived(quint32 clientId, const QByteArray &_msg)
{
    // Sometimes SingleApplication sends message with clientId == current id, ignore that.
    if (clientId == instanceId())
        return;

    Qv2rayStartupArguments msg;
    msg.loadJson(JsonFromString(_msg));
    qInfo() << "Received message, version:" << msg.version << "From client ID:" << clientId;
    qInfo() << _msg;

    if (QVersionNumber::fromString(msg.version) > QVersionNumber::fromString(QV2RAY_VERSION_STRING))
    {
        const auto newPath = msg.fullArgs.constFirst();
        QString message;
        message += tr("A new version of Qv2ray is starting:") + NEWLINE;
        message += '\n';
        message += tr("New version information: ") + '\n';
        message += tr("Version: %1").arg(msg.version) + '\n';
        message += tr("Path: %1").arg(newPath) + '\n';
        message += '\n';
        message += tr("Do you want to exit and launch that new version?");

        const auto result = p_MessageBoxAsk(tr("New version detected"), message, { Qv2rayBase::MessageOpt::Yes, Qv2rayBase::MessageOpt::No });
        if (result == Qv2rayBase::MessageOpt::Yes)
        {
            StartupArguments._qvNewVersionPath = newPath;
            exitReason = EXIT_NEW_VERSION_TRIGGER;
            QCoreApplication::quit();
        }
    }

    for (const auto &argument : msg.arguments)
    {
        switch (argument)
        {
            case Qv2rayStartupArguments::EXIT:
            {
                exitReason = EXIT_NORMAL;
                quit();
                break;
            }
            case Qv2rayStartupArguments::NORMAL:
            {
                mainWindow->show();
                mainWindow->raise();
                mainWindow->activateWindow();
                break;
            }
            case Qv2rayStartupArguments::RECONNECT:
            {
                QvProfileManager->StartConnection(QvKernelManager->CurrentConnection());
                break;
            }
            case Qv2rayStartupArguments::DISCONNECT:
            {
                QvProfileManager->StopConnection();
                break;
            }
            case Qv2rayStartupArguments::QV2RAY_LINK:
            {
                for (const auto &link : msg.links)
                {
                    const auto url = QUrl::fromUserInput(link);
                    const auto command = url.host();
                    auto subcommands = url.path().split('/');
                    subcommands.removeAll("");
                    QMap<QString, QString> args;
                    for (const auto &kvp : QUrlQuery(url).queryItems())
                    {
                        args.insert(kvp.first, kvp.second);
                    }
                    if (command == u"open"_qs)
                    {
                        mainWindow->ProcessCommand(command, subcommands, args);
                    }
                }
                break;
            }
        }
    }
}
