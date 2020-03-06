/*
 * httpproxy.cpp - the source file of HttpProxy class
 *
 * Copyright (C) 2015 Symeon Huang <hzwhuang@gmail.com>
 *
 * This file is part of the libQtShadowsocks.
 *
 * libQtShadowsocks is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * libQtShadowsocks is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libQtShadowsocks; see the file LICENSE. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#include "httpproxy.hpp"
#include "socketstream.hpp"
#include <QDebug>
#include <QTcpSocket>
#include <QUrl>

namespace Qv2ray::core::kernel {

HttpProxy::HttpProxy() : QTcpServer()
{
#ifndef _MSC_VER
    this->setMaxPendingConnections(FD_SETSIZE);
#else
    this->setMaxPendingConnections(1024);
#endif
}

bool HttpProxy::httpListen(const QHostAddress &http_addr,
                           uint16_t http_port,
                           uint16_t socks_port)
{
    upstreamProxy = QNetworkProxy(QNetworkProxy::Socks5Proxy,
                                  http_addr.toString(),
                                  socks_port);
    return this->listen(http_addr, http_port);
}

void HttpProxy::incomingConnection(qintptr socketDescriptor)
{
    QTcpSocket *socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::readyRead,
            this, &HttpProxy::onSocketReadyRead);
    connect(socket, &QTcpSocket::disconnected,
            socket, &QTcpSocket::deleteLater);
    connect(socket,
            static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>
            (&QTcpSocket::error),
            this,
            &HttpProxy::onSocketError);
    socket->setSocketDescriptor(socketDescriptor);
}

void HttpProxy::onSocketError(QAbstractSocket::SocketError err)
{
    if (err != QAbstractSocket::RemoteHostClosedError) {
        QDebug(QtMsgType::QtWarningMsg) << "HTTP socket error: "
                                        << err;
    }
    sender()->deleteLater();
}

void HttpProxy::onSocketReadyRead()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    QTcpSocket *proxySocket = nullptr;

    QByteArray reqData = socket->readAll();
    int pos = reqData.indexOf("\r\n");
    QByteArray reqLine = reqData.left(pos);
    reqData.remove(0, pos + 2);

    QList<QByteArray> entries = reqLine.split(' ');
    QByteArray method = entries.value(0);
    QByteArray address = entries.value(1);
    QByteArray version = entries.value(2);

    QString host;
    uint16_t port;
    QString key;

    if (method != "CONNECT") {
        QUrl url = QUrl::fromEncoded(address);
        if (!url.isValid()) {
            QDebug(QtMsgType::QtCriticalMsg) << "Invalid URL: " << url;
            socket->disconnectFromHost();
            return;
        }
        host = url.host();
        port = url.port(80);
        QString req = url.path();
        if (url.hasQuery()) {
            req.append('?').append(url.query());
        }
        reqLine = method + " " + req.toUtf8() + " " + version + "\r\n";
        reqData.prepend(reqLine);
        key = host + ':' + QString::number(port);
        proxySocket = socket->findChild<QTcpSocket *>(key);
        if (proxySocket) {
            proxySocket->write(reqData);
            return;//if we find an existing socket, then use it and return
        }
    } else {//CONNECT method
        /*
         * http://tools.ietf.org/html/draft-luotonen-ssl-tunneling-03
         * the first line would CONNECT HOST:PORT VERSION
         */
        QList<QByteArray> host_port_list = address.split(':');
        host = QString(host_port_list.first());
        port = host_port_list.last().toUShort();
    }

    proxySocket = new QTcpSocket(socket);
    proxySocket->setProxy(upstreamProxy);
    if (method != "CONNECT") {
        proxySocket->setObjectName(key);
        proxySocket->setProperty("reqData", reqData);
        connect (proxySocket, &QTcpSocket::connected,
                 this, &HttpProxy::onProxySocketConnected);
        connect (proxySocket, &QTcpSocket::readyRead,
                 this, &HttpProxy::onProxySocketReadyRead);
    } else {
        connect (proxySocket, &QTcpSocket::connected,
                 this, &HttpProxy::onProxySocketConnectedHttps);
    }
    connect (proxySocket, &QTcpSocket::disconnected,
             proxySocket, &QTcpSocket::deleteLater);
    connect (proxySocket,
             static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>
             (&QTcpSocket::error),
             this,
             &HttpProxy::onSocketError);
    proxySocket->connectToHost(host, port);
}

void HttpProxy::onProxySocketConnected()
{
    QTcpSocket *proxySocket = qobject_cast<QTcpSocket *>(sender());
    QByteArray reqData = proxySocket->property("reqData").toByteArray();
    proxySocket->write(reqData);
}

void HttpProxy::onProxySocketConnectedHttps()
{
    QTcpSocket *proxySocket = qobject_cast<QTcpSocket *>(sender());
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(proxySocket->parent());
    disconnect(socket, &QTcpSocket::readyRead,
               this, &HttpProxy::onSocketReadyRead);

    /*
     * once it's connected
     * we use a light-weight SocketStream class to do the job
     */
    SocketStream *stream = new SocketStream(socket, proxySocket, this);
    connect(socket, &QTcpSocket::disconnected,
            stream, &SocketStream::deleteLater);
    connect(proxySocket, &QTcpSocket::disconnected,
            stream, &SocketStream::deleteLater);
    static const QByteArray httpsHeader =
            "HTTP/1.0 200 Connection established\r\n\r\n";
    socket->write(httpsHeader);
}

void HttpProxy::onProxySocketReadyRead()
{
    QTcpSocket *proxySocket = qobject_cast<QTcpSocket *>(sender());
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(proxySocket->parent());
    socket->write(proxySocket->readAll());
}

}
