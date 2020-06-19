#pragma once

#include "libs/QJsonStruct/QJsonStruct.hpp"

#include <QSystemTrayIcon>
#include <SingleApplication>

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
        int _exitCode;
        QString _qvNewVersionPath;
        JSONSTRUCT_REGISTER(Qv2rayProcessArguments, F(arguments, version, data, links, fullArgs))
    };

    inline Qv2rayProcessArguments Qv2rayProcessArgument;

    class Qv2rayApplication : public SingleApplication
    {
        Q_OBJECT
        enum commandline_status
        {
            QV2RAY_ERROR,
            QV2RAY_QUIT,
            QV2RAY_CONTINUE
        };

      public:
        static bool PreInitilize(int argc, char *argv[]);
        explicit Qv2rayApplication(int &argc, char *argv[]);
        bool SetupQv2ray();
        bool FindAndCreateInitialConfiguration();
        bool LoadConfiguration();
        void InitilizeGlobalVariables();
        int RunQv2ray();

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
        void onMessageReceived(quint32 clientID, QByteArray msg);

      private:
        QSystemTrayIcon *hTray;
        MainWindow *mainWindow;
        static commandline_status ParseCommandLine(QString *errorMessage, const QStringList &args);
        bool initilized = false;
    };
} // namespace Qv2ray

#define qvApp (static_cast<Qv2ray::Qv2rayApplication *>(QCoreApplication::instance()))
#define qvAppTrayIcon (*qvApp->GetTrayIcon())
