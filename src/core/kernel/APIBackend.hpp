#pragma once
#include "base/Qv2rayBase.hpp"

// Check 10 times before telling user that API has failed.
constexpr auto QV2RAY_API_CALL_FAILEDCHECK_THRESHOLD = 30;

namespace Qv2ray::core::kernel
{
    struct APIConfigObject
    {
        QString protocol;
        StatisticsType type;
    };

    typedef std::map<QString, APIConfigObject> QvAPITagProtocolConfig;
    typedef std::map<StatisticsType, QStringList> QvAPIDataTypeConfig;

    class APIWorker : public QObject
    {
        Q_OBJECT

      public:
        APIWorker();
        ~APIWorker();
        void StartAPI(const QMap<bool, QMap<QString, QString>> &tagProtocolPair);
        void StopAPI();

      signals:
        void onAPIDataReady(const QMap<StatisticsType, QvStatsSpeed> &data);
        void OnAPIErrored(const QString &err);

      private slots:
        void process();

      private:
        QvAPITagProtocolConfig tagProtocolConfig;
        QThread *workThread;
        //
        bool started = false;
        bool running = false;
    };
} // namespace Qv2ray::core::kernel

using namespace Qv2ray::core::kernel;
