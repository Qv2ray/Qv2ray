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
    enum QvAPIType
    {
        API_INBOUND,
        API_OUTBOUND_PROXY,
        API_OUTBOUND_DIRECT,
        API_OUTBOUND_BLACKHOLE,
    };
    struct QvAPIConfig
    {
        QvAPIType type;
        QStringList tags;
    };

    class APIWorker : public QObject
    {
        Q_OBJECT

      public:
        APIWorker();
        ~APIWorker();
        void StartAPI(const QList<QvAPIConfig> &tags, bool useOutboundStats);
        static QList<QvAPIConfig> GetDefaultOutboundAPIConfig()
        {
            return { { API_OUTBOUND_PROXY, { "dns", "http", "mtproto", "shadowsocks", "socks", "vmess" } },
                     { API_OUTBOUND_DIRECT, { "freedom" } },
                     { API_OUTBOUND_BLACKHOLE, { "blackhole" } } };
        }
        void StopAPI();

      public slots:
        void process();

      signals:
        void OnDataReady(QvAPIType type, const quint64 speedUp, const quint64 speedDown);
        void error(const QString &err);

      private:
        qint64 CallStatsAPIByName(const QString &name);
        QList<QvAPIConfig> inboundTags;
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
