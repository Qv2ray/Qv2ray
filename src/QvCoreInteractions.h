#ifndef VINTERACT_H
#define VINTERACT_H
#include <QProcess>
#include <QString>
#include "QvGUIConfigObjects.h"

namespace Qv2ray
{
    enum V2RAY_INSTANCE_STARTUP_STATUS {
        STOPPED,
        STARTING,
        STARTED
    };

    class Qv2Instance
    {
        public:
            explicit Qv2Instance(QWidget *parent = nullptr);

            bool Start();
            void Stop();

            QString readOutput();
            V2RAY_INSTANCE_STARTUP_STATUS Status;
            static bool VerifyVConfigFile(QString path);
            static bool ValidateV2rayCoreExe();

            ~Qv2Instance();
        private:
            Qv2Config _config;
            QProcess *vProcess;
    };
}

#endif // VINTERACT_H
