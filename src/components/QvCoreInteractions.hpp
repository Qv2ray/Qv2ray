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
                explicit ConnectionInstance();
                ~ConnectionInstance() override;
                //
                // Speed
                long getTagSpeedUp(const QString &tag);
                long getTagSpeedDown(const QString &tag);
                long getTagDataUp(const QString &tag);
                long getTagDataDown(const QString &tag);
                long getAllDataUp();
                long getAllDataDown();
                long getAllSpeedUp();
                long getAllSpeedDown();
                //
                bool StartConnection(CONFIGROOT root, bool useAPI, int apiPort);
                void StopConnection();
                QvInstanceStatus ConnectionStatus;
                //
                static bool ValidateConfig(const QString &path);

            signals:
                void onProcessOutputReadyRead(QString);

            private:
                void timerEvent(QTimerEvent *event) override;
                QStringList inboundTags;
                bool enableAPI;
                int apiTimerId;
                int apiPort;
                //
                int apiFailedCounter;
                long CallStatsAPIByName(QString name);
                QProcess *vProcess;
                //
                QMap<QString, long> transferData;
                QMap<QString, long> transferSpeed;
                //
                std::shared_ptr<::grpc::Channel> Channel;
                std::unique_ptr<::v2ray::core::app::stats::command::StatsService::Stub> Stub;
        };
    }
}

using namespace Qv2ray::QvCoreInteration;

#endif // VINTERACT_H
