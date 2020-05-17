#include "ConnectionIO.hpp"

#include "Serialization.hpp"
#include "common/HTTPRequestHelper.hpp"
#include "common/QvHelpers.hpp"

namespace Qv2ray::core::connection
{
    namespace ConnectionIO
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
        QMultiHash<QString, CONFIGROOT> GetConnectionConfigFromSubscription(const QUrl &subscriptionUrl, const QString &groupName)
        {
            QMultiHash<QString, CONFIGROOT> subscriptionContent;
            QvHttpRequestHelper helper;
            const auto data = helper.Get(subscriptionUrl);
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
                    subscriptionContent.insert(connectionConfigMap.key(val), val);
                }
            }
            return subscriptionContent;
        }
    } // namespace ConnectionIO
} // namespace Qv2ray::core::connection
