/*
 * socketstream.h - the header file of SocketStream class
 *
 * Copyright (C) 2015-2018 Symeon Huang <hzwhuang@gmail.com>
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

#ifndef SOCKETSTREAM_H
#define SOCKETSTREAM_H

#include <QObject>
#include <QAbstractSocket>

namespace Qv2ray::core::kernel{

class SocketStream : public QObject
{
    Q_OBJECT
public:
    /*
     * A light-weight class dedicated to stream data between two sockets
     * all available data from socket a will be written to socket b
     * vice versa
     */
    SocketStream(QAbstractSocket *a,
                 QAbstractSocket *b,
                 QObject *parent = 0);

    SocketStream(const SocketStream &) = delete;

private:
    QAbstractSocket *m_as;
    QAbstractSocket *m_bs;

private slots:
    void onSocketAReadyRead();
    void onSocketBReadyRead();
};

}

#endif // SOCKETSTREAM_H
