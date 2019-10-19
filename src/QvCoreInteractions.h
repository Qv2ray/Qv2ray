#ifndef VINTERACT_H
#define VINTERACT_H
#include <QProcess>
#include <QString>
#include "Qv2rayBase.h"
#include <grpc++/grpc++.h>
#include "QvUtils.h"
#include "v2ray_api_commands.pb.h"
#include "v2ray_api_commands.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using namespace std;
using namespace v2ray::core::app::stats::command;

namespace Qv2ray
{
    namespace QvInteration
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
                void SetAPIPort(int port);
                long getTagTotalDownlink(QString tag);
                long getTagTotalUplink(QString tag);
                long getTagLastDownlink(QString tag);
                long getTagLastUplink(QString tag);
                bool StartVCore();
                void StopVCore();
                V2RAY_INSTANCE_STARTUP_STATUS VCoreStatus;
                static bool ValidateConfig(const QString *path);
                static bool ValidateKernal();
                QString ReadProcessOutput();

                ~Qv2Instance();
                QMap<QString, long> totalDataTransfered;
                QMap<QString, long> dataTransferSpeed;
            private:
                long CallStatsAPIByName(QString name);
                QProcess *vProcess;
                std::shared_ptr<::grpc::Channel> Channel;
                std::unique_ptr<StatsService::Stub> Stub;
                int port;
        };
    }
}

using namespace Qv2ray::QvInteration;

#endif // VINTERACT_H
