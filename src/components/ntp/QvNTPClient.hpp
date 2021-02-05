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

#pragma once
#include "base/Qv2rayFeatures.hpp"

#if QV2RAY_FEATURE(util_has_ntp)
#include <QDateTime>
#include <QHostAddress>
#include <QObject>
#include <QUdpSocket>
#include <QtEndian>
#include <QtGlobal>

namespace Qv2ray::components::ntp
{
    const qint64 january_1_1900 = -2208988800000ll;

    enum NtpLeapIndicator
    {
        NoWarning = 0,             /**< No warning. */
        LastMinute61Warning = 1,   /**< Last minute has 61 seconds. */
        LastMinute59Warning = 2,   /**< Last minute has 59 seconds. */
        UnsynchronizedWarning = 3, /**< Alarm condition (clock not synchronized). */
    };

    enum NtpMode
    {
        ReservedMode = 0,         /**< Reserved. */
        SymmetricActiveMode = 1,  /**< Symmetric active. */
        SymmetricPassiveMode = 2, /**< Symmetric passive. */
        ClientMode = 3,           /**< Client. */
        ServerMode = 4,           /**< Server. */
        BroadcastMode = 5,        /**< Broadcast. */
        ControlMode = 6,          /**< NTP control message. */
        PrivateMode = 7,          /**< Reserved for private use. */
    };

    enum NtpStratum
    {
        UnspecifiedStratum = 0,    /**< Unspecified or unavailable. */
        PrimaryStratum = 1,        /**< Primary reference (e.g. radio-clock). */
        SecondaryStratumFirst = 2, /**< Secondary reference (via NTP or SNTP). */
        SecondaryStratumLast = 15,
        UnsynchronizedStratum = 16, /**< Unsynchronized. */
                                    /* 17-255 are reserved. */
    };

    struct NtpPacketFlags
    {
        unsigned char mode : 3;
        unsigned char versionNumber : 3;
        unsigned char leapIndicator : 2;
    };

#pragma pack(push, 1)

    struct NtpTimestamp
    {
        quint32 seconds;
        quint32 fraction;
        static inline NtpTimestamp fromDateTime(const QDateTime &dateTime);
        static inline QDateTime toDateTime(const NtpTimestamp &ntpTime);
    };

    struct NtpPacket
    {
        NtpPacketFlags flags;
        quint8 stratum;
        qint8 poll;
        qint8 precision;
        qint32 rootDelay;
        qint32 rootDispersion;
        qint8 referenceID[4];
        NtpTimestamp referenceTimestamp;
        NtpTimestamp originateTimestamp;
        NtpTimestamp receiveTimestamp;
        NtpTimestamp transmitTimestamp;
    };

    struct NtpAuthenticationInfo
    {
        quint32 keyId;
        quint8 messageDigest[16];
    };

    struct NtpFullPacket
    {
        NtpPacket basic;
        NtpAuthenticationInfo auth;
    };

#pragma pack(pop)

    class NtpReplyPrivate : public QSharedData
    {
      public:
        NtpFullPacket packet;
        QDateTime destinationTime;
    };

    class NtpReply
    {
      public:
        NtpReply();
        NtpReply(const NtpReply &other);
        ~NtpReply();
        NtpReply &operator=(const NtpReply &other);
        NtpLeapIndicator leapIndicator() const;
        quint8 versionNumber() const;
        NtpMode mode() const;
        quint8 stratum() const;
        qreal pollInterval() const;
        qreal precision() const;
        QDateTime referenceTime() const;
        QDateTime originTime() const;
        QDateTime receiveTime() const;
        QDateTime transmitTime() const;
        QDateTime destinationTime() const;
        qint64 roundTripDelay() const;
        qint64 localClockOffset() const;
        bool isNull() const;

      protected:
        friend class NtpClient;
        NtpReply(NtpReplyPrivate *dd);

      private:
        QSharedDataPointer<NtpReplyPrivate> d;
    };

    class NtpClient : public QObject
    {
        Q_OBJECT

      public:
        NtpClient(QObject *parent = NULL);
        NtpClient(const QHostAddress &bindAddress, quint16 bindPort, QObject *parent = NULL);
        virtual ~NtpClient();
        bool sendRequest(const QHostAddress &address, quint16 port);

      Q_SIGNALS:
        void replyReceived(const QHostAddress &address, quint16 port, const NtpReply &reply);

      private Q_SLOTS:
        void readPendingDatagrams();

      private:
        void init(const QHostAddress &bindAddress, quint16 bindPort);

        QUdpSocket *mSocket;
    };
} // namespace Qv2ray::components::ntp
#endif
