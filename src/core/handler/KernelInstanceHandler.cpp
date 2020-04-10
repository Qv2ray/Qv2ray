#include "KernelInstanceHandler.hpp"

#include "components/plugins/QvPluginHost.hpp"
#include "core/CoreUtils.hpp"
#include "core/connection/Generation.hpp"
namespace Qv2ray::core::handlers
{
    KernelInstanceHandler::KernelInstanceHandler(QObject *parent) : QObject(parent)
    {
        vCoreInstance = new V2rayKernelInstance(this);
        connect(vCoreInstance, &V2rayKernelInstance::OnNewStatsDataArrived, this, &KernelInstanceHandler::OnStatsDataArrived_p);
        connect(vCoreInstance, &V2rayKernelInstance::OnProcessOutputReadyRead, this, &KernelInstanceHandler::OnKernelLogAvailable_p);
        connect(vCoreInstance, &V2rayKernelInstance::OnProcessErrored, this, &KernelInstanceHandler::OnKernelCrashed_p);
    }

    KernelInstanceHandler::~KernelInstanceHandler()
    {
    }

    std::optional<QString> KernelInstanceHandler::StartConnection(const ConnectionId &id, const CONFIGROOT &root)
    {
        if (vCoreInstance->KernelStarted)
        {
            StopConnection();
        }
        this->root = root;
        auto fullConfig = GenerateRuntimeConfig(root);
        auto inboundPorts = GetInboundPorts(fullConfig);
        //
        PluginHost->Send_ConnectivityEvent(QvConnectivityEventObject{ GetDisplayName(id), inboundPorts, QvConnecticity_Connecting });
        auto result = vCoreInstance->StartConnection(fullConfig);

        if (!result.has_value())
        {
            currentConnectionId = id;
            lastConnectionId = id;
            emit OnConnected(currentConnectionId);
            PluginHost->Send_ConnectivityEvent({ GetDisplayName(id), inboundPorts, QvConnecticity_Connected });
        }
        else
        {
            PluginHost->Send_ConnectivityEvent({ GetDisplayName(id), {}, QvConnecticity_Disconnected });
        }
        return result;
    }

    void KernelInstanceHandler::RestartConnection()
    {
        StopConnection();
        StartConnection(lastConnectionId, root);
    }

    void KernelInstanceHandler::OnKernelCrashed_p()
    {
        emit OnCrashed(currentConnectionId);
        emit OnDisconnected(currentConnectionId);
        lastConnectionId = currentConnectionId;
        currentConnectionId = NullConnectionId;
    }

    void KernelInstanceHandler::OnKernelLogAvailable_p(const QString &log)
    {
        emit OnKernelLogAvailable(currentConnectionId, log);
    }

    void KernelInstanceHandler::StopConnection()
    {
        if (vCoreInstance->KernelStarted)
        {
            PluginHost->Send_ConnectivityEvent({ GetDisplayName(currentConnectionId), {}, QvConnecticity_Disconnecting });
            vCoreInstance->StopConnection();
            // Copy
            ConnectionId id = currentConnectionId;
            currentConnectionId = NullConnectionId;
            emit OnDisconnected(id);
            PluginHost->Send_ConnectivityEvent({ GetDisplayName(id), {}, QvConnecticity_Disconnected });
        }
        else
        {
            LOG(MODULE_CORE_HANDLER, "VCore is not started, not disconnecting")
        }
    }

    void KernelInstanceHandler::OnStatsDataArrived_p(const quint64 uploadSpeed, const quint64 downloadSpeed)
    {
        emit OnStatsDataAvailable(currentConnectionId, uploadSpeed, downloadSpeed);
    }
} // namespace Qv2ray::core::handlers
