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
        bool setUrl(const QString &url);
        void setHeader(const QByteArray &key, const QByteArray &value);
        // get
        QByteArray syncget(const QString &url, bool useProxy);
        void get(const QString &url);
        //// insert
        // void post(const QString &url, const QByteArray &data);
        //// update
        // void put(const QString &url, const QByteArray &data);
        //// delete
        // void del(const QString &url);
        // void login(const QString &url, const QByteArray &data);
      signals:
        void httpRequestFinished(QByteArray &data);

      public slots:
        void onRequestFinished_p();
      private slots:
        void onReadyRead();

      private:
        QByteArray data;
        QUrl url;
        QNetworkReply *reply;
        QNetworkRequest request;
        QNetworkAccessManager accessManager;
    };
} // namespace Qv2ray::common

using namespace Qv2ray::common;
