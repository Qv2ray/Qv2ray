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
        StatAPIType type;
    };

    typedef QMap<QString, APIConfigObject> QvAPITagProtocolConfig;
    typedef QMap<StatAPIType, QStringList> QvAPIDataTypeConfig;

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
        void OnDataReady(StatAPIType type, const quint64 speedUp, const quint64 speedDown);
        void error(const QString &err);

      private:
        static QvAPITagProtocolConfig GetConfigObject(const QMap<QString, QString> &tagProtocolPair, bool isOutboundStats)
        {
            APIConfigObject o;
        }
        qint64 CallStatsAPIByName(const QString &name);
        QvAPITagProtocolConfig tagProtocolConfig;
        QThread *workThread;
        //
        bool started = false;
        bool running = false;
        uint apiFailedCounter = 0;
#ifndef ANDROID
        std::shared_ptr<::grpc::Channel> Channel;
        std::unique_ptr<::v2ray::core::app::stats::command::StatsService::Stub> Stub;
#endif
    };
} // namespace Qv2ray::core::kernel

using namespace Qv2ray::core::kernel;
