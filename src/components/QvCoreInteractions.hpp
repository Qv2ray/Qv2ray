#ifndef VINTERACT_H
#define VINTERACT_H
#include <QProcess>
#include <QString>
#include "Qv2rayBase.hpp"
#include <grpc++/grpc++.h>
#include "QvUtils.hpp"
#include "v2ray_api_commands.pb.h"
#include "v2ray_api_commands.grpc.pb.h"

namespace Qv2ray
{
    namespace QvCoreInteration
    {
        enum QvInstanceStatus {
            STOPPED,
            STARTING,
            STARTED
        };

        class ConnectionInstance : public QObject
        {
                Q_OBJECT
            public:
                explicit ConnectionInstance(QWidget *parent = nullptr);
                void SetAPIPort(int port);
                //
                long getTagTotalDownlink(const QString &tag);
                long getTagTotalUplink(const QString &tag);
                long getTagLastDownlink(const QString &tag);
                long getTagLastUplink(const QString &tag);
                //
                bool StartV2rayCore();
                void StopV2rayCore();
                QvInstanceStatus ConnectionStatus;
                QString ReadProcessOutput();
                //
                static bool ValidateConfig(const QString &path);
                static bool ValidateKernal();

                ~ConnectionInstance();
                QMap<QString, long> totalDataTransfered;
                QMap<QString, long> dataTransferSpeed;
            private:
                int apiFailedCounter;
                long CallStatsAPIByName(QString name);
                QProcess *vProcess;
                std::shared_ptr<::grpc::Channel> Channel;
                std::unique_ptr<::v2ray::core::app::stats::command::StatsService::Stub> Stub;
                int port;
        };
    }
}

using namespace Qv2ray::QvCoreInteration;

#endif // VINTERACT_H
