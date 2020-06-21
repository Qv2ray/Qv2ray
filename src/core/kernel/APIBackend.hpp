#pragma once
#include "base/Qv2rayBase.hpp"
#ifndef ANDROID
    #ifndef BACKEND_LIBQVB
        #include "v2ray_api.grpc.pb.h"

        #include <grpc++/grpc++.h>
    #endif
#endif
// Check 10 times before telling user that API has failed.
constexpr auto QV2RAY_API_CALL_FAILEDCHECK_THRESHOLD = 60;

namespace Qv2ray::core::kernel
{
    class APIWorker : public QObject
    {
        Q_OBJECT

      public:
        APIWorker();
        ~APIWorker();
        void StartAPI(const QStringList &tags);
        void StopAPI();

      public slots:
        void process();

      signals:
        void OnDataReady(const quint64 speedUp, const quint64 speedDown);
        void error(const QString &err);

      private:
        qint64 CallStatsAPIByName(const QString &name);
        QStringList inboundTags;
        QThread *thread;
        //
        bool started = false;
        bool running = false;
        uint apiFailedCounter = 0;
#if !defined(BACKEND_LIBQVB) && !defined(ANDROID)
        std::shared_ptr<::grpc::Channel> Channel;
        std::unique_ptr<::v2ray::core::app::stats::command::StatsService::Stub> Stub;
#endif
    };
} // namespace Qv2ray::core::kernel

using namespace Qv2ray::core::kernel;
