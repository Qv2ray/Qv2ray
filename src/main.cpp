#include "Qv2rayApplication.hpp"

#include <QProcess>
#include <csignal>
#include <iostream>

#ifndef Q_OS_WIN
#include <unistd.h>
#endif

int globalArgc;
char **globalArgv;
void init_msgbox(const QString &title, const QString &text);
void signal_handler(int signum);

void QtMessageHandle(QtMsgType, const QMessageLogContext &, const QString &str)
{
    std::cout << str.toStdString() << std::endl;
}

int main(int argc, char *argv[])
{
    qInstallMessageHandler(QtMessageHandle);
    globalArgc = argc;
    globalArgv = argv;
    // Register signal handlers.
    signal(SIGABRT, signal_handler);
    signal(SIGSEGV, signal_handler);
    signal(SIGTERM, signal_handler);
#ifndef Q_OS_WIN
    signal(SIGHUP, signal_handler);
    signal(SIGKILL, signal_handler);
#endif
    //
    // This line must be called before any other ones, since we are using these
    // values to identify instances.
    QCoreApplication::setApplicationVersion(QString::fromUtf8(QV2RAY_VERSION_STRING));
    QApplication::setApplicationDisplayName(u"Qv2ray"_qs);

#ifdef QT_DEBUG
    QCoreApplication::setApplicationName(u"qv2ray_debug"_qs);
    std::cerr << "WARNING: ================ This is a debug build, many features are not stable enough. ================" << std::endl;
#else
    QCoreApplication::setApplicationName("qv2ray");
#endif

    Qv2rayApplication app(argc, argv);
    if (!app.Initialize())
    {
        const auto reason = app.GetExitReason();
        if (reason == EXIT_INITIALIZATION_FAILED)
        {
            init_msgbox(u"Qv2ray Initialization Failed"_qs, "PreInitialization Failed." NEWLINE "For more information, please see the log.");
            qInfo() << "Qv2ray initialization failed:" << reason;
        }
        return reason;
    }

    app.RunQv2ray();

    const auto reason = app.GetExitReason();
    if (reason == EXIT_NEW_VERSION_TRIGGER)
    {
        qInfo() << "Starting new version of Qv2ray:" << app.StartupArguments._qvNewVersionPath;
        QProcess::startDetached(app.StartupArguments._qvNewVersionPath, {});
    }
    return reason;
}

void init_msgbox(const QString &title, const QString &text)
{
    if (qApp)
    {
        QMessageBox::warning(nullptr, title, text);
    }
    else
    {
        QApplication p(globalArgc, globalArgv);
        QMessageBox::warning(nullptr, title, text);
    }
}

void signal_handler(int signum)
{
    std::cout << "Signal: " << signum << std::endl;
#ifndef Q_OS_WIN
    if (signum == SIGTRAP)
    {
        exit(-99);
        return;
    }
#endif
    if (signum == SIGTERM)
    {
        if (qApp)
            qApp->exit();
        return;
    }
#if defined Q_OS_WIN || defined QT_DEBUG
    exit(-99);
#else
    kill(getpid(), SIGTRAP);
#endif
}
