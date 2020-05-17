#include "KernelInstanceHandler.hpp"

#include "components/port/QvPortDetector.hpp"
#include "core/CoreUtils.hpp"
#include "core/connection/Generation.hpp"

namespace Qv2ray::core::handlers
{
#define isConnected (vCoreInstance->KernelStarted || !activeKernels.empty())
    KernelInstanceHandler::KernelInstanceHandler(QObject *parent) : QObject(parent)
    {
        KernelInstance = this;
        vCoreInstance = new V2rayKernelInstance(this);
        connect(vCoreInstance, &V2rayKernelInstance::OnNewStatsDataArrived, this, &KernelInstanceHandler::OnStatsDataRcvd_p);
        connect(vCoreInstance, &V2rayKernelInstance::OnProcessOutputReadyRead, this, &KernelInstanceHandler::OnKernelLog_p);
        connect(vCoreInstance, &V2rayKernelInstance::OnProcessErrored, this, &KernelInstanceHandler::OnKernelCrashed_p);
        //
        auto kernelList = PluginHost->GetPluginKernels();
        for (const auto &internalName : kernelList.keys())
        {
            auto kernel = kernelList.value(internalName);
            for (const auto &protocol : kernel)
            {
                if (outboundKernelMap.contains(protocol))
                {
                    LOG(MODULE_PLUGINHOST, "Found multiple kernel providers for a protocol: " + protocol)
                    continue;
                }
                outboundKernelMap.insert(protocol, internalName);
            }
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
        //
        // Check inbound port allocation issue.
        QStringList portDetectionErrorMessage;
        auto portDetectionMsg = tr("Another process is using the port required to start the connection:") + NEWLINE + NEWLINE;
        for (const auto &key : inboundPorts.keys())
        {
            auto result = components::port::detectPortTCP(inboundPorts[key]);
            if (!result)
            {
                portDetectionErrorMessage << tr("Port %1 for inbound tag: \"%2\"").arg(inboundPorts[key]).arg(key);
            }
        }
        if (!portDetectionErrorMessage.isEmpty())
        {
            portDetectionMsg += portDetectionErrorMessage.join(NEWLINE);
            return portDetectionMsg;
        }
        //
        PluginHost->Send_ConnectivityEvent({ GetDisplayName(id.connectionId), inboundPorts, Events::Connectivity::Connecting });
        QList<std::tuple<QString, int, QString>> inboundInfo;
        for (const auto &inbound_v : fullConfig["inbounds"].toArray())
        {
            const auto &inbound = inbound_v.toObject();
            inboundInfo.push_back({ inbound["protocol"].toString(), inbound["port"].toInt(), inbound["tag"].toString() });
        }
        //
        using k = Qv2rayPlugin::QvPluginKernel;
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
                OUTBOUNDS processedOutbounds;
                auto _PluginPortAlloc = GlobalConfig.pluginConfig.portAllocationStart;
                //
                /// Key = Original Outbound Tag, Value = QStringList containing new outbound lists.
                for (const auto &outbound_v : fullConfig["outbounds"].toArray())
                {
                    const auto &outbound = outbound_v.toObject();
                    const auto &outProtocol = outbound["protocol"].toString();
                    //
                    if (!outboundKernelMap.contains(outProtocol))
                    {
                        // Normal outbound, or the one without a plugin supported.
                        // Marked as processed.
                        processedOutbounds.push_back(outbound);
                        continue;
                    }
                    {
                        auto kernel = PluginHost->CreatePluginKernel(outboundKernelMap[outProtocol]);
                        LOG(MODULE_CONNECTION, "Get kernel plugin: " + outProtocol)
                        // New object does not need disconnect?
                        // disconnect(kernel, &QvPluginKernel::OnKernelStatsAvailable, this, &KernelInstanceHandler::OnStatsDataArrived_p);
                        activeKernels[outProtocol] = std::move(kernel);
                    }
                    //
                    //
                    QMap<QvPluginKernel::KernelSetting, QVariant> _inboundSettings;
                    const auto &originalOutboundTag = outbound["tag"].toString();
                    for (const auto &[inProtocol, inPort, inTag] : inboundInfo)
                    {
                        // Ignore unsupported protocol.
                        if (!QStringList{ "http", "socks" }.contains(inProtocol))
                            continue;
                        //
                        _inboundSettings[k::KERNEL_HTTP_ENABLED] = _inboundSettings[k::KERNEL_HTTP_ENABLED].toBool() || inProtocol == "http";
                        _inboundSettings[k::KERNEL_SOCKS_ENABLED] = _inboundSettings[k::KERNEL_SOCKS_ENABLED].toBool() || inProtocol == "socks";
                        //
                        _inboundSettings.insert(inProtocol.toLower() == "http" ? k::KERNEL_HTTP_PORT : k::KERNEL_SOCKS_PORT, _PluginPortAlloc);
                        //
                        LOG(MODULE_VCORE,
                            "V2ray Integration: " + QSTRN(_PluginPortAlloc) + " = " + inProtocol + "(" + inTag + ") --> " + outProtocol)
                        //
                        const auto freedomTag = "plugin_" + inTag + "_" + inProtocol + "-" + QSTRN(inPort) + "_" + QSTRN(_PluginPortAlloc);
                        const auto pluginOutSettings = GenerateHTTPSOCKSOut("127.0.0.1", _PluginPortAlloc, false, "", "");
                        const auto direct = GenerateOutboundEntry(inProtocol, pluginOutSettings, {}, {}, "0.0.0.0", freedomTag);
                        //
                        // Add the integration outbound to the list.
                        processedOutbounds.push_back(direct);

                        LOG(MODULE_CONNECTION, "Appended originalOutboundTag, inTag, freedomTag into processedOutboundList")
                        pluginProcessedOutboundList.append({ originalOutboundTag, inTag, freedomTag });
                        _PluginPortAlloc++;
                    }
                    _inboundSettings[k::KERNEL_SOCKS_UDP_ENABLED] = GlobalConfig.inboundConfig.socksSettings.enableUDP;
                    _inboundSettings[k::KERNEL_SOCKS_LOCAL_ADDRESS] = GlobalConfig.inboundConfig.socksSettings.localIP;
                    _inboundSettings[k::KERNEL_LISTEN_ADDRESS] = GlobalConfig.inboundConfig.listenip;
                    LOG(MODULE_CONNECTION, "Sending connection settings to kernel.")
                    activeKernels[outProtocol]->SetConnectionSettings(_inboundSettings, outbound["settings"].toObject());
                }
                LOG(MODULE_CONNECTION, "Applying new outbound settings.")
                fullConfig["outbounds"] = processedOutbounds;
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
            for (auto &[kernel, kernelObject] : activeKernels)
            {
                LOG(MODULE_CONNECTION, "Starting kernel: " + kernel)
                bool status = kernelObject->StartKernel();
                connect(kernelObject.get(), &QvPluginKernel::OnKernelCrashed, this, &KernelInstanceHandler::OnKernelCrashed_p);
                connect(kernelObject.get(), &QvPluginKernel::OnKernelLogAvailable, this, &KernelInstanceHandler::OnKernelLog_p);
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
                PluginHost->Send_ConnectivityEvent({ GetDisplayName(id.connectionId), inboundPorts, Events::Connectivity::Connected });
            }
            else
            {
                PluginHost->Send_ConnectivityEvent({ GetDisplayName(id.connectionId), inboundPorts, Events::Connectivity::Disconnected });
            }
            return result;
        }
        else
        {
            LOG(MODULE_CONNECTION, "Starting kernel without V2ray Integration")
            auto firstOutbound = fullConfig["outbounds"].toArray().first().toObject();
            const auto protocol = firstOutbound["protocol"].toString();
            if (outboundKernelMap.contains(protocol))
            {
                LOG(MODULE_CONNECTION, "Found existing kernel for: " + protocol)
                {
                    auto kernel = PluginHost->CreatePluginKernel(outboundKernelMap[firstOutbound["protocol"].toString()]);
                    activeKernels[protocol] = std::move(kernel);
                }
                QMap<QvPluginKernel::KernelSetting, QVariant> pluginInboundPort;
                for (const auto &[_protocol, _port, _tag] : inboundInfo)
                {
                    pluginInboundPort[k::KERNEL_HTTP_ENABLED] = pluginInboundPort[k::KERNEL_HTTP_ENABLED].toBool() || _protocol == "http";
                    pluginInboundPort[k::KERNEL_SOCKS_ENABLED] = pluginInboundPort[k::KERNEL_SOCKS_ENABLED].toBool() || _protocol == "socks";
                    //
                    pluginInboundPort.insert(_protocol.toLower() == "http" ? k::KERNEL_HTTP_PORT : k::KERNEL_SOCKS_PORT, _port);
                }
                connect(activeKernels[protocol].get(), &QvPluginKernel::OnKernelStatsAvailable, this, &KernelInstanceHandler::OnStatsDataRcvd_p);
                connect(activeKernels[protocol].get(), &QvPluginKernel::OnKernelCrashed, this, &KernelInstanceHandler::OnKernelCrashed_p);
                connect(activeKernels[protocol].get(), &QvPluginKernel::OnKernelLogAvailable, this, &KernelInstanceHandler::OnKernelLog_p);
                currentId = id;
                lastConnectionId = id;
                //
                pluginInboundPort[k::KERNEL_SOCKS_UDP_ENABLED] = GlobalConfig.inboundConfig.socksSettings.enableUDP;
                pluginInboundPort[k::KERNEL_SOCKS_LOCAL_ADDRESS] = GlobalConfig.inboundConfig.socksSettings.localIP;
                pluginInboundPort[k::KERNEL_LISTEN_ADDRESS] = GlobalConfig.inboundConfig.listenip;
                //
                activeKernels[protocol]->SetConnectionSettings(pluginInboundPort, firstOutbound["settings"].toObject());

                bool result = activeKernels[protocol]->StartKernel();
                if (result)
                {
                    emit OnConnected(id);
                    PluginHost->Send_ConnectivityEvent({ GetDisplayName(id.connectionId), inboundPorts, Events::Connectivity::Connected });
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
                    PluginHost->Send_ConnectivityEvent({ GetDisplayName(id.connectionId), inboundPorts, Events::Connectivity::Disconnected });
                }
                else
                {
                    emit OnConnected(id);
                    PluginHost->Send_ConnectivityEvent({ GetDisplayName(id.connectionId), inboundPorts, Events::Connectivity::Connected });
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

    void KernelInstanceHandler::OnKernelLog_p(const QString &log)
    {
        emit OnKernelLogAvailable(currentId, log);
    }

    void KernelInstanceHandler::StopConnection()
    {
        if (isConnected)
        {
            PluginHost->Send_ConnectivityEvent({ GetDisplayName(currentId.connectionId), inboundPorts, Events::Connectivity::Disconnecting });
            if (vCoreInstance->KernelStarted)
            {
                vCoreInstance->StopConnection();
            }
            //
            for (const auto &[kernel, kernelObject] : activeKernels)
            {
                LOG(MODULE_CONNECTION, "Stopping plugin kernel: " + kernel)
                kernelObject->StopKernel();
            }
            activeKernels.clear();
            // Copy
            emit OnDisconnected(currentId);
            PluginHost->Send_ConnectivityEvent({ GetDisplayName(currentId.connectionId), inboundPorts, Events::Connectivity::Disconnected });
            currentId.clear();
        }
        else
        {
            LOG(MODULE_CORE_HANDLER, "Cannot disconnect when there's nothing connected.")
        }
    }

    void KernelInstanceHandler::OnStatsDataRcvd_p(const quint64 uploadSpeed, const quint64 downloadSpeed)
    {
        if (isConnected)
        {
            emit OnStatsDataAvailable(currentId, uploadSpeed, downloadSpeed);
        }
    }
} // namespace Qv2ray::core::handlers
