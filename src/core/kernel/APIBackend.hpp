#pragma once
#include "base/Qv2rayBase.hpp"

#ifdef WITH_LIB_GRPCPP
#include <grpc++/grpc++.h>
#include "libs/gen/v2ray_api.pb.h"
#include "libs/gen/v2ray_api.grpc.pb.h"
#endif

// Check 10 times before telling user that API has failed.
#define QV2RAY_API_CALL_FAILEDCHECK_THRESHOLD 10

namespace Qv2ray::core::kernel::api
{
    class APIWorkder : public QObject
    {
            Q_OBJECT

        public:
            APIWorkder();
            ~APIWorkder();
            void StartAPI(QStringList tags);
            void StopAPI();

        public slots:
            void process();

        signals:
            void OnDataReady(QString tag, long dataUp, long dataDown);
            void error(QString err);

        private:
            long CallStatsAPIByName(QString name);
            QStringList inboundTags;
            QThread *thread;
            //
            bool started = false;
            bool running = false;
            int apiFailedCounter;
            //
#ifdef WITH_LIB_GRPCPP
            std::shared_ptr<::grpc::Channel> Channel;
            std::unique_ptr<::v2ray::core::app::stats::command::StatsService::Stub> Stub;
#endif
    };


}

using namespace Qv2ray::core::kernel::api;
