#ifndef VINTERACT_H
#define VINTERACT_H
#include <QProcess>
#include <QString>
#include "QvConfigObjects.hpp"

namespace Qv2ray
{
    enum V2RAY_INSTANCE_STARTUP_STATUS {
        STOPPED,
        STARTING,
        STARTED
    };

    class v2Instance
    {
        public:
            explicit v2Instance(QWidget *parent = nullptr);

            bool start();
            void stop();

            QString readOutput();
            V2RAY_INSTANCE_STARTUP_STATUS Status;
            static bool checkConfigFile(QString path);
            static bool checkCoreExe();

            ~v2Instance();
        private:
            QvConfigModels::Qv2Config _config;
            QProcess *vProcess;
    };
}

#endif // VINTERACT_H
