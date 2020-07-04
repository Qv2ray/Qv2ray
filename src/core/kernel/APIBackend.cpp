#include "APIBackend.hpp"

#ifndef ANDROID
    #include "v2ray_api.pb.h"
using namespace v2ray::core::app::stats::command;
using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
#endif

namespace Qv2ray::core::kernel
{
    static QvAPIDataTypeConfig DefaultInboundAPIConfig{ { API_INBOUND, { "dokodemo-door", "http", "socks" } } };
    static QvAPIDataTypeConfig DefaultOutboundAPIConfig{ { API_OUTBOUND_PROXY, { "dns", "http", "mtproto", "shadowsocks", "socks", "vmess" } },
                                                         { API_OUTBOUND_DIRECT, { "freedom" } },
                                                         { API_OUTBOUND_BLACKHOLE, { "blackhole" } } };

    // To all contributors:
    //
    // You may feel it difficult to understand this part of API backend.
    // It's been expected that you will take hours to fully understand the tricks and hacks lying deeply in this class.
    //
    // The API Worker runs as a daemon together with Qv2ray, on a single thread.
    // They use a flag, running, to indicate if the API worker should go and fetch the statistics from V2ray Core.
    //
    // The flag, running, will be set to true, immediately after the V2ray core reported that it's been started.
    // and will be set to false right before we stopping V2ray Core.
    //

    APIWorker::APIWorker()
    {
        workThread = new QThread();
        this->moveToThread(workThread);
        DEBUG(MODULE_VCORE, "API Worker initialised.")
        // connect(this, SIGNAL(error(QString)), this, SLOT(errorString(QString)));
        connect(workThread, SIGNAL(started()), this, SLOT(process()));
        connect(workThread, &QThread::finished, [] { LOG(MODULE_VCORE, "API thread stopped") });
        started = true;
        workThread->start();
        DEBUG(MODULE_VCORE, "API Worker started.")
    }

    void APIWorker::StartAPI(const QMap<QString, QString> &tagProtocolPair, bool useOutboundStats)
    {
        // Config API
        apiFailedCounter = 0;
        tagProtocolConfig.clear();
        const auto config = useOutboundStats ? DefaultOutboundAPIConfig : DefaultInboundAPIConfig;

        for (const auto &[tag, protocol] : tagProtocolPair.toStdMap())
        {
            for (const auto &[type, tags] : config)
            {
                if (tags.contains(tag))
                    tagProtocolConfig[tag] = { protocol, type };
            }
        }

        running = true;
    }

    void APIWorker::StopAPI()
    {
        running = false;
    }

    // --- DESTRUCTOR ---
    APIWorker::~APIWorker()
    {
        StopAPI();
        // Set started signal to false and wait for API thread to stop.
        started = false;
        workThread->wait();
        delete workThread;
    }

    // API Core Operations
    // Start processing data.
    void APIWorker::process()
    {
        while (started)
        {
            QThread::msleep(1000);
            bool dialed = false;

            while (running)
            {
                if (!dialed)
                {
                    auto channelAddress = "127.0.0.1:" + QString::number(GlobalConfig.kernelConfig.statsPort);
#ifndef ANDROID
                    LOG(MODULE_VCORE, "gRPC Version: " + QString::fromStdString(grpc::Version()))
                    grpc_channel = grpc::CreateChannel(channelAddress.toStdString(), grpc::InsecureChannelCredentials());
                    v2ray::core::app::stats::command::StatsService service;
                    stats_service_stub = service.NewStub(grpc_channel);
#endif
                    dialed = true;
                }

                std::map<Qv2rayStatisticsType, std::pair<long, long>> statsResult;
                bool hasResult = false;
                for (const auto &[tag, config] : tagProtocolConfig)
                {
                    const QString prefix = config.type == API_INBOUND ? "inbound" : "outbound";
                    const auto value_up = CallStatsAPIByName(prefix + ">>>" + tag + ">>>traffic>>>uplink");
                    const auto value_down = CallStatsAPIByName(prefix + ">>>" + tag + ">>>traffic>>>downlink");
                    hasResult = hasResult || (value_up > 0 && value_down > 0);
                    //
                    statsResult[config.type].first += value_up;
                    statsResult[config.type].second += value_down;
                }

                if (!hasResult)
                {
                    dialed = false;
                    break;
                }

                // Changed: Removed isrunning check here.
                emit OnDataReady(statsResult);

                QThread::msleep(1000);
            } // end while running
        }     // end while started

        workThread->exit();
    }

    qint64 APIWorker::CallStatsAPIByName(const QString &name)
    {
        if (apiFailedCounter == QV2RAY_API_CALL_FAILEDCHECK_THRESHOLD)
        {
            LOG(MODULE_VCORE, "API call failure threshold reached, cancelling further API aclls.")
            emit error("Failed to get statistics data, please check if V2ray is running properly");
            apiFailedCounter++;
            return 0;
        }
        else if (apiFailedCounter > QV2RAY_API_CALL_FAILEDCHECK_THRESHOLD)
        {
            return 0;
        }
#ifndef ANDROID
        GetStatsRequest request;
        request.set_name(name.toStdString());
        request.set_reset(true);
        GetStatsResponse response;
        ClientContext context;
        Status status = stats_service_stub->GetStats(&context, request, &response);

        if (!status.ok())
        {
            LOG(MODULE_VCORE, "API call returns: " + QSTRN(status.error_code()) + " (" + QString::fromStdString(status.error_message()) + ")")
            apiFailedCounter++;
        }
        else
        {
            apiFailedCounter = 0;
        }
        qint64 data = response.stat().value();

        if (data < 0)
        {
            LOG(MODULE_VCORE, "API call returns: " + QSTRN(data))
            apiFailedCounter++;
            return 0;
        }

        return data;
#else
        Q_UNUSED(name)
        return 0;
#endif
    }
} // namespace Qv2ray::core::kernel
