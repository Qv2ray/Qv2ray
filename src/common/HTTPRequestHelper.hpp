/*
    Copyright (C) 2019 SoneWinstone (jianwenzhen@qq.com)
    Copyright (C) 2019 Leroy.H.Y

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QObject>

namespace Qv2ray::common
{
    class QvHttpRequestHelper : public QObject
    {
        Q_OBJECT
      public:
        explicit QvHttpRequestHelper(QObject *parent = nullptr);
        ~QvHttpRequestHelper();
        // get
        void AsyncGet(const QString &url);
        QByteArray Get(const QUrl &url);
        QByteArray Get(const QString &url)
        {
            return Get(QUrl{ url });
        }
      signals:
        void OnRequestFinished(QByteArray &data);

      private slots:
        void onRequestFinished_p();
        void onReadyRead_p();

      private:
        void setAccessManagerAttributes(QNetworkAccessManager &accessManager);
        void setHeader(const QByteArray &key, const QByteArray &value);
        QByteArray data;
        QUrl url;
        QNetworkReply *reply;
        QNetworkRequest request;
        QNetworkAccessManager accessManager;
    };
} // namespace Qv2ray::common

using namespace Qv2ray::common;
