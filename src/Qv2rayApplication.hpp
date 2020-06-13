#pragma once

#include "libs/QJsonStruct/QJsonStruct.hpp"

#include <SingleApplication>

namespace Qv2ray
{
    struct Qv2rayProcessArguments
    {
        enum Argument
        {
            NORMAL = 0,
            QV2RAY_LINK = 1,
            EXIT = 2
        };
        Argument argument;
        QString version;
        QString path;
        QString data;

        JSONSTRUCT_REGISTER(Qv2rayProcessArguments, F(argument, version, path, data))
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
        //
        bool SetupQv2ray();
        bool InitilizeConfigurations();
        bool CheckSettingsPathAvailability(const QString &_path, bool checkExistingConfig);

      private slots:
        void onMessageReceived(quint32 clientID, QByteArray msg);

      private:
        static commandline_status ParseCommandLine(QString *errorMessage);
        bool initilized = false;
    };
} // namespace Qv2ray

#define qvApp (static_cast<Qv2ray::Qv2rayApplication *>(QCoreApplication::instance()))
