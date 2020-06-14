#pragma once

#include "libs/QJsonStruct/QJsonStruct.hpp"

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

        JSONSTRUCT_REGISTER(Qv2rayProcessArguments, F(arguments, version, data, links, fullArgs))
    };

    inline Qv2rayProcessArguments Qv2rayProcessArgument;

    class Qv2rayApplication : public SingleApplication
    {
        enum commandline_status
        {
            ERROR,
            QUIT,
            CONTINUE
        };
        Q_OBJECT
      public:
        static bool PreInitilize(int argc, char *argv[]);
        explicit Qv2rayApplication(int &argc, char *argv[]);
        bool SetupQv2ray();
        bool FindAndCreateInitialConfiguration();
        bool LoadConfiguration();
        void InitilizeGlobalVariables();
        int RunQv2ray();
        void DeallocateGlobalVariables();

      private slots:
        void onMessageReceived(quint32 clientID, QByteArray msg);

      private:
        MainWindow *mainWindow;
        static commandline_status ParseCommandLine(QString *errorMessage);
        bool initilized = false;
    };
} // namespace Qv2ray

#define qvApp (static_cast<Qv2ray::Qv2rayApplication *>(QCoreApplication::instance()))
