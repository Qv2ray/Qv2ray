#include "V2RayAPIStats.hpp"

#include "V2RayCorePluginTemplate.hpp"

#include <QStringBuilder>
#include <QThread>

#ifndef QV2RAY_NO_GRPC
using namespace v2ray::core::app::stats::command;
using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
#else
#include "api.pb.h"
#include "pb_decode.h"
#include "pb_encode.h"

#include <QCoreApplication>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>
#include <QtEndian>
#include <utility>

namespace V2RayPluginNamespace
{
    struct QByteArrayBuffer
    {
        QByteArray *buffer;
        qsizetype read_size;
    };
    bool encode_req_name(pb_ostream_t *stream, const pb_field_t *field, void **arg)
    {
        auto str = (const std::string *) (*arg);
        if (!pb_encode_tag_for_field(stream, field))
            return false;
        return pb_encode_string(stream, reinterpret_cast<const uint8_t *>(str->data()), str->size());
    }
    bool encode_callback(pb_ostream_t *stream, const uint8_t *buf, size_t count)
    {
        auto *qByteArray = static_cast<QByteArray *>(stream->state);
        qByteArray->append(reinterpret_cast<const char *>(buf), count);
        return true;
    }
    bool decode_callback(pb_istream_t *stream, uint8_t *buf, size_t count)
    {
        auto *qByteArray = static_cast<QByteArrayBuffer *>(stream->state);
        if (qByteArray->read_size + count > qByteArray->buffer->size())
        {
            auto copy_size = qByteArray->buffer->size() - qByteArray->read_size;
            memcpy(buf, qByteArray->buffer->data() + qByteArray->read_size, copy_size);
            qByteArray->read_size = qByteArray->buffer->size();
            return false;
        }
        else
        {
            memcpy(buf, qByteArray->buffer->data() + qByteArray->read_size, count);
            qByteArray->read_size += static_cast<qsizetype>(count);
        }
        return true;
    }

    bool decode_string(pb_istream_t *stream, const pb_field_t *field, void **arg)
    {
        // skip string field
        uint8_t buffer[1024];
        if (stream->bytes_left > sizeof(buffer) - 1)
            return false;
        if (!pb_read(stream, buffer, stream->bytes_left))
            return false;
        return true;
    }

    const char *GrpcAcceptEncodingHeader = "grpc-accept-encoding";
    const char *AcceptEncodingHeader = "accept-encoding";
    const char *TEHeader = "te";
    const char *GrpcStatusHeader = "grpc-status";
    const char *GrpcStatusMessage = "grpc-message";
    const int GrpcMessageSizeHeaderSize = 5;
    struct Http2GrpcChannelPrivate
    {

        QUrl url;
        QNetworkAccessManager nm;
        QObject lambdaContext;

        QNetworkReply *post(const QString &method, const QString &service, const QByteArray &args, bool stream = false)
        {
            QUrl callUrl = url;
            callUrl.setPath(u"/"_qs + service + u"/"_qs + method);

            // qDebug() << "Service call url: " << callUrl;
            QNetworkRequest request(callUrl);
            request.setAttribute(QNetworkRequest::CacheSaveControlAttribute, false);
            request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysNetwork);
            request.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String{ "application/grpc" });
            request.setRawHeader(GrpcAcceptEncodingHeader, QByteArray{ "identity,deflate,gzip" });
            request.setRawHeader(AcceptEncodingHeader, QByteArray{ "identity,gzip" });
            request.setRawHeader(TEHeader, QByteArray{ "trailers" });

            request.setAttribute(QNetworkRequest::Http2DirectAttribute, true);

            QByteArray msg(GrpcMessageSizeHeaderSize, '\0');
            *reinterpret_cast<int *>(msg.data() + 1) = qToBigEndian((int) args.size());
            msg += args;
            // qDebug() << "SEND: " << msg.size();

            QNetworkReply *networkReply = nm.post(request, msg);

            if (!stream)
            {
                QTimer::singleShot(6000, networkReply, [networkReply]() { Http2GrpcChannelPrivate::abortNetworkReply(networkReply); });
            }
            return networkReply;
        }

        static void abortNetworkReply(QNetworkReply *networkReply)
        {
            if (networkReply->isRunning())
            {
                networkReply->abort();
            }
            else
            {
                networkReply->deleteLater();
            }
        }

        static QByteArray processReply(QNetworkReply *networkReply, QNetworkReply::NetworkError &statusCode)
        {
            // Check if no network error occured
            if (networkReply->error() != QNetworkReply::NoError)
            {
                statusCode = networkReply->error();
                return {};
            }

            // Check if server answer with error
            auto errCode = networkReply->rawHeader(GrpcStatusHeader).toInt();
            if (errCode != 0)
            {
                qDebug() << "grpc error code:" << errCode << ", error msg:" << QLatin1String(networkReply->rawHeader(GrpcStatusMessage));
                statusCode = QNetworkReply::NetworkError::ProtocolUnknownError;
                return {};
            }
            statusCode = QNetworkReply::NetworkError::NoError;

            // Message size doesn't matter for now
            return networkReply->readAll().mid(GrpcMessageSizeHeaderSize);
        }

        explicit Http2GrpcChannelPrivate(QUrl _url) : url(std::move(_url))
        {
            if (url.scheme().isEmpty())
            {
                url.setScheme(QLatin1String{ "http" });
            }
        }

        QNetworkReply::NetworkError call(const QString &method, const QString &service, const QByteArray &args, QByteArray &ret)
        {
            QEventLoop loop;

            QNetworkReply *networkReply = post(method, service, args);
            QObject::connect(networkReply, &QNetworkReply::finished, &loop, &QEventLoop::quit);

            if (!networkReply->isFinished())
            {
                loop.exec();
            }

            auto grpcStatus = QNetworkReply::NetworkError::ProtocolUnknownError;
            ret = processReply(networkReply, grpcStatus);

            networkReply->deleteLater();
            // qDebug() << __func__ << "RECV: " << ret.toHex() << "grpcStatus" << grpcStatus;
            return grpcStatus;
        }
    };
} // namespace V2RayPluginNamespace
#endif

using namespace V2RayPluginNamespace;

constexpr auto Qv2ray_GRPC_ERROR_RETCODE = -1;
const std::map<StatisticsObject::StatisticsType, QStringList> DefaultOutboundAPIConfig{
    { StatisticsObject::PROXY,
      {
          u"http"_qs,
          u"shadowsocks"_qs,
          u"socks"_qs,
          u"vmess"_qs,
          u"trojan"_qs,
      } },
    { StatisticsObject::DIRECT,
      {
          u"freedom"_qs,
          u"dns"_qs,
      } },
};

APIWorker::APIWorker()
{
    workThread = new QThread();
    this->moveToThread(workThread);
    V2RayCorePluginClass::Log(u"API Worker initialised."_qs);
    connect(workThread, &QThread::started, this, &APIWorker::process);
    connect(workThread, &QThread::finished, [] { V2RayCorePluginClass::Log(u"API thread stopped"_qs); });
    started = true;
    workThread->start();
}

void APIWorker::StartAPI(const QMap<QString, QString> &tagProtocolPair)
{
    // Config API
    tagProtocolConfig.clear();
    for (auto it = tagProtocolPair.constKeyValueBegin(); it != tagProtocolPair.constKeyValueEnd(); it++)
    {
        const auto tag = it->first;
        const auto protocol = it->second;
        for (const auto &[type, protocols] : DefaultOutboundAPIConfig)
        {
            if (protocols.contains(protocol))
                tagProtocolConfig[tag] = type;
        }
    }

    running = true;
}

void APIWorker::StopAPI()
{
    running = false;
}

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
    V2RayCorePluginClass::Log(u"API Worker started."_qs);
    while (started)
    {
        QThread::msleep(1000);
        bool dialed = false;
        int apiFailCounter = 0;

        while (running)
        {
            if (!dialed)
            {
#ifndef QV2RAY_NO_GRPC
                const auto channelAddress = u"127.0.0.1:"_qs + QString::number(V2RayCorePluginClass::PluginInstance->settings.APIPort);
                V2RayCorePluginClass::Log(u"gRPC Version: "_qs + QString::fromStdString(grpc::Version()));
                grpc_channel = grpc::CreateChannel(channelAddress.toStdString(), grpc::InsecureChannelCredentials());
                stats_service_stub = v2ray::core::app::stats::command::StatsService::NewStub(grpc_channel);
                dialed = true;
#else
                const auto channelAddress = u"http://127.0.0.1:"_qs + QString::number(V2RayCorePluginClass::PluginInstance->settings.APIPort);
                grpc_channel = std::make_unique<Http2GrpcChannelPrivate>(QUrl{ channelAddress });
                dialed = true;
#endif
            }
            if (apiFailCounter == QV2RAY_API_CALL_FAILEDCHECK_THRESHOLD)
            {
                V2RayCorePluginClass::Log(u"API call failure threshold reached, cancelling further API aclls."_qs);
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

            StatisticsObject statsResult;
            bool hasError = false;
            for (const auto &[tag, statType] : tagProtocolConfig)
            {
                const auto value_up = CallStatsAPIByName(u"outbound>>>"_qs + tag + u">>>traffic>>>uplink"_qs);
                const auto value_down = CallStatsAPIByName(u"outbound>>>"_qs + tag + u">>>traffic>>>downlink"_qs);
                hasError = hasError || value_up == Qv2ray_GRPC_ERROR_RETCODE || value_down == Qv2ray_GRPC_ERROR_RETCODE;
                if (statType == StatisticsObject::PROXY)
                {
                    statsResult.proxyUp += std::max(value_up, 0LL);
                    statsResult.proxyDown += std::max(value_down, 0LL);
                }
                if (statType == StatisticsObject::DIRECT)
                {
                    statsResult.directUp += std::max(value_up, 0LL);
                    statsResult.directDown += std::max(value_down, 0LL);
                }
            }
            apiFailCounter = hasError ? apiFailCounter + 1 : 0;
            // Changed: Removed isrunning check here
            emit OnAPIDataReady(statsResult);
            QThread::msleep(1000);
        } // end while running
    }     // end while started

    workThread->exit();
}

qint64 APIWorker::CallStatsAPIByName(const QString &name)
{
#ifndef QV2RAY_NO_GRPC
    ClientContext context;
    GetStatsRequest request;
    GetStatsResponse response;
    request.set_name(name.toStdString());
    request.set_reset(true);

    const auto status = stats_service_stub->GetStats(&context, request, &response);
    if (!status.ok())
    {
        V2RayCorePluginClass::Log(u"API call returns:"_qs + QString::number(status.error_code()) + u":"_qs + QString::fromStdString(status.error_message()));
        return Qv2ray_GRPC_ERROR_RETCODE;
    }
    else
    {
        return response.stat().value();
    }
#else
    auto reqName = name.toStdString();
    QByteArray reqMsg;
    v2ray_core_app_stats_command_GetStatsRequest req = v2ray_core_app_stats_command_GetStatsRequest_init_default;
    req.name.funcs.encode = &encode_req_name;
    req.name.arg = &reqName;
    req.reset = true;
    pb_ostream_t stream = { &encode_callback, &reqMsg, SIZE_MAX, 0 };
    auto status = pb_encode(&stream, v2ray_core_app_stats_command_GetStatsRequest_fields, &req);
    // qDebug()<<"req msg hex:"<<reqMsg.toHex();
    if (!status)
    {
        return Qv2ray_GRPC_ERROR_RETCODE;
    }
    QByteArray responseArray;
    auto err = grpc_channel->call(u"GetStats"_qs, u"v2ray.core.app.stats.command.StatsService"_qs, reqMsg, responseArray);
    if (err != QNetworkReply::NetworkError::NoError)
    {
        V2RayCorePluginClass::Log(u"API call returns:"_qs + QString::number(err));
        return Qv2ray_GRPC_ERROR_RETCODE;
    }
    QByteArrayBuffer responseBuffer{ &responseArray, 0 };
    // qDebug()<<"resp array size:"<<responseArray.size();
    pb_istream_t pbIstream = { &decode_callback, &responseBuffer, static_cast<size_t>(responseArray.size()), NULL };
    v2ray_core_app_stats_command_GetStatsResponse response = v2ray_core_app_stats_command_GetStatsResponse_init_default;
    response.stat.name.funcs.decode = &decode_string;
    status = pb_decode(&pbIstream, v2ray_core_app_stats_command_GetStatsResponse_fields, &response);
    if (!status)
    {
        V2RayCorePluginClass::Log(u"API response decode failed"_qs);
        return Qv2ray_GRPC_ERROR_RETCODE;
    }
    return response.stat.value;
#endif
}
