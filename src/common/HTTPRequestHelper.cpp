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

    bool QvHttpRequestHelper::setUrl(const QString &url)
    {
        QUrl qUrl = QUrl(url);

        if (!qUrl.isValid())
        {
            LOG(MODULE_NETWORK, "Provided URL is invalid: " + url)
            return false;
        }

        request.setUrl(qUrl);
        return true;
    }

    void QvHttpRequestHelper::setHeader(const QByteArray &key, const QByteArray &value)
    {
        DEBUG(MODULE_NETWORK, "Adding HTTP request header: " + key + ":" + value)
        request.setRawHeader(key, value);
    }

    QByteArray QvHttpRequestHelper::syncget(const QString &url, bool useProxy)
    {
        this->setUrl(url);

        if (useProxy)
        {
            auto proxy = QNetworkProxyFactory::systemProxyForQuery();
            accessManager.setProxy(proxy.first());
            LOG(MODULE_NETWORK, "Sync get is using system proxy settings")
        }
        else
        {
            accessManager.setProxy(QNetworkProxy(QNetworkProxy::ProxyType::NoProxy));
        }

        request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);
        request.setAttribute(QNetworkRequest::HTTP2AllowedAttribute, true);
        request.setHeader(QNetworkRequest::KnownHeaders::UserAgentHeader, "Mozilla/5.0 (rv:71.0) Gecko/20100101 Firefox/71.0");
        reply = accessManager.get(request);
        connect(reply, &QNetworkReply::finished, this, &QvHttpRequestHelper::onRequestFinished_p);
        //
        QEventLoop loop;
        connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();
        // Data or timeout?
        auto data = reply->readAll();
        return data;
    }

    void QvHttpRequestHelper::get(const QString &url)
    {
        this->setUrl(url);
        //    request.setRawHeader("Content-Type",
        //    "application/x-www-form-urlencoded");
        reply = accessManager.get(request);
        connect(reply, &QNetworkReply::finished, this, &QvHttpRequestHelper::onRequestFinished_p);
        connect(reply, &QNetworkReply::readyRead, this, &QvHttpRequestHelper::onReadyRead);
    }

    // void QvHttpRequestHelper::post(const QString &url, const QByteArray
    // &data)
    //{
    //    this->setUrl(url);
    //    request.setRawHeader("Content-Type", "application/json");
    //    reply = accessManager.post(request, data);
    //    connect(reply, &QNetworkReply::finished, this,
    //    &QvHttpRequestHelper::onRequestFinished); connect(reply,
    //    &QNetworkReply::readyRead, this, &QvHttpRequestHelper::onReadyRead);
    //}

    //    void QvHttpRequestHelper::put(const QString &url, const QByteArray
    //    &data)
    //    {
    //        this->setUrl(url);
    //        request.setRawHeader("Content-Type", "application/json");
    //        reply = accessManager.put(request, data);
    //        connect(reply, &QNetworkReply::finished, this,
    //        &QvHttpRequestHelper::onRequestFinished); connect(reply,
    //        &QNetworkReply::readyRead, this,
    //        &QvHttpRequestHelper::onReadyRead);
    //    }

    //    void QvHttpRequestHelper::del(const QString &url)
    //    {
    //        this->setUrl(url);
    //        request.setRawHeader("Content-Type", "application/json");
    //        reply = accessManager.deleteResource(request);
    //        connect(reply, &QNetworkReply::finished, this,
    //        &QvHttpRequestHelper::onRequestFinished); connect(reply,
    //        &QNetworkReply::readyRead, this,
    //        &QvHttpRequestHelper::onReadyRead);
    //    }

    //    void QvHttpRequestHelper::login(const QString &url, const QByteArray
    //    &data)
    //    {
    //        this->setUrl(url);
    //        request.setRawHeader("Content-Type",
    //        "application/x-www-form-urlencoded"); reply =
    //        accessManager.post(request, data); connect(reply,
    //        &QNetworkReply::finished, this,
    //        &QvHttpRequestHelper::onRequestFinished); connect(reply,
    //        &QNetworkReply::readyRead, this,
    //        &QvHttpRequestHelper::onReadyRead);
    //    }

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
            emit httpRequestFinished(empty);
        }
        else
        {
            emit httpRequestFinished(this->data);
        }
    }

    void QvHttpRequestHelper::onReadyRead()
    {
        DEBUG(MODULE_NETWORK, "A request is now ready read")
        this->data += reply->readAll();
    }
} // namespace Qv2ray::common
