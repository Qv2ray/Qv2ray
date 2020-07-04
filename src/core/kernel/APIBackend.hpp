#pragma once
#include "base/Qv2rayBase.hpp"
#ifndef ANDROID
    #include "v2ray_api.grpc.pb.h"

    #include <grpc++/grpc++.h>
#endif

// Check 10 times before telling user that API has failed.
constexpr auto QV2RAY_API_CALL_FAILEDCHECK_THRESHOLD = 60;

namespace Qv2ray::core::kernel
{
    struct APIConfigObject
    {
        QString protocol;
        Qv2rayStatisticsType type;
    };

    typedef std::map<QString, APIConfigObject> QvAPITagProtocolConfig;
    typedef std::map<Qv2rayStatisticsType, QStringList> QvAPIDataTypeConfig;

    class APIWorker : public QObject
    {
        Q_OBJECT

      public:
        APIWorker();
        ~APIWorker();
        void StartAPI(const QMap<QString, QString> &tagProtocolPair, bool useOutboundStats);

        void StopAPI();

      public slots:
        void process();

      signals:
        void OnDataReady(const std::map<Qv2rayStatisticsType, std::pair<long, long>> &data);
        void error(const QString &err);

      private:
        qint64 CallStatsAPIByName(const QString &name);
        QvAPITagProtocolConfig tagProtocolConfig;
        QThread *workThread;
        //
        bool started = false;
        bool running = false;
        int apiFailedCounter = 0;
#ifndef ANDROID
        std::shared_ptr<::grpc::Channel> grpc_channel;
        std::unique_ptr<::v2ray::core::app::stats::command::StatsService::Stub> stats_service_stub;
#endif
    };
} // namespace Qv2ray::core::kernel

using namespace Qv2ray::core::kernel;
