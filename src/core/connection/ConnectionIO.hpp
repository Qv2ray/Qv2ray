#include "base/Qv2rayBase.hpp"
#include "core/CoreSafeTypes.hpp"

namespace Qv2ray::core::connection
{
    namespace ConnectionIO
    {
        QMap<ConnectionId, CONFIGROOT> GetGroupConnections(const GroupId &group);
        QMap<ConnectionId, CONFIGROOT> GetSubscriptionConnections(const SubscriptionId &subscription);
        QMap<SubscriptionId, QMap<ConnectionId, CONFIGROOT>> GetSubscriptionConnections(const QList<SubscriptionId> &subscriptions);
        //
        // Save Connection Config
        bool SaveConnectionConfig(CONFIGROOT obj, const ConnectionId &id, bool canOverrideExisting);
        bool SaveSubscriptionConfig(CONFIGROOT obj, const SubscriptionId &subscription, const ConnectionId &name);
        //
        bool RemoveConnection(const SubscriptionId &id);
        bool RemoveSubscriptionConnection(const SubscriptionId &id, const ConnectionId &name);
        //
        bool RenameConnection(const ConnectionId &id, const QString &newName);
        bool RenameSubscription(const SubscriptionId &id, const QString &newName);

        // File Protocol
        CONFIGROOT ConvertConfigFromFile(QString sourceFilePath, bool importComplex);
    }
}

using namespace Qv2ray::core::connection;
using namespace Qv2ray::core::connection::ConnectionIO;
