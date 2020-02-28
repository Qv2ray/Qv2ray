#include "ConnectionHandler.hpp"
#include "core/connection/Generation.hpp"

optional<QString> QvConnectionHandler::CHStartConnection_p(const ConnectionId &id, const CONFIGROOT &root)
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

void QvConnectionHandler::CHStopConnection_p()
{
    if (vCoreInstance->KernelStarted)
    {
        vCoreInstance->StopConnection();
        // Copy
        ConnectionId id = currentConnectionId;
        currentConnectionId = NullConnectionId;
        emit OnDisConnected(id);
    }
    else
    {
        LOG(MODULE_CORE_HANDLER, "VCore is not started, not disconnecting")
    }
}

void QvConnectionHandler::OnStatsDataArrived(const ConnectionId &id, const quint64 uploadSpeed, const quint64 downloadSpeed)
{
    connections[id].upLinkData += uploadSpeed;
    connections[id].downLinkData += downloadSpeed;
    emit OnStatsAvailable(id, uploadSpeed, downloadSpeed, connections[id].upLinkData, connections[id].downLinkData);
}

void QvConnectionHandler::OnVCoreCrashed(const ConnectionId &id)
{
    LOG(MODULE_CORE_HANDLER, "V2ray core crashed!")
    currentConnectionId = NullConnectionId;
    emit OnDisConnected(id);
    emit OnCrashed();
}
