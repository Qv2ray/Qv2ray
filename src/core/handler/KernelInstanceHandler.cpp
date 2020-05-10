#include "KernelInstanceHandler.hpp"

#include "core/CoreUtils.hpp"
#include "core/connection/Generation.hpp"
namespace Qv2ray::core::handlers
{
#define isConnected (vCoreInstance->KernelStarted || !activeKernels.isEmpty())
    KernelInstanceHandler::KernelInstanceHandler(QObject *parent) : QObject(parent)
    {
        KernelInstance = this;
        vCoreInstance = new V2rayKernelInstance(this);
        connect(vCoreInstance, &V2rayKernelInstance::OnNewStatsDataArrived, this, &KernelInstanceHandler::OnStatsDataArrived_p);
        connect(vCoreInstance, &V2rayKernelInstance::OnProcessOutputReadyRead, this, &KernelInstanceHandler::OnKernelLogAvailable_p);
        connect(vCoreInstance, &V2rayKernelInstance::OnProcessErrored, this, &KernelInstanceHandler::OnKernelCrashed_p);
        //
        auto kernelList = PluginHost->GetPluginKernels();
        for (const auto &kernelInfo : kernelList.keys())
        {
            auto kernel = kernelList.value(kernelInfo).get();
            kernels[kernelInfo] = kernel;
            connect(kernel, &QvPluginKernel::OnKernelCrashed, this, &KernelInstanceHandler::OnKernelCrashed_p);
            connect(kernel, &QvPluginKernel::OnKernelLogAvaliable, this, &KernelInstanceHandler::OnKernelLogAvailable_p);
        }
    }

    KernelInstanceHandler::~KernelInstanceHandler()
    {
    }

    std::optional<QString> KernelInstanceHandler::StartConnection(const ConnectionGroupPair &id, const CONFIGROOT &root)
    {
        if (isConnected)
        {
            StopConnection();
        }
        activeKernels.clear();
        this->root = root;
        bool isComplex = IsComplexConfig(root);
        auto fullConfig = GenerateRuntimeConfig(root);
        inboundPorts = GetConfigInboundPorts(fullConfig);
        PluginHost->Send_ConnectivityEvent({ GetDisplayName(id.connectionId), inboundPorts, Events::Connectivity::Connecting });
        QList<std::tuple<QString, int, QString>> inboundInfo;
        for (const auto &inbound_v : fullConfig["inbounds"].toArray())
        {
            const auto &inbound = inbound_v.toObject();
            inboundInfo.push_back({ inbound["protocol"].toString(), inbound["port"].toInt(), inbound["tag"].toString() });
        }
        //
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
                    auto &kernel = kernels[outProtocol];
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

                        LOG(MODULE_CONNECTION, "Appended originalOutboundTag, inTag, freedomTag into processedOutboundList")
                        pluginProcessedOutboundList.append({ originalOutboundTag, inTag, freedomTag });
                        pluginPort++;
                    }
                    pluginInboundPort.insert("enable_udp", GlobalConfig.inboundConfig.socksUDP ? 1 : 0);
                    LOG(MODULE_CONNECTION, "Sending connection settings to kernel.")
                    kernel->SetConnectionSettings(GlobalConfig.inboundConfig.listenip, pluginInboundPort, outbound["settings"].toObject());
                }
                LOG(MODULE_CONNECTION, "Applying new outbound settings.")
                fullConfig["outbounds"] = new_outbounds;
            }
            //
            // Process routing entries
            {
                LOG(MODULE_CONNECTION, "Started processing route tables.")
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
                            LOG(MODULE_CONNECTION, "Replacing existed plugin outbound rule.")
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
                    LOG(MODULE_CONNECTION, "Adding new plugin outbound rule.")
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
            // ================================================================================================
            //
            currentId = id;
            lastConnectionId = id;
            bool success = true;
            for (auto &kernel : activeKernels.keys())
            {
                LOG(MODULE_CONNECTION, "Starting kernel: " + kernel)
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
                StopConnection();
                return tr("A plugin kernel failed to start. Please check the outbound settings.");
            }
            //
            auto result = vCoreInstance->StartConnection(fullConfig);
            //
            if (!result.has_value())
            {
                emit OnConnected(id);
                PluginHost->Send_ConnectivityEvent(
                    { GetDisplayName(id.connectionId), inboundPorts, Events::Connectivity::Connected });
            }
            else
            {
                PluginHost->Send_ConnectivityEvent(
                    { GetDisplayName(id.connectionId), inboundPorts, Events::Connectivity::Disconnected });
            }
            return result;
        }
        else
        {
            LOG(MODULE_CONNECTION, "Starting kernel without V2ray Integration")
            auto firstOutbound = fullConfig["outbounds"].toArray().first().toObject();
            const auto protocol = firstOutbound["protocol"].toString();
            if (kernels.contains(protocol))
            {
                LOG(MODULE_CONNECTION, "Found existing kernel for: " + protocol)
                auto &kernel = kernels[firstOutbound["protocol"].toString()];
                activeKernels[protocol] = kernel;
                QMap<QString, int> pluginInboundPort;
                for (const auto &[_protocol, _port, _tag] : inboundInfo)
                {
                    pluginInboundPort[_protocol] = _port;
                }
                connect(kernel, &QvPluginKernel::OnKernelStatsAvailable, this, &KernelInstanceHandler::OnStatsDataArrived_p);
                currentId = id;
                lastConnectionId = id;
                pluginInboundPort.insert("enable_udp", GlobalConfig.inboundConfig.socksUDP ? 1 : 0);
                kernel->SetConnectionSettings(GlobalConfig.inboundConfig.listenip, pluginInboundPort, firstOutbound["settings"].toObject());
                bool result = kernel->StartKernel();
                if (result)
                {
                    emit OnConnected(id);
                    PluginHost->Send_ConnectivityEvent(
                        { GetDisplayName(id.connectionId), inboundPorts, Events::Connectivity::Connected });
                    return {};
                }
                else
                {
                    return tr("A plugin kernel failed to start. Please check the outbound settings.");
                }
            }
            else
            {
                LOG(MODULE_CONNECTION, "Starting V2ray without kernel")
                currentId = id;
                lastConnectionId = id;
                auto result = vCoreInstance->StartConnection(fullConfig);
                if (result.has_value())
                {
                    PluginHost->Send_ConnectivityEvent(
                        { GetDisplayName(id.connectionId), inboundPorts, Events::Connectivity::Disconnected });
                }
                else
                {
                    emit OnConnected(id);
                    PluginHost->Send_ConnectivityEvent(
                        { GetDisplayName(id.connectionId), inboundPorts, Events::Connectivity::Connected });
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
        emit OnCrashed(currentId, msg);
        emit OnDisconnected(currentId);
        lastConnectionId = currentId;
        currentId.clear();
    }

    void KernelInstanceHandler::OnKernelLogAvailable_p(const QString &log)
    {
        emit OnKernelLogAvailable(currentId, log);
    }

    void KernelInstanceHandler::StopConnection()
    {
        if (isConnected)
        {
            PluginHost->Send_ConnectivityEvent(
                { GetDisplayName(currentId.connectionId), inboundPorts, Events::Connectivity::Disconnecting });
            if (vCoreInstance->KernelStarted)
            {
                vCoreInstance->StopConnection();
            }
            //
            for (const auto &kernel : activeKernels.keys())
            {
                LOG(MODULE_CONNECTION, "Stopping plugin kernel: " + kernel)
                disconnect(activeKernels[kernel], &QvPluginKernel::OnKernelStatsAvailable, this, &KernelInstanceHandler::OnStatsDataArrived_p);
                activeKernels[kernel]->StopKernel();
            }
            // Copy
            emit OnDisconnected(currentId);
            PluginHost->Send_ConnectivityEvent(
                { GetDisplayName(currentId.connectionId), inboundPorts, Events::Connectivity::Disconnected });
            currentId.clear();
        }
        else
        {
            LOG(MODULE_CORE_HANDLER, "Cannot disconnect when there's nothing connected.")
        }
    }

    void KernelInstanceHandler::OnStatsDataArrived_p(const quint64 uploadSpeed, const quint64 downloadSpeed)
    {
        if (isConnected)
        {
            emit OnStatsDataAvailable(currentId, uploadSpeed, downloadSpeed);
        }
    }
} // namespace Qv2ray::core::handlers
