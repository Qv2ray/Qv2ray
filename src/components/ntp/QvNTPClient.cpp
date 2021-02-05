/* This file from part of QNtp, a library that implements NTP protocol.
 *
 * Copyright (C) 2011 Alexander Fokin <apfokin@gmail.com>
 *
 * QNtp is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * QNtp is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with QNtp. If not, see <http://www.gnu.org/licenses/>. */

#include "QvNTPClient.hpp"

#if QV2RAY_FEATURE(util_has_ntp)
#include <cmath>

namespace Qv2ray::components::ntp
{

    NtpTimestamp NtpTimestamp::fromDateTime(const QDateTime &dateTime)
    {
        qint64 ntpMSecs = dateTime.toMSecsSinceEpoch() - january_1_1900;

        quint32 seconds = ntpMSecs / 1000;
        quint32 fraction = 0x100000000ll * (ntpMSecs % 1000) / 1000;

        NtpTimestamp result;
        result.seconds = qToBigEndian(seconds);
        result.fraction = qToBigEndian(fraction);
        return result;
    }

    QDateTime NtpTimestamp::toDateTime(const NtpTimestamp &ntpTime)
    {
        /* Convert to local-endian. */
        quint32 seconds = qFromBigEndian(ntpTime.seconds);
        quint32 fraction = qFromBigEndian(ntpTime.fraction);

        /* Convert NTP timestamp to number of milliseconds passed since Jan 1 1900. */
        qint64 ntpMSecs = seconds * 1000ll + fraction * 1000ll / 0x100000000ll;

        /* Construct Qt date time. */
        return QDateTime::fromMSecsSinceEpoch(ntpMSecs + january_1_1900);
    }

    NtpReply::NtpReply() : d(new NtpReplyPrivate())
    {
        /* We don't use shared null because NtpReplyPrivate isn't a POD type and
         * allocation overhead is negligible here. */
        memset(&d->packet, 0, sizeof(d->packet));
    }

    NtpReply::NtpReply(NtpReplyPrivate *dd) : d(dd)
    {
        Q_ASSERT(dd != NULL);
    }

    NtpReply::NtpReply(const NtpReply &other) : d(other.d)
    {
    }

    NtpReply::~NtpReply()
    {
    }

    NtpReply &NtpReply::operator=(const NtpReply &other)
    {
        d = other.d;

        return *this;
    }

    NtpLeapIndicator NtpReply::leapIndicator() const
    {
        return static_cast<NtpLeapIndicator>(d->packet.basic.flags.leapIndicator);
    }

    quint8 NtpReply::versionNumber() const
    {
        return d->packet.basic.flags.versionNumber;
    }

    NtpMode NtpReply::mode() const
    {
        return static_cast<NtpMode>(d->packet.basic.flags.mode);
    }

    quint8 NtpReply::stratum() const
    {
        return d->packet.basic.stratum;
    }

    qreal NtpReply::pollInterval() const
    {
        return std::pow(static_cast<qreal>(2), static_cast<qreal>(d->packet.basic.poll));
    }

    qreal NtpReply::precision() const
    {
        return std::pow(static_cast<qreal>(2), static_cast<qreal>(d->packet.basic.precision));
    }

    QDateTime NtpReply::referenceTime() const
    {
        return NtpTimestamp::toDateTime(d->packet.basic.referenceTimestamp);
    }

    QDateTime NtpReply::originTime() const
    {
        return NtpTimestamp::toDateTime(d->packet.basic.originateTimestamp);
    }

    QDateTime NtpReply::receiveTime() const
    {
        return NtpTimestamp::toDateTime(d->packet.basic.receiveTimestamp);
    }

    QDateTime NtpReply::transmitTime() const
    {
        return NtpTimestamp::toDateTime(d->packet.basic.transmitTimestamp);
    }

    QDateTime NtpReply::destinationTime() const
    {
        return d->destinationTime;
    }

    qint64 NtpReply::roundTripDelay() const
    {
        return originTime().msecsTo(destinationTime()) - receiveTime().msecsTo(transmitTime());
    }

    qint64 NtpReply::localClockOffset() const
    {
        return (originTime().msecsTo(receiveTime()) + destinationTime().msecsTo(transmitTime())) / 2;
    }

    bool NtpReply::isNull() const
    {
        return d->destinationTime.isNull();
    }

    NtpClient::NtpClient(QObject *parent) : QObject(parent)
    {
        init(QHostAddress::Any, 0);
    }

    NtpClient::NtpClient(const QHostAddress &bindAddress, quint16 bindPort, QObject *parent) : QObject(parent)
    {
        init(bindAddress, bindPort);
    }

    void NtpClient::init(const QHostAddress &bindAddress, quint16 bindPort)
    {
        mSocket = new QUdpSocket(this);
        mSocket->bind(bindAddress, bindPort);

        connect(mSocket, SIGNAL(readyRead()), this, SLOT(readPendingDatagrams()));
    }

    NtpClient::~NtpClient()
    {
        return;
    }

    bool NtpClient::sendRequest(const QHostAddress &address, quint16 port)
    {
        if (mSocket->state() != QAbstractSocket::BoundState)
            return false;

        /* Initialize the NTP packet. */
        NtpPacket packet;
        memset(&packet, 0, sizeof(packet));
        packet.flags.mode = ClientMode;
        packet.flags.versionNumber = 4;
        packet.transmitTimestamp = NtpTimestamp::fromDateTime(QDateTime::currentDateTimeUtc());

        /* Send it. */
        if (mSocket->writeDatagram(reinterpret_cast<const char *>(&packet), sizeof(packet), address, port) < 0)
            return false;

        return true;
    }

    void NtpClient::readPendingDatagrams()
    {
        while (mSocket->hasPendingDatagrams())
        {
            NtpFullPacket packet;
            memset(&packet, 0, sizeof(packet));

            QHostAddress address;
            quint16 port;

            if (mSocket->readDatagram(reinterpret_cast<char *>(&packet), sizeof(packet), &address, &port) < (qint64) sizeof(NtpPacket))
                continue;

            QDateTime now = QDateTime::currentDateTime();

            /* Prepare reply. */
            NtpReplyPrivate *replyPrivate = new NtpReplyPrivate();
            replyPrivate->packet = packet;
            replyPrivate->destinationTime = now;
            NtpReply reply(replyPrivate);

            /* Notify. */
            Q_EMIT replyReceived(address, port, reply);
        }
    }

} // namespace Qv2ray::components::ntp
#endif
