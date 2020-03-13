#include "ConfigHandler.hpp"
#include "core/connection/Generation.hpp"

optional<QString> QvConfigHandler::CHStartConnection_p(const ConnectionId &id, const CONFIGROOT &root)
{
    connections[id].lastConnected = system_clock::to_time_t(system_clock::now());
    //
    auto fullConfig = GenerateRuntimeConfig(root);
    auto result = vCoreInstance->StartConnection(id, fullConfig);

    if (!result.has_value())
    {
        currentConnectionId = id;
        emit OnConnected(currentConnectionId);
    }

    return result;
}

void QvConfigHandler::CHStopConnection_p()
{
    if (vCoreInstance->KernelStarted)
    {
        vCoreInstance->StopConnection();
        // Copy
        ConnectionId id = currentConnectionId;
        currentConnectionId = NullConnectionId;
        emit OnDisconnected(id);
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
}

void QvConfigHandler::OnVCoreCrashed(const ConnectionId &id)
{
    LOG(MODULE_CORE_HANDLER, "V2ray core crashed!")
    currentConnectionId = NullConnectionId;
    emit OnDisconnected(id);
    emit OnCrashed();
}
