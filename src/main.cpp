#include <QtGlobal>

#ifdef QV2RAY_CLI
#include "ui/cli/Qv2rayCliApplication.hpp"
#endif

#ifdef QV2RAY_GUI_QWIDGETS
#include "ui/widgets/Qv2rayWidgetApplication.hpp"
#endif

#ifdef QV2RAY_GUI_QML
#include "ui/qml/Qv2rayQMLApplication.hpp"
#endif

#include "utils/QvHelpers.hpp"

#include <csignal>

#ifndef Q_OS_WIN
#include <unistd.h>
#else
#include <Windows.h>
//
#include <DbgHelp.h>
#endif

#define QV_MODULE_NAME "Init"

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
#ifdef Q_OS_WIN
        void *stack[1024];
        HANDLE process = GetCurrentProcess();
        SymInitialize(process, NULL, TRUE);
        SymSetOptions(SYMOPT_LOAD_ANYTHING);
        WORD numberOfFrames = CaptureStackBackTrace(0, 1024, stack, NULL);
        SYMBOL_INFO *symbol = (SYMBOL_INFO *) malloc(sizeof(SYMBOL_INFO) + (512 - 1) * sizeof(TCHAR));
        symbol->MaxNameLen = 512;
        symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
        DWORD displacement;
        IMAGEHLP_LINE64 *line = (IMAGEHLP_LINE64 *) malloc(sizeof(IMAGEHLP_LINE64));
        line->SizeOfStruct = sizeof(IMAGEHLP_LINE64);
        //
        for (int i = 0; i < numberOfFrames; i++)
        {
            const auto address = (DWORD64) stack[i];
            SymFromAddr(process, address, NULL, symbol);
            if (SymGetLineFromAddr64(process, address, &displacement, line))
            {
                msg << QString("[%1]: %2 (%3:%4)").arg(symbol->Address).arg(symbol->Name).arg(line->FileName).arg(line->LineNumber);
            }
            else
            {
                msg << QString("[%1]: %2 SymGetLineFromAddr64[%3]").arg(symbol->Address).arg(symbol->Name).arg(GetLastError());
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
        const auto plugins = PluginHost->AllPlugins();
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

    if (QvCoreApplication)
    {
        msg << "GlobalConfig:";
        msg << JsonToString(GlobalConfig.toJson(), QJsonDocument::Compact);
    }

    msg << "------- END OF QV2RAY CRASH REPORT -------";
    return msg.join(NEWLINE);
}

void signalHandler(int signum)
{
#ifndef Q_OS_WIN
    if (signum == SIGTRAP)
    {
        exit(-99);
        return;
    }
#endif
    std::cout << "Qv2ray: Interrupt signal (" << signum << ") received." << std::endl;

    if (signum == SIGTERM)
    {
        if (qApp)
            qApp->exit();
        return;
    }
    std::cout << "Collecting StackTrace" << std::endl;
    const auto msg = "Signal: " + QSTRN(signum) + NEWLINE + SayLastWords();
    std::cout << msg.toStdString() << std::endl;

    if (qApp && QvCoreApplication)
    {
        QDir().mkpath(QV2RAY_CONFIG_DIR + "bugreport/");
        const auto filePath = QV2RAY_CONFIG_DIR + "bugreport/QvBugReport_" + QSTRN(system_clock::to_time_t(system_clock::now())) + ".stacktrace";
        StringToFile(msg, filePath);
        std::cout << "Backtrace saved in: " + filePath.toStdString() << std::endl;
        const auto message = QObject::tr("Qv2ray has encountered an uncaught exception: ") + NEWLINE +              //
                             QObject::tr("Please report a bug via Github with the file located here: ") + NEWLINE + //
                             NEWLINE + filePath;
        BootstrapMessageBox("UNCAUGHT EXCEPTION", message);
    }

#if defined Q_OS_WIN || defined QT_DEBUG
    exit(-99);
#else
    kill(getpid(), SIGTRAP);
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
    // AddVectoredExceptionHandler(0, TopLevelExceptionHandler);
#endif
    //
    // This line must be called before any other ones, since we are using these
    // values to identify instances.
    QCoreApplication::setApplicationVersion(QV2RAY_VERSION_STRING);

#ifdef QT_DEBUG
    QCoreApplication::setApplicationName("qv2ray_debug");
#else
    QCoreApplication::setApplicationName("qv2ray");
#endif

#ifdef QV2RAY_GUI
    QApplication::setApplicationDisplayName("Qv2ray");
#endif

#ifdef QT_DEBUG
    std::cerr << "WARNING: ================ This is a debug build, many features are not stable enough. ================" << std::endl;
#endif

    if (qEnvironmentVariableIsSet("QV2RAY_NO_SCALE_FACTORS"))
    {
        LOG("Force set QT_SCALE_FACTOR to 1.");
        DEBUG("UI", "Original QT_SCALE_FACTOR was:", qEnvironmentVariable("QT_SCALE_FACTOR"));
        qputenv("QT_SCALE_FACTOR", "1");
    }
    else
    {
        DEBUG("High DPI scaling is enabled.");
#ifndef QV2RAY_QT6
        QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
#ifdef QV2RAY_GUI
        QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#endif
#endif
    }

#ifndef QV2RAY_QT6
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps, true);
#endif

    Qv2rayApplication app(argc, argv);
    if (const auto list = app.CheckPrerequisites(); !list.isEmpty())
    {
        BootstrapMessageBox("Qv2ray Prerequisites Check Failed", list.join(NEWLINE));
        return Qv2rayExitReason::EXIT_PRECONDITION_FAILED;
    }

    if (!app.Initialize())
    {
        const auto reason = app.GetExitReason();
        if (reason == EXIT_INITIALIZATION_FAILED)
        {
            BootstrapMessageBox("Qv2ray Initialization Failed", "PreInitialization Failed." NEWLINE "For more information, please see the log.");
            LOG("Qv2ray initialization failed:", reason);
        }
        return reason;
    }

#ifndef Q_OS_WIN
    signal(SIGUSR1, [](int) { ConnectionManager->RestartConnection(); });
    signal(SIGUSR2, [](int) { ConnectionManager->StopConnection(); });
#endif

    app.RunQv2ray();
    const auto reason = app.GetExitReason();
    if (reason == EXIT_NEW_VERSION_TRIGGER)
    {
        LOG("Starting new version of Qv2ray: " + app.StartupArguments._qvNewVersionPath);
        QProcess::startDetached(app.StartupArguments._qvNewVersionPath, {});
    }
    return reason;
}
