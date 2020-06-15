#pragma once

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QObject>

namespace Qv2ray::common::network
{
    typedef void (*Qv2rayNetworkRequestCallback)(const QByteArray &);
    void AsyncHttpGet(const QString &url, Qv2rayNetworkRequestCallback funcPtr);
    QByteArray HttpGet(const QUrl &url);
} // namespace Qv2ray::common::network

using namespace Qv2ray::common::network;
