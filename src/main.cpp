#include "Qv2rayApplication.hpp"
#include "common/QvHelpers.hpp"
#include "core/handler/ConfigHandler.hpp"

#include <QFileInfo>
#include <QLocale>
#include <QProcess>
#include <QSslSocket>
#include <csignal>

#ifdef Q_OS_WIN
    #include <Windows.h>
    //
    #include <DbgHelp.h>

QString GetStackTraceImpl_Windows()
{
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
    QString msg;
    //
    for (int i = 0; i < numberOfFrames; i++)
    {
        const auto address = (DWORD64) stack[i];
        SymFromAddr(process, address, NULL, symbol);
        if (SymGetLineFromAddr64(process, address, &displacement, line))
        {
            msg += QString("[%1]: %2 (%3:%4)\r\n").arg(symbol->Address).arg(symbol->Name).arg(line->FileName).arg(line->LineNumber);
        }
        else
        {
            msg += QString("[%1]: %2 SymGetLineFromAddr64[%3]\r\n").arg(symbol->Address).arg(symbol->Name).arg(GetLastError());
        }
    }
    return msg;
}
#endif

void signalHandler(int signum)
{
    std::cout << "Qv2ray: Interrupt signal (" << signum << ") received." << std::endl;
#ifdef Q_OS_WIN
    if (SIGSEGV == signum || SIGFPE == signum)
    {
        std::cout << "Collecting StackTrace" << std::endl;
        const auto msg = GetStackTraceImpl_Windows();
        std::cout << msg.toStdString() << std::endl;
        QDir().mkpath(QV2RAY_CONFIG_DIR + "bugreport/");
        auto filePath = QV2RAY_CONFIG_DIR + "bugreport/QvBugReport_" + QSTRN(system_clock::to_time_t(system_clock::now())) + ".stacktrace";
        StringToFile(msg, filePath);
        std::cout << "Backtrace saved in: " + filePath.toStdString() << std::endl;
        if (qApp)
        {
            qApp->clipboard()->setText(filePath);
            auto message = QObject::tr("Qv2ray has encountered an uncaught exception: ") + NEWLINE +                      //
                           QObject::tr("Please report a bug via Github with the file located here: ") + NEWLINE NEWLINE + //
                           filePath;
            QvMessageBoxWarn(nullptr, "UNCAUGHT EXCEPTION", message);
        }
    }
#endif
    exit(-99);
}

Qv2rayExitCode RunQv2rayApplicationScoped(int argc, char *argv[])
{
    Qv2rayApplication app(argc, argv);

    const auto setupStatus = app.SetupQv2ray();
    switch (setupStatus)
    {
        case Qv2rayApplication::NORMAL: break;
        case Qv2rayApplication::SINGLE_APPLICATION: return QV2RAY_SECONDARY_INSTANCE;
        case Qv2rayApplication::FAILED: return QV2RAY_EARLY_SETUP_FAIL;
    }

    LOG("LICENCE", NEWLINE                                                      //
        "This program comes with ABSOLUTELY NO WARRANTY." NEWLINE               //
        "This is free software, and you are welcome to redistribute it" NEWLINE //
        "under certain conditions." NEWLINE NEWLINE                             //
        "Copyright (c) 2019-2020 Qv2ray Development Group." NEWLINE             //
        "Third-party libraries that have been used in Qv2ray can be found in the About page." NEWLINE)

#ifdef QT_DEBUG
    std::cerr << "WARNING: ================ This is a debug build, many features are not stable enough. ================" << std::endl;
#endif
    //
    // Qv2ray Initialize, find possible config paths and verify them.
    if (!app.FindAndCreateInitialConfiguration())
    {
        LOG(MODULE_INIT, "Cannot find or create initial configuration file.")
        return QV2RAY_CONFIG_PATH_FAIL;
    }
    if (!app.LoadConfiguration())
    {
        LOG(MODULE_INIT, "Cannot load existing configuration file.")
        return QV2RAY_CONFIG_FILE_FAIL;
    }

    // Check OpenSSL version for auto-update and subscriptions
    auto osslReqVersion = QSslSocket::sslLibraryBuildVersionString();
    auto osslCurVersion = QSslSocket::sslLibraryVersionString();
    LOG(MODULE_NETWORK, "Current OpenSSL version: " + osslCurVersion)

    if (!QSslSocket::supportsSsl())
    {
        LOG(MODULE_NETWORK, "Required OpenSSL version: " + osslReqVersion)
        LOG(MODULE_NETWORK, "OpenSSL library MISSING, Quitting.")
        QvMessageBoxWarn(nullptr, QObject::tr("Dependency Missing"),
                         QObject::tr("Cannot find openssl libs") + NEWLINE +
                             QObject::tr("This could be caused by a missing of `openssl` package in your system.") + NEWLINE +
                             QObject::tr("If you are using an AppImage from Github Action, please report a bug.") + NEWLINE + //
                             NEWLINE + QObject::tr("Technical Details") + NEWLINE +                                           //
                             "OSsl.Rq.V=" + osslReqVersion + NEWLINE +                                                        //
                             "OSsl.Cr.V=" + osslCurVersion);
        return QV2RAY_SSL_FAIL;
    }

    app.InitializeGlobalVariables();

#ifndef Q_OS_WIN
    signal(SIGUSR1, [](int) { ConnectionManager->RestartConnection(); });
    signal(SIGUSR2, [](int) { ConnectionManager->StopConnection(); });
#endif
    return app.RunQv2ray();
}

int main(int argc, char *argv[])
{
    // Register signal handlers.
    signal(SIGINT, signalHandler);
    signal(SIGABRT, signalHandler);
    signal(SIGSEGV, signalHandler);
    signal(SIGTERM, signalHandler);
#ifndef Q_OS_WIN
    signal(SIGHUP, signalHandler);
    signal(SIGKILL, signalHandler);
#endif
    //
    // This line must be called before any other ones, since we are using these
    // values to identify instances.
    QApplication::setApplicationVersion(QV2RAY_VERSION_STRING);
    //
#ifdef QT_DEBUG
    QApplication::setApplicationName("qv2ray_debug");
    QApplication::setApplicationDisplayName("Qv2ray - " + QObject::tr("Debug version"));
#else
    QApplication::setApplicationName("qv2ray");
    QApplication::setApplicationDisplayName("Qv2ray");
#endif
    //
    // parse the command line before starting as a Qt application
    if (!Qv2rayApplication::PreInitialize(argc, argv))
        return QV2RAY_PRE_INITIALIZE_FAIL;
    const auto rcode = RunQv2rayApplicationScoped(argc, argv);
    if (rcode == QV2RAY_NEW_VERSION)
    {
        LOG(MODULE_INIT, "Starting new version of Qv2ray: " + Qv2rayProcessArgument._qvNewVersionPath)
        QProcess::startDetached(Qv2rayProcessArgument._qvNewVersionPath, {});
    }
    return rcode;
}
