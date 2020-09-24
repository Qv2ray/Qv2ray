#include "BuiltinSubscriptionAdapter.hpp"

#include "core/SubscriptionAdapter.hpp"

bool InternalSubscriptionSupportPlugin::InitializePlugin(const QString &, const QJsonObject &settings)
{
    this->settings = settings;
    subscriptionAdapter = std::make_shared<BuiltinSubscriptionAdapterInterface>();
    return true;
}
