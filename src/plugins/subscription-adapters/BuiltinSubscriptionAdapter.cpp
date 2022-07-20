#include "BuiltinSubscriptionAdapter.hpp"

#include "core/SubscriptionAdapter.hpp"

bool InternalSubscriptionSupportPlugin::InitializePlugin()
{
    m_SubscriptionInterface = std::make_shared<BuiltinSubscriptionAdapterInterface>();
    return true;
}
