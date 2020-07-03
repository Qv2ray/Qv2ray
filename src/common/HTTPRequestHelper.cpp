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

    void QvHttpRequestHelper::setAccessManagerAttributes(QNetworkAccessManager &accessManager)
    {
        switch (GlobalConfig.networkConfig.proxyType)
        {
            case Qv2rayNetworkConfig::QVPROXY_NONE:
            {
                DEBUG(MODULE_NETWORK, "Get without proxy.")
                accessManager.setProxy(QNetworkProxy(QNetworkProxy::ProxyType::NoProxy));
                break;
            }
            case Qv2rayNetworkConfig::QVPROXY_SYSTEM:
            {
                accessManager.setProxy(QNetworkProxyFactory::systemProxyForQuery().first());
                break;
            }
            case Qv2rayNetworkConfig::QVPROXY_CUSTOM:
            {
                QNetworkProxy p{
                    GlobalConfig.networkConfig.type == "http" ? QNetworkProxy::HttpProxy : QNetworkProxy::Socks5Proxy, //
                    GlobalConfig.networkConfig.address,                                                                //
                    quint16(GlobalConfig.networkConfig.port)                                                           //
                };
                accessManager.setProxy(p);
                break;
            }
        }

        if (accessManager.proxy().type() == QNetworkProxy::Socks5Proxy)
        {
            DEBUG(MODULE_NETWORK, "Adding HostNameLookupCapability to proxy.")
            accessManager.proxy().setCapabilities(accessManager.proxy().capabilities() | QNetworkProxy::HostNameLookupCapability);
        }

        request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);
        request.setAttribute(QNetworkRequest::HTTP2AllowedAttribute, true);
        auto ua = GlobalConfig.networkConfig.userAgent;
        ua.replace("$VERSION", QV2RAY_VERSION_STRING);
        request.setHeader(QNetworkRequest::KnownHeaders::UserAgentHeader, ua);
    }

    QByteArray QvHttpRequestHelper::Get(const QString &url)
    {
        request.setUrl({ url });
        setAccessManagerAttributes(accessManager);
        auto _reply = accessManager.get(request);
        //
        QEventLoop loop;
        connect(_reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();
        //
        // Data or timeout?
        auto data = _reply->readAll();
        return data;
    }

    void QvHttpRequestHelper::AsyncGet(const QString &url)
    {
        request.setUrl({ url });
        setAccessManagerAttributes(accessManager);
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
