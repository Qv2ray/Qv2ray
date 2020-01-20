#include "QvHTTPRequestHelper.hpp"
#include <QByteArray>
#include <QNetworkProxy>

namespace Qv2ray
{
    QvHttpRequestHelper::QvHttpRequestHelper() : reply()
    {
    }

    QvHttpRequestHelper::~QvHttpRequestHelper()
    {
        accessManager.disconnect();
    }

    bool QvHttpRequestHelper::setUrl(const QString &url)
    {
        QUrl qUrl = QUrl(url);

        if (!qUrl.isValid()) {
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

        if (useProxy) {
            auto proxy = QNetworkProxyFactory::systemProxyForQuery();
            accessManager.setProxy(proxy.first());
        } else {
            accessManager.setProxy(QNetworkProxy(QNetworkProxy::ProxyType::NoProxy));
        }

        LOG(MODULE_NETWORK, "Sync get is using system proxy settings")
        request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);
        reply = accessManager.get(request);
        connect(reply, &QNetworkReply::finished, this, &QvHttpRequestHelper::onRequestFinished);
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
        //    request.setRawHeader("Content-Type", "application/x-www-form-urlencoded");
        reply = accessManager.get(request);
        connect(reply, &QNetworkReply::finished, this, &QvHttpRequestHelper::onRequestFinished);
        connect(reply, &QNetworkReply::readyRead, this, &QvHttpRequestHelper::onReadyRead);
    }

    //void QvHttpRequestHelper::post(const QString &url, const QByteArray &data)
    //{
    //    this->setUrl(url);
    //    request.setRawHeader("Content-Type", "application/json");
    //    reply = accessManager.post(request, data);
    //    connect(reply, &QNetworkReply::finished, this, &QvHttpRequestHelper::onRequestFinished);
    //    connect(reply, &QNetworkReply::readyRead, this, &QvHttpRequestHelper::onReadyRead);
    //}

    //    void QvHttpRequestHelper::put(const QString &url, const QByteArray &data)
    //    {
    //        this->setUrl(url);
    //        request.setRawHeader("Content-Type", "application/json");
    //        reply = accessManager.put(request, data);
    //        connect(reply, &QNetworkReply::finished, this, &QvHttpRequestHelper::onRequestFinished);
    //        connect(reply, &QNetworkReply::readyRead, this, &QvHttpRequestHelper::onReadyRead);
    //    }

    //    void QvHttpRequestHelper::del(const QString &url)
    //    {
    //        this->setUrl(url);
    //        request.setRawHeader("Content-Type", "application/json");
    //        reply = accessManager.deleteResource(request);
    //        connect(reply, &QNetworkReply::finished, this, &QvHttpRequestHelper::onRequestFinished);
    //        connect(reply, &QNetworkReply::readyRead, this, &QvHttpRequestHelper::onReadyRead);
    //    }

    //    void QvHttpRequestHelper::login(const QString &url, const QByteArray &data)
    //    {
    //        this->setUrl(url);
    //        request.setRawHeader("Content-Type", "application/x-www-form-urlencoded");
    //        reply = accessManager.post(request, data);
    //        connect(reply, &QNetworkReply::finished, this, &QvHttpRequestHelper::onRequestFinished);
    //        connect(reply, &QNetworkReply::readyRead, this, &QvHttpRequestHelper::onReadyRead);
    //    }

    void QvHttpRequestHelper::onRequestFinished()
    {
        LOG(MODULE_NETWORK, "Network request errcode: " + QSTRN(reply->error()))
        emit httpRequestFinished(this->data);
    }

    void QvHttpRequestHelper::onReadyRead()
    {
        DEBUG(MODULE_NETWORK, "A request is now ready read")
        this->data += reply->readAll();
    }
}
