#include "ConfigHandler.hpp"
#include "components/plugins/QvPluginHost.hpp"
#include "core/connection/Generation.hpp"

optional<QString> QvConfigHandler::CHStartConnection_p(const ConnectionId &id, const CONFIGROOT &root)
{
    connections[id].lastConnected = system_clock::to_time_t(system_clock::now());
    //
    auto fullConfig = GenerateRuntimeConfig(root);

    //
    auto v = QVariant::fromValue(QList<QString>{ connections[id].displayName });
    PluginHost->SendHook(HOOK_TYPE_STATE_EVENTS, HOOK_STYPE_PRE_CONNECTING, v);
    auto result = vCoreInstance->StartConnection(id, fullConfig);

    if (!result.has_value())
    {
        currentConnectionId = id;
        emit OnConnected(currentConnectionId);
        PluginHost->SendHook(HOOK_TYPE_STATE_EVENTS, HOOK_STYPE_POST_CONNECTED, v);
    }
    else
    {
        PluginHost->SendHook(HOOK_TYPE_STATE_EVENTS, HOOK_STYPE_POST_DISCONNECTED, v);
    }
    return result;
}

void QvConfigHandler::CHStopConnection_p()
{
    if (vCoreInstance->KernelStarted)
    {
        auto v = QVariant::fromValue(QList<QString>{ connections[currentConnectionId].displayName });
        PluginHost->SendHook(HOOK_TYPE_STATE_EVENTS, HOOK_STYPE_PRE_DISCONNECTING, v);
        vCoreInstance->StopConnection();
        // Copy
        ConnectionId id = currentConnectionId;
        currentConnectionId = NullConnectionId;
        emit OnDisconnected(id);
        PluginHost->SendHook(HOOK_TYPE_STATE_EVENTS, HOOK_STYPE_POST_DISCONNECTED, v);
    }
    else
    {
        LOG(MODULE_CORE_HANDLER, "VCore is not started, not disconnecting")
    }
}

void QvConfigHandler::OnStatsDataArrived(const ConnectionId &id, const quint64 uploadSpeed, const quint64 downloadSpeed)
{
    connections[id].upLinkData += uploadSpeed;
    connections[id].downLinkData += downloadSpeed;
    emit OnStatsAvailable(id, uploadSpeed, downloadSpeed, connections[id].upLinkData, connections[id].downLinkData);
    auto v = QVariant::fromValue(QList<quint64>() << uploadSpeed << downloadSpeed << connections[id].upLinkData << connections[id].downLinkData);
    PluginHost->SendHook(HOOK_TYPE_STATS_EVENTS, HOOK_STYPE_STATS_CHANGED, v);
}

void QvConfigHandler::OnVCoreCrashed(const ConnectionId &id)
{
    LOG(MODULE_CORE_HANDLER, "V2ray core crashed!")
    currentConnectionId = NullConnectionId;
    emit OnDisconnected(id);
    emit OnCrashed();
}
