#include "APIBackend.hpp"

#include "v2ray_api.pb.h"
using namespace v2ray::core::app::stats::command;
using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

#define QV_MODULE_NAME "gRPCBackend"

namespace Qv2ray::core::kernel
{
    constexpr auto Qv2ray_GRPC_ERROR_RETCODE = -1;
    static QvAPIDataTypeConfig DefaultInboundAPIConfig{ { API_INBOUND, { "dokodemo-door", "http", "socks" } } };
    static QvAPIDataTypeConfig DefaultOutboundAPIConfig{ { API_OUTBOUND_PROXY,
                                                           { "dns", "http", "mtproto", "shadowsocks", "socks", "vmess", "vless", "trojan" } },
                                                         { API_OUTBOUND_DIRECT, { "freedom" } },
                                                         { API_OUTBOUND_BLACKHOLE, { "blackhole" } } };

    APIWorker::APIWorker()
    {
        workThread = new QThread();
        this->moveToThread(workThread);
        DEBUG("API Worker initialised.");
        connect(workThread, &QThread::started, this, &APIWorker::process);
        connect(workThread, &QThread::finished, [] { LOG("API thread stopped"); });
        started = true;
        workThread->start();
    }

    void APIWorker::StartAPI(const QMap<bool, QMap<QString, QString>> &tagProtocolPair)
    {
        // Config API
        tagProtocolConfig.clear();
        for (const auto &key : tagProtocolPair.keys())
        {
            const auto config = key ? DefaultOutboundAPIConfig : DefaultInboundAPIConfig;
            for (const auto &[tag, protocol] : tagProtocolPair[key].toStdMap())
            {
                for (const auto &[type, protocols] : config)
                {
                    if (protocols.contains(protocol))
                        tagProtocolConfig[tag] = { protocol, type };
                }
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
        DEBUG("API Worker started.");
        while (started)
        {
            QThread::msleep(1000);
            bool dialed = false;
            int apiFailCounter = 0;

            while (running)
            {
                if (!dialed)
                {
                    const auto channelAddress = "127.0.0.1:" + QString::number(GlobalConfig.kernelConfig.statsPort);
                    LOG("gRPC Version: " + QString::fromStdString(grpc::Version()));
                    grpc_channel = grpc::CreateChannel(channelAddress.toStdString(), grpc::InsecureChannelCredentials());
                    v2ray::core::app::stats::command::StatsService service;
                    stats_service_stub = service.NewStub(grpc_channel);
                    dialed = true;
                }
                if (apiFailCounter == QV2RAY_API_CALL_FAILEDCHECK_THRESHOLD)
                {
                    LOG("API call failure threshold reached, cancelling further API aclls.");
                    emit OnAPIErrored(tr("Failed to get statistics data, please check if V2Ray is running properly"));
                    apiFailCounter++;
                    QThread::msleep(1000);
                    continue;
                }
                else if (apiFailCounter > QV2RAY_API_CALL_FAILEDCHECK_THRESHOLD)
                {
                    // Ignored future requests.
                    QThread::msleep(1000);
                    continue;
                }

                QMap<StatisticsType, QvStatsSpeed> statsResult;
                bool hasError = false;
                for (const auto &[tag, config] : tagProtocolConfig)
                {
                    const QString prefix = config.type == API_INBOUND ? "inbound" : "outbound";
                    const auto value_up = CallStatsAPIByName(prefix % ">>>" % tag % ">>>traffic>>>uplink");
                    const auto value_down = CallStatsAPIByName(prefix % ">>>" % tag % ">>>traffic>>>downlink");
                    hasError = hasError || value_up == Qv2ray_GRPC_ERROR_RETCODE || value_down == Qv2ray_GRPC_ERROR_RETCODE;
                    statsResult[config.type].first += std::max(value_up, 0LL);
                    statsResult[config.type].second += std::max(value_down, 0LL);
                }
                apiFailCounter = hasError ? apiFailCounter + 1 : 0;
                // Changed: Removed isrunning check here
                emit onAPIDataReady(statsResult);
                QThread::msleep(1000);
            } // end while running
        }     // end while started

        workThread->exit();
    }

    qint64 APIWorker::CallStatsAPIByName(const QString &name)
    {
        ClientContext context;
        GetStatsRequest request;
        GetStatsResponse response;
        request.set_name(name.toStdString());
        request.set_reset(true);

        const auto status = stats_service_stub->GetStats(&context, request, &response);
        if (!status.ok())
        {
            LOG("API call returns: " + QSTRN(status.error_code()) + " (" + QString::fromStdString(status.error_message()) + ")");
            return Qv2ray_GRPC_ERROR_RETCODE;
        }
        else
        {
            return response.stat().value();
        }
    }
} // namespace Qv2ray::core::kernel
