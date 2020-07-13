#include "ConnectionIO.hpp"

#include "Serialization.hpp"
#include "common/QvHelpers.hpp"

namespace Qv2ray::core::connection::connectionIO
{
    CONFIGROOT ConvertConfigFromFile(const QString &sourceFilePath, bool importComplex)
    {
        auto root = CONFIGROOT(JsonFromString(StringFromFile(sourceFilePath)));

        if (!importComplex)
        {
            root.remove("inbounds");
            root.remove("routing");
            root.remove("dns");
        }

        root.remove("log");
        root.remove("api");
        root.remove("stats");
        return root;
    }

    QList<QPair<QString, CONFIGROOT>> GetConnectionConfigFromSubscription(const QByteArray &arr, const QString &groupName)
    {
        QList<QPair<QString, CONFIGROOT>> subscriptionContent;
        auto subscriptionLines = SplitLines(TryDecodeSubscriptionString(arr));
        for (const auto &line : subscriptionLines)
        {
            QString __alias;
            QString __errMessage;
            // Assign a group name, to pass the name check.
            QString __groupName = groupName;
            const auto connectionConfigMap = ConvertConfigFromString(line.trimmed(), &__alias, &__errMessage, &__groupName);
            if (!__errMessage.isEmpty())
                LOG(MODULE_SUBSCRIPTION, "Error: " + __errMessage)
            subscriptionContent << connectionConfigMap;
        }
        return subscriptionContent;
    }
} // namespace Qv2ray::core::connection::connectionIO
