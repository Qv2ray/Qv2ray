#include "ConnectionHandler.hpp"
#include "core/connection/Generation.hpp"

optional<QString> QvConnectionHandler::CHStartConnection_p(const ConnectionId &id, const CONFIGROOT &root)
{
    auto &connectionMeta = connections[id];
    auto fullConfig = GenerateRuntimeConfig(root);
    //
    auto result = vCoreInstance->StartConnection(id, fullConfig);

    if (!result.has_value()) {
        currentConnectionId = id;
        emit OnConnected(currentConnectionId);
        connectionMeta.lastConnected = system_clock::to_time_t(system_clock::now());
    }

    return result;
}

void QvConnectionHandler::CHStopConnection_p()
{
    if (vCoreInstance->KernelStarted) {
        vCoreInstance->StopConnection();
        emit OnDisConnected(currentConnectionId);
    } else {
        LOG(MODULE_CORE_HANDLER, "VCore is not started, not disconnecting")
    }
}

void QvConnectionHandler::OnStatsDataArrived(const ConnectionId &id, const QString tag, const quint64 uploadSpeed, const quint64 downloadSpeed)
{
    Q_UNUSED(tag)
    emit OnStatsAvailable(id, uploadSpeed, downloadSpeed);
    connections[id].upLinkData += uploadSpeed;
    connections[id].downLinkData += downloadSpeed;
}

void QvConnectionHandler::OnVCoreCrashed(const ConnectionId &id)
{
    LOG(MODULE_CORE_HANDLER, "V2ray core crashed!")
    emit OnDisConnected(id);
}
