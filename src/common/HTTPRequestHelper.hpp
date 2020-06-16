#pragma once

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QObject>

namespace Qv2ray::common::network
{
    class NetworkRequestHelper
    {
      public:
        typedef void (*Qv2rayNetworkRequestCallback)(const QByteArray &);
        void AsyncHttpGet(const QString &url, Qv2rayNetworkRequestCallback funcPtr);
        QByteArray HttpGet(const QUrl &url);

      private:
        static void setAccessManagerAttributes(QNetworkRequest &request, QNetworkAccessManager &accessManager);
        static void setHeader(QNetworkRequest &request, const QByteArray &key, const QByteArray &value);
        QNetworkAccessManager accessManager;
    };
} // namespace Qv2ray::common::network

using namespace Qv2ray::common::network;
