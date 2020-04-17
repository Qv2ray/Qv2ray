#include "KernelInstanceHandler.hpp"

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
        //
        kernels = PluginHost->GetPluginKernels();
        for (const auto &kernel : kernels)
        {
            connect(kernel.get(), &QvPluginKernel::OnKernelCrashed, this, &KernelInstanceHandler::OnKernelCrashed_p);
            connect(kernel.get(), &QvPluginKernel::OnKernelLogAvaliable, this, &KernelInstanceHandler::OnKernelLogAvailable_p);
        }
    }

    KernelInstanceHandler::~KernelInstanceHandler()
    {
    }

    std::optional<QString> KernelInstanceHandler::StartConnection(const ConnectionId &id, const CONFIGROOT &root)
    {
        if (vCoreInstance->KernelStarted || !activeKernels.isEmpty())
        {
            StopConnection();
        }
        activeKernels.clear();
        this->root = root;
        auto fullConfig = GenerateRuntimeConfig(root);
        auto inboundPorts = GetInboundPorts(fullConfig);
        PluginHost->Send_ConnectivityEvent({ GetDisplayName(id), inboundPorts, Events::Connectivity::QvConnecticity_Connecting });
        QList<std::tuple<QString, int, QString>> inboundInfo;
        for (const auto &inbound_v : fullConfig["inbounds"].toArray())
        {
            const auto &inbound = inbound_v.toObject();
            inboundInfo.push_back({ inbound["protocol"].toString(), inbound["port"].toInt(), inbound["tag"].toString() });
        }
        //
        bool isComplex = IsComplexConfig(root);
        if (GlobalConfig.pluginConfig.v2rayIntegration)
        {
            if (isComplex)
            {
                LOG(MODULE_CONNECTION, "WARNING: Complex connection config support of this feature has not been tested.")
            }
            QList<std::tuple<QString, QString, QString>> pluginProcessedOutboundList;
            //
            // Process outbounds.
            {
                OUTBOUNDS new_outbounds;
                auto pluginPort = GlobalConfig.pluginConfig.portAllocationStart;
                //
                /// Key = Original Outbound Tag, Value = QStringList containing new outbound lists.
                for (const auto &outbound_v : fullConfig["outbounds"].toArray())
                {
                    const auto &outbound = outbound_v.toObject();
                    const auto &outProtocol = outbound["protocol"].toString();
                    //
                    if (!kernels.contains(outProtocol))
                    {
                        // Normal outbound, or the one without a plugin supported.
                        new_outbounds.push_back(outbound);
                        continue;
                    }
                    LOG(MODULE_CONNECTION, "Get kernel plugin: " + outProtocol)
                    auto kernel = kernels[outProtocol].get();
                    disconnect(kernel, &QvPluginKernel::OnKernelStatsAvailable, this, &KernelInstanceHandler::OnStatsDataArrived_p);
                    activeKernels.insert(outProtocol, kernel);
                    //
                    QMap<QString, int> pluginInboundPort;
                    const auto &originalOutboundTag = outbound["tag"].toString();
                    for (const auto &[inProtocol, inPort, inTag] : inboundInfo)
                    {
                        if (!QStringList{ "http", "socks" }.contains(inProtocol))
                            continue;
                        pluginInboundPort.insert(inProtocol, pluginPort);
                        LOG(MODULE_VCORE, "Plugin Integration: " + QSTRN(pluginPort) + " = " + inProtocol + "(" + inTag + ") --> " + outProtocol)
                        //
                        const auto &freedomTag = "plugin_" + inTag + "_" + inProtocol + "-" + QSTRN(inPort) + "_" + QSTRN(pluginPort);
                        const auto &pluginOutSettings = GenerateHTTPSOCKSOut("127.0.0.1", pluginPort, false, "", "");
                        const auto &direct = GenerateOutboundEntry(inProtocol, pluginOutSettings, {}, {}, "0.0.0.0", freedomTag);
                        //
                        // Add the integration outbound to the list.
                        new_outbounds.push_back(direct);
                        pluginProcessedOutboundList.append({ originalOutboundTag, inTag, freedomTag });
                        pluginPort++;
                    }
                    kernel->SetConnectionSettings(GlobalConfig.inboundConfig.listenip, pluginInboundPort, outbound["settings"].toObject());
                }
                fullConfig["outbounds"] = new_outbounds;
            }
            //
            // Process routing entries
            {
                QJsonArray newRules;
                auto unprocessedOutbound = pluginProcessedOutboundList;
                const auto rules = fullConfig["routing"].toObject()["rules"].toArray();
                for (auto i = 0; i < rules.count(); i++)
                {
                    const auto rule = rules.at(i).toObject();
                    //
                    bool ruleProcessed = false;
                    for (const auto &[originalTag, inboundTag, newOutboundTag] : pluginProcessedOutboundList)
                    {
                        // Check if a rule corresponds to the plugin outbound.
                        if (rule["outboundTag"] == originalTag)
                        {
                            auto newRule = rule;
                            newRule["outboundTag"] = newOutboundTag;
                            newRule["inboundTag"] = QJsonArray{ inboundTag };
                            newRules.push_back(newRule);
                            ruleProcessed = true;
                            unprocessedOutbound.removeOne({ originalTag, inboundTag, newOutboundTag });
                        }
                    }
                    if (!ruleProcessed)
                    {
                        newRules.append(rule);
                    }
                }

                for (const auto &[originalTag, inboundTag, newOutboundTag] : unprocessedOutbound)
                {
                    QJsonObject integrationRule;
                    integrationRule["type"] = "field";
                    integrationRule["outboundTag"] = newOutboundTag;
                    integrationRule["inboundTag"] = QJsonArray{ inboundTag };
                    newRules.push_back(integrationRule);
                }
                auto routing = fullConfig["routing"].toObject();
                routing["rules"] = newRules;
                fullConfig["routing"] = routing;
            }
            //
            currentConnectionId = id;
            lastConnectionId = id;
            bool success = true;
            for (auto &kernel : activeKernels.keys())
            {
                bool status = activeKernels[kernel]->StartKernel();
                success = success && status;
                if (!status)
                {
                    LOG(MODULE_CONNECTION, "Plugin Kernel: " + kernel + " failed to start.")
                    break;
                }
            }
            if (!success)
            {
                return tr("A plugin kernel failed to start. Please check the outbound settings.");
            }
            //
            auto result = vCoreInstance->StartConnection(fullConfig);

            if (!result.has_value())
            {
                emit OnConnected(currentConnectionId, inboundPorts);
                PluginHost->Send_ConnectivityEvent({ GetDisplayName(id), inboundPorts, Events::Connectivity::QvConnecticity_Connected });
            }
            else
            {
                PluginHost->Send_ConnectivityEvent({ GetDisplayName(id), {}, Events::Connectivity::QvConnecticity_Disconnected });
            }
            return result;
        } // namespace Qv2ray::core::handlers
        else
        {
            auto firstOutbound = fullConfig["outbounds"].toArray().first().toObject();
            const auto protocol = firstOutbound["protocol"].toString();
            if (kernels.contains(protocol))
            {
                auto kernel = kernels[firstOutbound["protocol"].toString()].get();
                activeKernels[protocol] = kernel;
                QMap<QString, int> pluginInboundPort;
                for (const auto &[_protocol, _port, _tag] : inboundInfo)
                {
                    pluginInboundPort[_protocol] = _port;
                }
                connect(kernel, &QvPluginKernel::OnKernelStatsAvailable, this, &KernelInstanceHandler::OnStatsDataArrived_p);
                currentConnectionId = id;
                lastConnectionId = id;
                kernel->SetConnectionSettings(GlobalConfig.inboundConfig.listenip, pluginInboundPort, firstOutbound["settings"].toObject());
                bool result = kernel->StartKernel();
                if (result)
                {
                    emit OnConnected(currentConnectionId, inboundPorts);
                    return {};
                }
                else
                {
                    return tr("A plugin kernel failed to start. Please check the outbound settings.");
                }
            }
            else
            {
                currentConnectionId = id;
                lastConnectionId = id;
                auto result = vCoreInstance->StartConnection(fullConfig);
                if (!result.has_value())
                {
                    emit OnConnected(currentConnectionId, inboundPorts);
                    PluginHost->Send_ConnectivityEvent({ GetDisplayName(id), inboundPorts, Events::Connectivity::QvConnecticity_Connected });
                }
                else
                {
                    PluginHost->Send_ConnectivityEvent({ GetDisplayName(id), {}, Events::Connectivity::QvConnecticity_Disconnected });
                }
                return result;
            }
        }
    }

    void KernelInstanceHandler::RestartConnection()
    {
        StopConnection();
        StartConnection(lastConnectionId, root);
    }

    void KernelInstanceHandler::OnKernelCrashed_p(const QString &msg)
    {
        StopConnection();
        emit OnCrashed(currentConnectionId, msg);
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
        if (vCoreInstance->KernelStarted || !activeKernels.isEmpty())
        {
            PluginHost->Send_ConnectivityEvent({ GetDisplayName(currentConnectionId), {}, Events::Connectivity::QvConnecticity_Disconnecting });
            if (vCoreInstance->KernelStarted)
            {
                vCoreInstance->StopConnection();
            }
            //
            for (const auto &kernel : activeKernels.keys())
            {
                LOG(MODULE_CONNECTION, "Stopping plugin kernel: " + kernel)
                activeKernels[kernel]->StopKernel();
            }
            // Copy
            ConnectionId id = currentConnectionId;
            currentConnectionId = NullConnectionId;
            emit OnDisconnected(id);
            PluginHost->Send_ConnectivityEvent({ GetDisplayName(id), {}, Events::Connectivity::QvConnecticity_Disconnected });
        }
        else
        {
            LOG(MODULE_CORE_HANDLER, "Cannot disconnect when there's nothing connected.")
        }
    }

    void KernelInstanceHandler::OnStatsDataArrived_p(const quint64 uploadSpeed, const quint64 downloadSpeed)
    {
        emit OnStatsDataAvailable(currentConnectionId, uploadSpeed, downloadSpeed);
    }
} // namespace Qv2ray::core::handlers
