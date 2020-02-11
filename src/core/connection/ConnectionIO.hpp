#include "base/Qv2rayBase.hpp"

namespace Qv2ray::core::connection
{
    namespace ConnectionIO
    {
        QMap<QString, CONFIGROOT> GetRegularConnections(QStringList connections);
        QMap<QString, CONFIGROOT> GetSubscriptionConnection(QString subscription);
        QMap<QString, QMap<QString, CONFIGROOT>> GetSubscriptionConnections(QStringList subscriptions);
        //
        // Save Connection Config
        bool SaveConnectionConfig(CONFIGROOT obj, QString *alias, bool canOverrideExisting);
        bool SaveSubscriptionConfig(CONFIGROOT obj, const QString &subscription, QString *name);
        //
        bool RemoveConnection(const QString &alias);
        bool RemoveSubscriptionConnection(const QString &subsName, const QString &name);
        //
        bool RenameConnection(const QString &originalName, const QString &newName);
        bool RenameSubscription(const QString &originalName, const QString &newName);

        // File Protocol
        CONFIGROOT ConvertConfigFromFile(QString sourceFilePath, bool keepInbounds);
    }
}

using namespace Qv2ray::core;
using namespace Qv2ray::core::connection;
using namespace Qv2ray::core::connection::ConnectionIO;
