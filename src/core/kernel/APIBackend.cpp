#include "APIBackend.hpp"

#ifdef WITH_LIB_GRPCPP
using namespace v2ray::core::app::stats::command;
using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
#else
#include "libs/libqvb/build/libqvb.h"
#endif

namespace Qv2ray::core::kernel::api
{
    // To all contributors:
    //

    // --- CONSTRUCTOR ---
    APIWorkder::APIWorkder()
    {
        thread = new QThread();
        this->moveToThread(thread);
        DEBUG(MODULE_VCORE, "API Worker initialised.")
        connect(this, SIGNAL(error(QString)), this, SLOT(errorString(QString)));
        connect(thread, SIGNAL(started()), this, SLOT(process()));
        connect(thread, &QThread::finished, []() {
            LOG(MODULE_VCORE, "API thread stopped")
        });
        started = true;
        thread->start();
        DEBUG(MODULE_VCORE, "API Worker started.")
    }

    void APIWorkder::StartAPI(QStringList tags)
    {
        // Config API
        apiFailedCounter = 0;
        inboundTags = tags;
        running = true;
    }

    void APIWorkder::StopAPI()
    {
        running = false;
    }

    // --- DECONSTRUCTOR ---
    APIWorkder::~APIWorkder()
    {
        StopAPI();
        // Set started signal to false and wait for API thread to stop.
        started = false;
        thread->wait();

        // Although thread shouldnot be null, we'll add this check to be safe.
        if (thread) {
            delete thread;
        }
    }

    // API Core Operations
    // Start processing data.
    void APIWorkder::process()
    {
        while (started) {
            QThread::msleep(1000);
            bool dialed = false;

            while (running) {
                if (!dialed) {
                    auto channelAddress = "127.0.0.1:" + QString::number(GlobalConfig.apiConfig.statsPort);
#ifdef WITH_LIB_GRPCPP
                    Channel = grpc::CreateChannel(channelAddress.toStdString(), grpc::InsecureChannelCredentials());
                    StatsService service;
                    Stub = service.NewStub(Channel);
#else
                    auto str = Dial(const_cast<char *>(channelAddress.toStdString().c_str()), 10000);
                    LOG(MODULE_VCORE, QString(str))
                    free(str);
#endif
                    dialed = true;
                }

                for (auto tag : inboundTags) {
                    auto valup = CallStatsAPIByName("inbound>>>" + tag + ">>>traffic>>>uplink");
                    auto valdown = CallStatsAPIByName("inbound>>>" + tag + ">>>traffic>>>downlink");

                    if (valup < 0 && valdown < 0) {
                        dialed = false;
                        break;
                    }

                    if (running) {
                        apiFailedCounter = 0;
                        emit OnDataReady(tag, valup, valdown);
                    } else {
                        // If the connection has stopped, just quit.
                        break;
                    }
                }

                QThread::msleep(1000);
            } // end while running
        } // end while started

        thread->exit();
    }

    qint64 APIWorkder::CallStatsAPIByName(QString name)
    {
        if (apiFailedCounter == QV2RAY_API_CALL_FAILEDCHECK_THRESHOLD) {
            LOG(MODULE_VCORE, "API call failure threshold reached, cancelling further API aclls.")
            emit error("Failed to get statistics data, please check if V2ray is running properly");
            apiFailedCounter++;
            return 0;
        } else if (apiFailedCounter > QV2RAY_API_CALL_FAILEDCHECK_THRESHOLD) {
            return 0;
        }

#ifdef WITH_LIB_GRPCPP
        GetStatsRequest request;
        request.set_name(name.toStdString());
        request.set_reset(false);
        GetStatsResponse response;
        ClientContext context;
        Status status = Stub->GetStats(&context, request, &response);

        if (!status.ok()) {
            LOG(VCORE, "API call returns: " + QSTRN(status.error_code()) + " (" + QString::fromStdString(status.error_message()) + ")")
            apiFailedCounter++;
        }

        qint64 data = response.stat().value();
#else
        qint64 data = GetStats(const_cast<char *>(name.toStdString().c_str()), 1000);
#endif

        if (data < 0) {
            LOG(MODULE_VCORE, "API call returns: " + QSTRN(data))
            apiFailedCounter++;
            return 0;
        }

        return data;
    }
}
