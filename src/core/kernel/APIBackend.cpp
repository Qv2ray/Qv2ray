#include "APIBackend.hpp"

#ifndef ANDROID
    #ifndef BACKEND_LIBQVB
        #include "v2ray_api.pb.h"
using namespace v2ray::core::app::stats::command;
using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
    #else
        #include "libs/libqvb/build/libqvb.h"
    #endif
#endif

namespace Qv2ray::core::kernel
{
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

    // --- CONSTRUCTOR ---
    APIWorker::APIWorker()
    {
        thread = new QThread();
        this->moveToThread(thread);
        DEBUG(MODULE_VCORE, "API Worker initialised.")
        // connect(this, SIGNAL(error(QString)), this, SLOT(errorString(QString)));
        connect(thread, SIGNAL(started()), this, SLOT(process()));
        connect(thread, &QThread::finished, [] { LOG(MODULE_VCORE, "API thread stopped") });
        started = true;
        thread->start();
        DEBUG(MODULE_VCORE, "API Worker started.")
    }

    void APIWorker::StartAPI(const QList<QvAPIConfig> &tags, bool useOutboundStats)
    {
        // Config API
        apiFailedCounter = 0;
        inboundTags = tags;
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
        thread->wait();
        delete thread;
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
    #ifdef BACKEND_LIBQVB
                    auto str = Dial(const_cast<char *>(channelAddress.toStdString().c_str()), 10000);
                    LOG(MODULE_VCORE, QString(str))
                    LOG(MODULE_VCORE, "Currently, libqvb does not support speed reporting, your stats might go wrong.")
                    free(str);
    #else
                    LOG(MODULE_VCORE, "gRPC Version: " + QString::fromStdString(grpc::Version()))
                    Channel = grpc::CreateChannel(channelAddress.toStdString(), grpc::InsecureChannelCredentials());
                    v2ray::core::app::stats::command::StatsService service;
                    Stub = service.NewStub(Channel);
    #endif
#endif
                    dialed = true;
                }

                qint64 value_up = 0;
                qint64 value_down = 0;

                for (auto tag : inboundTags)
                {
                    value_up += CallStatsAPIByName("inbound>>>"
                                                   "SHIT"
                                                   ">>>traffic>>>uplink");
                    value_down += CallStatsAPIByName("inbound>>>"
                                                     "SHIT"
                                                     ">>>traffic>>>downlink");
                }

                if (value_up < 0 || value_down < 0)
                {
                    dialed = false;
                    break;
                }

                if (running)
                {
                    emit OnDataReady(API_INBOUND, value_up, value_down);
                }

                QThread::msleep(1000);
            } // end while running
        }     // end while started

        thread->exit();
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
    #ifndef BACKEND_LIBQVB
        GetStatsRequest request;
        request.set_name(name.toStdString());
        request.set_reset(true);
        GetStatsResponse response;
        ClientContext context;
        Status status = Stub->GetStats(&context, request, &response);

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
    #else
        qint64 data = GetStats(const_cast<char *>(name.toStdString().c_str()), 1000);
    #endif

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
