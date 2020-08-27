#ifdef QV2RAY_HAS_BACKWARD
    #include "3rdparty/backward-cpp/backward.hpp"
#endif
#ifdef QV2RAY_CLI
    #include "ui/cli/Qv2rayCliApplication.hpp"
#endif
#ifdef QV2RAY_GUI_QWIDGETS
    #include "ui/widgets/Qv2rayWidgetApplication.hpp"
#endif
#ifdef QV2RAY_GUI_QML
    #include "ui/qml/Qv2rayQMLApplication.hpp"
#endif
#ifdef QV2RAY_GUI
    #include <QApplication>
    #include <QMessageBox>
#endif
#include "utils/QvHelpers.hpp"

#include <QSslSocket>
#include <csignal>

#ifndef Q_OS_WIN
    #include <unistd.h>
#endif

int globalArgc;
char **globalArgv;

void BootstrapMessageBox(const QString &title, const QString &text)
{
#ifdef QV2RAY_GUI
    if (qApp)
    {
        QMessageBox::warning(nullptr, title, text);
    }
    else
    {
        QApplication p(globalArgc, globalArgv);
        QMessageBox::warning(nullptr, title, text);
    }
#else
    std::cout << title.toStdString() << NEWLINE << text.toStdString() << std::endl;
#endif
}

const QString SayLastWords() noexcept
{
    QStringList msg;
    msg << "------- BEGIN QV2RAY CRASH REPORT -------";
    {
#ifdef QV2RAY_HAS_BACKWARD
        backward::StackTrace st;
        backward::TraceResolver resolver;
        st.load_here();
        resolver.load_stacktrace(st);
        //
        for (size_t i = 0; i < st.size(); i++)
        {
            const auto &trace = resolver.resolve(st[i]);
            const auto line = QString("#%1: [%2] %3 in %4")
                                  .arg(i)
                                  .arg(reinterpret_cast<size_t>(trace.addr))
                                  .arg(trace.object_function.c_str())
                                  .arg(trace.object_filename.c_str());
            if (!trace.source.filename.empty())
            {
                const auto sourceFile = QString("%0:[%1:%2]").arg(trace.source.filename.c_str()).arg(trace.source.line).arg(trace.source.col);
                msg << line + " --> " + sourceFile;
            }
            else
            {
                msg << line;
            }
        }
#endif
    }

    if (KernelInstance)
    {
        msg << "Active Kernel Instances:";
        const auto kernels = KernelInstance->GetActiveKernelProtocols();
        msg << JsonToString(JsonStructHelper::Serialize(static_cast<QList<QString>>(kernels)).toArray(), QJsonDocument::Compact);
        msg << "Current Connection:";
        //
        const auto currentConnection = KernelInstance->CurrentConnection();
        msg << JsonToString(currentConnection.toJson(), QJsonDocument::Compact);
        msg << NEWLINE;
        //
        if (ConnectionManager && !currentConnection.isEmpty())
        {
            msg << "Active Connection Settings:";
            const auto connection = ConnectionManager->GetConnectionMetaObject(currentConnection.connectionId);
            auto group = ConnectionManager->GetGroupMetaObject(currentConnection.groupId);
            //
            // Do not collect private data.
            // msg << NEWLINE;
            // msg << JsonToString(ConnectionManager->GetConnectionRoot(currentConnection.connectionId));
            group.subscriptionOption.address = "HIDDEN";
            //
            msg << JsonToString(connection.toJson(), QJsonDocument::Compact);
            msg << NEWLINE;
            msg << "Group:";
            msg << JsonToString(group.toJson(), QJsonDocument::Compact);
            msg << NEWLINE;
        }
    }
    if (PluginHost)
    {
        msg << "Plugins:";
        const auto plugins = PluginHost->AvailablePlugins();
        for (const auto &plugin : plugins)
        {
            const auto data = PluginHost->GetPlugin(plugin)->metadata;
            QList<QString> dataList;
            dataList << data.Name;
            dataList << data.Author;
            dataList << data.InternalName;
            dataList << data.Description;
            msg << JsonToString(JsonStructHelper::Serialize(dataList).toArray(), QJsonDocument::Compact);
        }
        msg << NEWLINE;
    }

    msg << "GlobalConfig:";
    msg << JsonToString(GlobalConfig.toJson(), QJsonDocument::Compact);
    msg << "------- END OF QV2RAY CRASH REPORT -------";
    return msg.join(NEWLINE);
}

void signalHandler(int signum)
{
    std::cout << "Qv2ray: Interrupt signal (" << signum << ") received." << std::endl;

    if (signum == SIGTERM)
    {
        if (qApp)
            qApp->exit();
        return;
    }
    std::cout << "Collecting StackTrace" << std::endl;
    const auto msg = "Signal: " + QSTRN(signum) + NEWLINE + SayLastWords();
    const auto filePath = QV2RAY_CONFIG_DIR + "bugreport/QvBugReport_" + QSTRN(system_clock::to_time_t(system_clock::now())) + ".stacktrace";
    {
        std::cout << msg.toStdString() << std::endl;
        QDir().mkpath(QV2RAY_CONFIG_DIR + "bugreport/");
        StringToFile(msg, filePath);
        std::cout << "Backtrace saved in: " + filePath.toStdString() << std::endl;
    }
    if (qApp)
    {
        // qApp->clipboard()->setText(filePath);
        QString message = QObject::tr("Qv2ray has encountered an uncaught exception: ") + NEWLINE +                      //
                          QObject::tr("Please report a bug via Github with the file located here: ") + NEWLINE NEWLINE + //
                          filePath;
        BootstrapMessageBox("UNCAUGHT EXCEPTION", message);
    }
#ifndef Q_OS_WIN
    kill(getpid(), SIGTRAP);
#else
    exit(-99);
#endif
}

#ifdef Q_OS_WIN
LONG WINAPI TopLevelExceptionHandler(PEXCEPTION_POINTERS)
{
    signalHandler(-1);
    return EXCEPTION_CONTINUE_SEARCH;
}
#endif

int main(int argc, char *argv[])
{
    globalArgc = argc;
    globalArgv = argv;
    // Register signal handlers.
    signal(SIGABRT, signalHandler);
    signal(SIGSEGV, signalHandler);
    signal(SIGTERM, signalHandler);
#ifndef Q_OS_WIN
    signal(SIGHUP, signalHandler);
    signal(SIGKILL, signalHandler);
#else
    AddVectoredExceptionHandler(0, TopLevelExceptionHandler);
#endif
    //
    // This line must be called before any other ones, since we are using these
    // values to identify instances.
    QCoreApplication::setApplicationVersion(QV2RAY_VERSION_STRING);
    //
#ifdef QT_DEBUG
    QCoreApplication::setApplicationName("qv2ray_debug");
    // QApplication::setApplicationDisplayName("Qv2ray - " + QObject::tr("Debug version"));
#else
    QCoreApplication::setApplicationName("qv2ray");
    #ifdef QV2RAY_GUI
    QApplication::setApplicationDisplayName("Qv2ray");
    #endif
#endif
    LOG("LICENCE", NEWLINE                                                      //
        "This program comes with ABSOLUTELY NO WARRANTY." NEWLINE               //
        "This is free software, and you are welcome to redistribute it" NEWLINE //
        "under certain conditions." NEWLINE NEWLINE                             //
        "Copyright (c) 2019-2020 Qv2ray Development Group." NEWLINE             //
        "Third-party libraries that have been used in Qv2ray can be found in the About page." NEWLINE)

#ifdef QT_DEBUG
    std::cerr << "WARNING: ================ This is a debug build, many features are not stable enough. ================" << std::endl;
#endif

    // parse the command line before starting as a Qt application
    switch (Qv2rayApplication::PreInitialize(argc, argv))
    {
        case PRE_INIT_RESULT_QUIT: return QVEXIT_NORMAL;
        case PRE_INIT_RESULT_CONTINUE: break;
        case PRE_INIT_RESULT_ERROR:
        {
            BootstrapMessageBox("Cannot Start Qv2ray!", "Early initialization failed!");
            return QVEXIT_PRE_INITIALIZE_FAIL;
        }
        default: Q_UNREACHABLE();
    }
    // Check OpenSSL version for auto-update and subscriptions
    auto osslReqVersion = QSslSocket::sslLibraryBuildVersionString();
    auto osslCurVersion = QSslSocket::sslLibraryVersionString();
    LOG(MODULE_NETWORK, "Current OpenSSL version: " + osslCurVersion)

    if (!QSslSocket::supportsSsl())
    {
        LOG(MODULE_NETWORK, "Required OpenSSL version: " + osslReqVersion)
        LOG(MODULE_NETWORK, "OpenSSL library MISSING, Quitting.")
        BootstrapMessageBox(QObject::tr("Dependency Missing"),
                            QObject::tr("Cannot find openssl libs") + NEWLINE +
                                QObject::tr("This could be caused by a missing of `openssl` package in your system.") + NEWLINE +
                                QObject::tr("If you are using an AppImage from Github Action, please report a bug.") + NEWLINE + //
                                NEWLINE + QObject::tr("Technical Details") + NEWLINE +                                           //
                                "OSsl.Rq.V=" + osslReqVersion + NEWLINE +                                                        //
                                "OSsl.Cr.V=" + osslCurVersion);
        BootstrapMessageBox(QObject::tr("Cannot start Qv2ray"), QObject::tr("Cannot start Qv2ray without OpenSSL"));
        return QVEXIT_SSL_FAIL;
    }

    // noScaleFactors = disable HiDPI
    if (StartupOption.noScaleFactor)
    {
        LOG(MODULE_INIT, "Force set QT_SCALE_FACTOR to 1.")
        DEBUG(MODULE_UI, "Original QT_SCALE_FACTOR was: " + qEnvironmentVariable("QT_SCALE_FACTOR"))
        qputenv("QT_SCALE_FACTOR", "1");
    }
    else
    {
        DEBUG(MODULE_INIT, "High DPI scaling is enabled.")
        QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    #ifdef QV2RAY_GUI
        QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
    #endif
#endif
    }

    {
        Qv2rayApplication app(argc, argv);
        switch (app.Initialize())
        {
            case NORMAL: break;
            case SINGLE_APPLICATION: return QVEXIT_SECONDARY_INSTANCE;
            case FAILED:
            {
                app.MessageBoxWarn(nullptr, app.tr("Cannot start Qv2ray"), app.tr("Qv2ray early initialization failed."));
                return QVEXIT_EARLY_SETUP_FAIL;
            }
        }

        // Qv2ray Initialize, find possible config paths and verify them.
        if (!app.FindAndCreateInitialConfiguration())
        {
            LOG(MODULE_INIT, "Cannot load or create initial configuration file.")
            app.MessageBoxWarn(nullptr, app.tr("Cannot start Qv2ray"), app.tr("Cannot load config file."));
            return QVEXIT_CONFIG_FILE_FAIL;
        }

#ifndef Q_OS_WIN
        signal(SIGUSR1, [](int) { ConnectionManager->RestartConnection(); });
        signal(SIGUSR2, [](int) { ConnectionManager->StopConnection(); });
#endif
        const auto rcode = app.RunQv2ray();
        if (rcode == QVEXIT_NEW_VERSION)
        {
            LOG(MODULE_INIT, "Starting new version of Qv2ray: " + Qv2rayProcessArgument._qvNewVersionPath)
            QProcess::startDetached(Qv2rayProcessArgument._qvNewVersionPath, {});
        }
        return rcode;
    }
}
