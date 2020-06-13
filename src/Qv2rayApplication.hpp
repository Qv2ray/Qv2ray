#pragma once

#include "3rdparty/SingleApplication/singleapplication.h"

namespace Qv2ray
{
    class Qv2rayApplication : public SingleApplication
    {
        Q_OBJECT
      public:
        explicit Qv2rayApplication(int &argc, char *argv[]);
        bool SetupQv2ray();
        bool InitilizeConfigurations();
        static void SetHiDPIEnableState(bool enabled);
        bool CheckSettingsPathAvailability(const QString &_path, bool checkExistingConfig);

      private:
        bool initilized = false;
    };
} // namespace Qv2ray

#define qvApp (static_cast<Qv2ray::Qv2rayApplication *>(QCoreApplication::instance()))
