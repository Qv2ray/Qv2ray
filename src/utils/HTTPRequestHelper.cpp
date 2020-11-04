#include "HTTPRequestHelper.hpp"

#include "base/Qv2rayBase.hpp"

#include <QByteArray>
#include <QNetworkProxy>

#define QV_MODULE_NAME "NetworkCore"

namespace Qv2ray::common::network
{
    void NetworkRequestHelper::setHeader(QNetworkRequest &request, const QByteArray &key, const QByteArray &value)
    {
        DEBUG("Adding HTTP request header: " + key + ":" + value);
        request.setRawHeader(key, value);
    }

    void NetworkRequestHelper::setAccessManagerAttributes(QNetworkRequest &request, QNetworkAccessManager &accessManager)
    {
        switch (GlobalConfig.networkConfig.proxyType)
        {
            case Qv2rayConfig_Network::QVPROXY_NONE:
            {
                DEBUG("Get without proxy.");
                accessManager.setProxy(QNetworkProxy(QNetworkProxy::ProxyType::NoProxy));
                break;
            }
            case Qv2rayConfig_Network::QVPROXY_SYSTEM:
            {
                accessManager.setProxy(QNetworkProxyFactory::systemProxyForQuery().first());
                break;
            }
            case Qv2rayConfig_Network::QVPROXY_CUSTOM:
            {
                QNetworkProxy p{
                    GlobalConfig.networkConfig.type == "http" ? QNetworkProxy::HttpProxy : QNetworkProxy::Socks5Proxy, //
                    GlobalConfig.networkConfig.address,                                                                //
                    quint16(GlobalConfig.networkConfig.port)                                                           //
                };
                accessManager.setProxy(p);
                break;
            }
            default: Q_UNREACHABLE();
        }

        if (accessManager.proxy().type() == QNetworkProxy::Socks5Proxy)
        {
            DEBUG("Adding HostNameLookupCapability to proxy.");
            accessManager.proxy().setCapabilities(accessManager.proxy().capabilities() | QNetworkProxy::HostNameLookupCapability);
        }

        request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);

#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
        // request.setAttribute(QNetworkRequest::Http2AllowedAttribute, true);
#else
        // request.setAttribute(QNetworkRequest::HTTP2AllowedAttribute, true);
#endif

        auto ua = GlobalConfig.networkConfig.userAgent;
        ua.replace("$VERSION", QV2RAY_VERSION_STRING);
        request.setHeader(QNetworkRequest::KnownHeaders::UserAgentHeader, ua);
    }

    QByteArray NetworkRequestHelper::HttpGet(const QUrl &url)
    {
        QNetworkRequest request;
        QNetworkAccessManager accessManager;
        request.setUrl(url);
        setAccessManagerAttributes(request, accessManager);
        auto _reply = accessManager.get(request);
        //
        {
            QEventLoop loop;
            QObject::connect(&accessManager, &QNetworkAccessManager::finished, &loop, &QEventLoop::quit);
            loop.exec();
        }
        //
        // Data or timeout?
        LOG(_reply->errorString());
        auto data = _reply->readAll();
        return data;
    }

    void NetworkRequestHelper::AsyncHttpGet(const QString &url, std::function<void(const QByteArray &)> funcPtr)
    {
        QNetworkRequest request;
        request.setUrl(url);
        auto accessManagerPtr = new QNetworkAccessManager();
        setAccessManagerAttributes(request, *accessManagerPtr);
        auto reply = accessManagerPtr->get(request);
        QObject::connect(reply, &QNetworkReply::finished, [=]() {
            {
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
                bool h2Used = reply->attribute(QNetworkRequest::Http2WasUsedAttribute).toBool();
#else
                bool h2Used = reply->attribute(QNetworkRequest::HTTP2WasUsedAttribute).toBool();
#endif
                if (h2Used)
                    DEBUG("HTTP/2 was used.");

                if (reply->error() != QNetworkReply::NoError)
                    LOG("Network error: " + QString(QMetaEnum::fromType<QNetworkReply::NetworkError>().key(reply->error())));

                funcPtr(reply->readAll());
                accessManagerPtr->deleteLater();
            }
        });
    }

} // namespace Qv2ray::common::network
