#pragma once

#include <QString>
#include <QHash>
#include <QHashFunctions>

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

    template <typename T> uint qHash(const IDType<T> &key, uint seed = 0)
    {
        return key.qHash(seed);
    }
    //
    class __QvGroup;
    class __QvConnection;
    class __QvSubscription;
    typedef IDType<__QvGroup> GroupId;
    typedef IDType<__QvConnection> ConnectionId;
    typedef IDType<__QvSubscription> SubscriptionId;
}

using namespace Qv2ray::core;
