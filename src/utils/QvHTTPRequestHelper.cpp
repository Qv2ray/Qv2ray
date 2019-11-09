#include "QvHTTPRequestHelper.hpp"
#include <QByteArray>
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
            LOG(MODULE_NETWORK, "Provided URL is invalid")
            return false;
        }

        request.setUrl(qUrl);
        return true;
    }

    void QvHttpRequestHelper::setHeader(const QByteArray &key, const QByteArray &value)
    {
        request.setRawHeader(key, value);
    }

    QByteArray QvHttpRequestHelper::syncget(const QString &url)
    {
        this->setUrl(url);
        reply = accessManager.get(request);
        bool isReadyRead = reply->waitForReadyRead(10000);
        return isReadyRead ? reply->readAll() : QByteArray();
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
        LOG(MODULE_NETWORK, "Network request errcode: " + to_string(reply->error()))
        emit httpRequestFinished(this->data);
    }

    void QvHttpRequestHelper::onReadyRead()
    {
        this->data += reply->readAll();
    }
}
