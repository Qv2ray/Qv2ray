#pragma once

#include <QString>
#include <QHash>
#include <QHashFunctions>

#include "base/models/QvConfigIdentifier.hpp"

namespace Qv2ray::core
{
    template <typename T>
    class IDType
    {
        public:
            IDType(const QString &id): m_id(id) {}
            friend bool operator==(const IDType<T> &lhs, const IDType<T> &rhs)
            {
                return lhs.m_id == rhs.m_id;
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

    template<typename IDType>
    QList<IDType> StringsToIdList(const QList<QString> &strings)
    {
        QList<IDType> list;

        for (auto str : strings) {
            list << IDType(str);
        }

        return list;
    }

    template <typename T> uint qHash(const IDType<T> &key, uint seed = 0)
    {
        return key.qHash(seed);
    }

    // Define several safetypes to prevent misuse of QString.
    class __QvGroup;
    class __QvConnection;
    typedef IDType<__QvGroup> GroupId;
    typedef IDType<__QvConnection> ConnectionId;
    //
    /// Metadata object representing a connection.
    struct ConnectionMetaObject : ConnectionObject_Config {
        GroupId groupId;
        ConnectionMetaObject(): ConnectionObject_Config(), groupId("null") { }
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
    struct GroupMetaObject: SubscriptionObject_Config  {
        // Implicit base of two types, since group object is actually the group base object.
        bool isSubscription;
        QList<ConnectionId> connections;
        // Suger for down casting.
        GroupMetaObject(): connections() {}
        GroupMetaObject(const GroupObjectBase &base): GroupMetaObject()
        {
            this->displayName = base.displayName;
            this->importDate = base.importDate;
            this->connections = StringsToIdList<ConnectionId>(base.connections);
        }
        // Suger for down casting.
        GroupMetaObject(const GroupObject_Config &base): GroupMetaObject((GroupObjectBase)base)
        {
            this->isSubscription = false;
        }
        // Suger for down casting.
        GroupMetaObject(const SubscriptionObject_Config &base): GroupMetaObject((GroupObjectBase)base)
        {
            this->isSubscription = true;
            this->address = base.address;
            this->lastUpdated = base.lastUpdated;
            this->updateInterval = base.updateInterval;
        }
    };
}

using namespace Qv2ray::core;
