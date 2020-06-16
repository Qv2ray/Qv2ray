#include "ConnectionIO.hpp"

#include "Serialization.hpp"
#include "common/HTTPRequestHelper.hpp"
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
        }

        root.remove("log");
        root.remove("api");
        root.remove("stats");
        root.remove("dns");
        return root;
    }

    QList<QPair<QString, CONFIGROOT>> GetConnectionConfigFromSubscription(const QUrl &subscriptionUrl, const QString &groupName)
    {
        QList<QPair<QString, CONFIGROOT>> subscriptionContent;
        const auto data = NetworkRequestHelper().HttpGet(subscriptionUrl);
        auto subscriptionLines = SplitLines(TryDecodeSubscriptionString(data));
        for (const auto &line : subscriptionLines)
        {
            QString __alias;
            QString __errMessage;
            // Assign a group name, to pass the name check.
            QString __groupName = groupName;
            auto connectionConfigMap = ConvertConfigFromString(line.trimmed(), &__alias, &__errMessage, &__groupName);
            if (!__errMessage.isEmpty())
                LOG(MODULE_SUBSCRIPTION, "Error: " + __errMessage)
            for (const auto &val : connectionConfigMap)
            {
                subscriptionContent.append({ connectionConfigMap.key(val), val });
            }
        }
        return subscriptionContent;
    }
} // namespace Qv2ray::core::connection::connectionIO
