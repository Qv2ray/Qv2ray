#include "HTTPRequestHelper.hpp"

#include "base/Qv2rayBase.hpp"

#include <QByteArray>
#include <QNetworkProxy>

namespace Qv2ray::common
{
    QvHttpRequestHelper::QvHttpRequestHelper(QObject *parent) : QObject(parent), reply()
    {
    }

    QvHttpRequestHelper::~QvHttpRequestHelper()
    {
        accessManager.disconnect();
    }

    void QvHttpRequestHelper::setHeader(const QByteArray &key, const QByteArray &value)
    {
        DEBUG(MODULE_NETWORK, "Adding HTTP request header: " + key + ":" + value)
        request.setRawHeader(key, value);
    }

    QByteArray QvHttpRequestHelper::Get(const QString &url, bool useProxy)
    {
        request.setUrl({ url });
        if (useProxy)
        {
            auto p = GlobalConfig.networkConfig.useCustomProxy ?
                         QNetworkProxy{
                             GlobalConfig.networkConfig.type == "http" ? QNetworkProxy::HttpProxy : QNetworkProxy::Socks5Proxy, //
                             GlobalConfig.networkConfig.address,                                                                //
                             quint16(GlobalConfig.networkConfig.port)                                                           //
                         } :
                         QNetworkProxyFactory::systemProxyForQuery().first();
            accessManager.setProxy(p);
        }
        else
        {
            LOG(MODULE_NETWORK, "Get without proxy.")
            accessManager.setProxy(QNetworkProxy(QNetworkProxy::ProxyType::NoProxy));
        }

        request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);
        request.setAttribute(QNetworkRequest::HTTP2AllowedAttribute, true);
        request.setHeader(QNetworkRequest::KnownHeaders::UserAgentHeader, GlobalConfig.networkConfig.userAgent);
        reply = accessManager.get(request);
        //
        QEventLoop loop;
        connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();
        //
        // Data or timeout?
        auto data = reply->readAll();
        return data;
    }

    void QvHttpRequestHelper::AsyncGet(const QString &url)
    {
        request.setUrl({ url });
        if (GlobalConfig.networkConfig.useCustomProxy)
        {
            QNetworkProxy p{
                GlobalConfig.networkConfig.type == "http" ? QNetworkProxy::HttpProxy : QNetworkProxy::Socks5Proxy, //
                GlobalConfig.networkConfig.address,                                                                //
                quint16(GlobalConfig.networkConfig.port)                                                           //
            };
            accessManager.setProxy(p);
        }
        else
        {
            LOG(MODULE_NETWORK, "Get without proxy.")
            accessManager.setProxy(QNetworkProxy(QNetworkProxy::ProxyType::NoProxy));
        }
        request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);
        request.setAttribute(QNetworkRequest::HTTP2AllowedAttribute, true);
        request.setHeader(QNetworkRequest::KnownHeaders::UserAgentHeader, GlobalConfig.networkConfig.userAgent);
        reply = accessManager.get(request);
        connect(reply, &QNetworkReply::finished, this, &QvHttpRequestHelper::onRequestFinished_p);
        connect(reply, &QNetworkReply::readyRead, this, &QvHttpRequestHelper::onReadyRead_p);
    }

    void QvHttpRequestHelper::onRequestFinished_p()
    {
        if (reply->attribute(QNetworkRequest::HTTP2WasUsedAttribute).toBool())
        {
            DEBUG(MODULE_NETWORK, "HTTP/2 was used.")
        }

        if (reply->error() != QNetworkReply::NoError)
        {
            QString error = QMetaEnum::fromType<QNetworkReply::NetworkError>().key(reply->error());
            LOG(MODULE_NETWORK, "Network request error string: " + error)
            QByteArray empty;
            emit OnRequestFinished(empty);
        }
        else
        {
            emit OnRequestFinished(this->data);
        }
    }

    void QvHttpRequestHelper::onReadyRead_p()
    {
        DEBUG(MODULE_NETWORK, "A request is now ready read")
        this->data += reply->readAll();
    }
} // namespace Qv2ray::common
