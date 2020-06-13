#pragma once

#include "libs/QJsonStruct/QJsonStruct.hpp"

#include <SingleApplication>

namespace Qv2ray
{
    struct Qv2rayInterProcessArguments
    {
        enum Argument
        {
            SHOWWINDOW = 0,
            PROTOCOLHANDLER = 1,
            EXITQV2RAY = 2,
        };
        Argument argument;
        QString data;

        JSONSTRUCT_REGISTER(Qv2rayInterProcessArguments, F(argument, data))
    };
    class Qv2rayApplication : public SingleApplication
    {
        Q_OBJECT
      public:
        explicit Qv2rayApplication(int &argc, char *argv[]);
        bool SetupQv2ray();
        bool InitilizeConfigurations();
        static void SetHiDPIEnableState(bool enabled);
        bool CheckSettingsPathAvailability(const QString &_path, bool checkExistingConfig);

      private slots:
        void onMessageReceived(quint32 clientID, QByteArray msg);

      private:
        bool initilized = false;
    };
} // namespace Qv2ray

#define qvApp (static_cast<Qv2ray::Qv2rayApplication *>(QCoreApplication::instance()))
