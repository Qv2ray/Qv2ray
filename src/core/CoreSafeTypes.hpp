#pragma once

#include "base/models/QvConfigIdentifier.hpp"

#include <QHash>
#include <QHashFunctions>
#include <QString>

namespace Qv2ray::core
{
    template<typename T>
    class IDType
    {
      public:
        explicit IDType() : m_id("null")
        {
        }
        explicit IDType(const QString &id) : m_id(id)
        {
        }
        friend bool operator==(const IDType<T> &lhs, const IDType<T> &rhs)
        {
            return lhs.m_id == rhs.m_id;
        }
        friend bool operator!=(const IDType<T> &lhs, const IDType<T> &rhs)
        {
            return lhs.toString() != rhs.toString();
        }
        const QString &toString() const
        {
            return m_id;
        }
        uint qHash(uint seed) const
        {
            return ::qHash(m_id, seed);
        }

      private:
        QString m_id;
    };

    // Define several safetypes to prevent misuse of QString.
    class __QvGroup;
    class __QvConnection;
    typedef IDType<__QvGroup> GroupId;
    typedef IDType<__QvConnection> ConnectionId;

    inline const static ConnectionId NullConnectionId = ConnectionId("null");
    inline const static GroupId NullGroupId = GroupId("null");

    template<typename IDType>
    QList<IDType> StringsToIdList(const QList<QString> &strings)
    {
        QList<IDType> list;

        for (auto str : strings)
        {
            list << IDType(str);
        }

        return list;
    }

    template<typename IDType>
    QList<QString> IdListToStrings(const QList<IDType> &ids)
    {
        QList<QString> list;

        for (auto id : ids)
        {
            list << id.toString();
        }

        return list;
    }
    template<typename T>
    uint qHash(const IDType<T> &key, uint seed = 0)
    {
        return key.qHash(seed);
    }
    //
    /// Metadata object representing a connection.
    struct ConnectionMetaObject : ConnectionObject_Config
    {
        GroupId groupId = NullGroupId;
        ConnectionMetaObject() : ConnectionObject_Config()
        {
        }
        // Suger for down casting.
        ConnectionMetaObject(const ConnectionObject_Config &base) : ConnectionMetaObject()
        {
            this->latency = base.latency;
            this->lastConnected = base.lastConnected;
            this->importDate = base.lastConnected;
            this->upLinkData = base.upLinkData;
            this->downLinkData = base.downLinkData;
            this->displayName = base.displayName;
        }
    };

    /// Metadata object representing a group.
    struct GroupMetaObject : SubscriptionObject_Config
    {
        // Implicit base of two types, since group object is actually the group base object.
        bool isSubscription = false;
        QList<ConnectionId> connections;
        // Suger for down casting.
        GroupMetaObject() : connections()
        {
        }
        GroupMetaObject(const GroupObject_Config &base) : GroupMetaObject()
        {
            this->isSubscription = false;
            this->displayName = base.displayName;
            this->importDate = base.importDate;
            this->connections = StringsToIdList<ConnectionId>(base.connections);
        }
        // Suger for down casting.
        GroupMetaObject(const SubscriptionObject_Config &base) : GroupMetaObject((GroupObject_Config) base)
        {
            this->address = base.address;
            this->lastUpdated = base.lastUpdated;
            this->updateInterval = base.updateInterval;
            this->isSubscription = true;
        }
    };
} // namespace Qv2ray::core

using namespace Qv2ray::core;
