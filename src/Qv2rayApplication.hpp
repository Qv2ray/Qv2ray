#pragma once

#include "base/Qv2rayBaseApplication.hpp"

#include <QSystemTrayIcon>

#ifdef Q_OS_ANDROID
    #define QV2RAY_NO_SINGLEAPPLICATON
#endif

#define QV2RAY_WORKAROUND_MACOS_MEMLOCK 0

#if QV2RAY_WORKAROUND_MACOS_MEMLOCK
    #ifndef QV2RAY_NO_SINGLEAPPLICATION
        #define QV2RAY_NO_SINGLEAPPLICATON
    #endif
#endif

#ifndef QV2RAY_NO_SINGLEAPPLICATON
    #include <SingleApplication>
#endif

class MainWindow;

namespace Qv2ray
{
    struct Qv2rayProcessArguments
    {
        enum Argument
        {
            NORMAL = 0,
            QV2RAY_LINK = 1,
            EXIT = 2,
            RECONNECT = 3,
            DISCONNECT = 4
        };
        QList<Argument> arguments;
        QString version;
        QString data;
        QList<QString> links;
        QList<QString> fullArgs;
        //
        QString _qvNewVersionPath;
        JSONSTRUCT_REGISTER(Qv2rayProcessArguments, F(arguments, version, data, links, fullArgs))
    };

    enum Qv2rayPreInitResult
    {
        PRE_INIT_RESULT_ERROR,
        PRE_INIT_RESULT_QUIT,
        PRE_INIT_RESULT_CONTINUE
    };

    inline Qv2rayProcessArguments Qv2rayProcessArgument;
#ifdef QV2RAY_NO_SINGLEAPPLICATON
    class Qv2rayApplication : public QApplication
#else
    class Qv2rayApplication : public SingleApplication
#endif
    {
        Q_OBJECT

      public:
        enum Qv2raySetupStatus
        {
            NORMAL,
            SINGLE_APPLICATION,
            FAILED
        };
        //
        void QuitApplication(int retCode = 0);
        static Qv2rayPreInitResult PreInitialize(int argc, char **argv);
        explicit Qv2rayApplication(int &argc, char *argv[]);
        Qv2raySetupStatus SetupQv2ray();
        bool FindAndCreateInitialConfiguration();
        bool LoadConfiguration();
        void InitializeGlobalVariables();
        Qv2rayExitCode RunQv2ray();

      public:
        QSystemTrayIcon **GetTrayIcon()
        {
            return &hTray;
        }
        void showMessage(const QString &m, const QIcon &icon, int msecs = 10000)
        {
            hTray->showMessage("Qv2ray", m, icon, msecs);
        }
        void showMessage(const QString &m, QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::Information, int msecs = 10000)
        {
            hTray->showMessage("Qv2ray", m, icon, msecs);
        }

      private slots:
        void aboutToQuitSlot();

      private:
#ifndef QV2RAY_NO_SINGLEAPPLICATON
        void onMessageReceived(quint32 clientID, QByteArray msg);
#endif

        QSystemTrayIcon *hTray;
        MainWindow *mainWindow;
        static Qv2rayPreInitResult ParseCommandLine(QString *errorMessage, const QStringList &args);
        bool initialized = false;
    };
} // namespace Qv2ray

using namespace Qv2ray;

#define qvApp (dynamic_cast<Qv2ray::Qv2rayApplication *>(QCoreApplication::instance()))
#define qvAppTrayIcon (*qvApp->GetTrayIcon())
