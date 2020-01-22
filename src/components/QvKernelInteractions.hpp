#ifndef VINTERACT_H
#define VINTERACT_H
#include <QProcess>
#include "QvUtils.hpp"
#ifdef WITH_LIB_GRPCPP
#include <grpc++/grpc++.h>
#include "v2ray_api_commands.pb.h"
#include "v2ray_api_commands.grpc.pb.h"
#endif

namespace Qv2ray
{
    namespace QvKernelInterations
    {
        class V2rayKernelInstance : public QObject
        {
                Q_OBJECT
            public:
                explicit V2rayKernelInstance();
                ~V2rayKernelInstance() override;
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
                bool StartConnection(CONFIGROOT root);
                void StopConnection();
                bool KernelStarted = false;
                //
                static bool ValidateConfig(const QString &path);
                static bool ValidateKernel(const QString &vCorePath, const QString &vAssetsPath, QString *message);

            signals:
                void onProcessErrored();
                void onProcessOutputReadyRead(QString);

            private:
                void timerEvent(QTimerEvent *event) override;
                QStringList inboundTags;
                int apiTimerId = -1;
                int apiPort;
                //
                int apiFailedCounter;
                long CallStatsAPIByName(QString name);
                QProcess *vProcess;
                //
                QMap<QString, long> transferData;
                QMap<QString, long> transferSpeed;
                //
#ifdef WITH_LIB_GRPCPP
                std::shared_ptr<::grpc::Channel> Channel;
                std::unique_ptr<::v2ray::core::app::stats::command::StatsService::Stub> Stub;
#endif
        };
    }
}

using namespace Qv2ray::QvKernelInterations;

#endif // VINTERACT_H
