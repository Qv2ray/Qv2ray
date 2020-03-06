/*
 * socketstream.cpp - the source file of SocketStream class
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

#include "socketstream.hpp"

namespace Qv2ray::core::kernel {
SocketStream::SocketStream(QAbstractSocket *a,
                           QAbstractSocket *b,
                           QObject *parent) :
    QObject(parent),
    m_as(a),
    m_bs(b)
{
    connect(m_as, &QAbstractSocket::readyRead,
            this, &SocketStream::onSocketAReadyRead);
    connect(m_bs, &QAbstractSocket::readyRead,
            this, &SocketStream::onSocketBReadyRead);
}

void SocketStream::onSocketAReadyRead()
{
    if (m_bs->isWritable()) {
        m_bs->write(m_as->readAll());
    } else {
        qCritical("The second socket is not writable");
    }
}

void SocketStream::onSocketBReadyRead()
{
    if (m_as->isWritable()) {
        m_as->write(m_bs->readAll());
    } else {
        qCritical("The first socket is not writable");
    }
}
}
