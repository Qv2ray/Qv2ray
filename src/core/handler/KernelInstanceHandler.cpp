#include "KernelInstanceHandler.hpp"

#include "components/port/QvPortDetector.hpp"
#include "core/CoreUtils.hpp"
#include "core/connection/Generation.hpp"

namespace Qv2ray::core::handler
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
        StopConnection();
    }

    std::optional<QString> KernelInstanceHandler::StartConnection(const ConnectionGroupPair &id, CONFIGROOT fullConfig)
    {
        StopConnection();
        inboundPorts = GetConfigInboundPorts(fullConfig);
        inboundHosts = GetConfigInboundHosts(fullConfig);
        //
        // Check inbound port allocation issue.
        QStringList portDetectionErrorMessage;
        auto portDetectionMsg = tr("Another process is using the port required to start the connection:") + NEWLINE + NEWLINE;
        for (const auto &key : inboundPorts.keys())
        {
            auto result = components::port::CheckTCPPortStatus(inboundHosts[key], inboundPorts[key]);
            if (!result)
            {
                portDetectionErrorMessage << tr("Port: %1 for listening IP: %2 for inbound tag: \"%3\"") //
                                                 .arg(inboundPorts[key])
                                                 .arg(inboundHosts[key])
                                                 .arg(key);
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
            //
            // Process outbounds.
            //
            {
                OUTBOUNDS processedOutbounds;
                auto pluginPort = GlobalConfig.pluginConfig.portAllocationStart;
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
                        LOG(MODULE_CONNECTION, "Outbound protocol " + outProtocol + " is not a registered plugin outbound.")
                        continue;
                    }
                    {
                        LOG(MODULE_CONNECTION, "Creating kernel plugin instance for protocol" + outProtocol)
                        auto kernel = PluginHost->CreatePluginKernel(outboundKernelMap[outProtocol]);
                        // New object does not need disconnect?
                        // disconnect(kernel, &QvPluginKernel::OnKernelStatsAvailable, this, &KernelInstanceHandler::OnStatsDataArrived_p);
                        activeKernels[outProtocol] = std::move(kernel);
                    }
                    //
                    //
                    QMap<QvPluginKernel::KernelSetting, QVariant> _inboundSettings;

                    _inboundSettings[k::KERNEL_HTTP_ENABLED] = false;
                    _inboundSettings[k::KERNEL_SOCKS_ENABLED] = true;
                    _inboundSettings.insert(k::KERNEL_SOCKS_PORT, pluginPort);
                    LOG(MODULE_VCORE, "V2rayIntegration: " + QSTRN(pluginPort) + "=" + outProtocol)

                    const auto pluginOutSettings = GenerateHTTPSOCKSOut("127.0.0.1", pluginPort, false, "", "");
                    const auto pluginOut = GenerateOutboundEntry("socks", pluginOutSettings, {}, {}, "0.0.0.0", outbound["tag"].toString());
                    //
                    // Add the integration outbound to the list.
                    processedOutbounds.push_back(pluginOut);

                    pluginPort++;

                    _inboundSettings[k::KERNEL_SOCKS_UDP_ENABLED] = GlobalConfig.inboundConfig.socksSettings.enableUDP;
                    _inboundSettings[k::KERNEL_SOCKS_LOCAL_ADDRESS] = GlobalConfig.inboundConfig.socksSettings.localIP;
                    _inboundSettings[k::KERNEL_LISTEN_ADDRESS] = "127.0.0.1";
                    LOG(MODULE_CONNECTION, "Sending connection settings to kernel.")
                    activeKernels[outProtocol]->SetConnectionSettings(_inboundSettings, outbound["settings"].toObject());
                }
                LOG(MODULE_CONNECTION, "Applying new outbound settings.")
                fullConfig["outbounds"] = processedOutbounds;
            }
            //
            // Process routing entries
            //
            // No needs to process routing entries since each plugin is map to an unique outbound with the same tag.
            //
            // ================================================================================================
            //
            bool hasAllKernelStarted = true;
            for (auto &[kernel, kernelObject] : activeKernels)
            {
                LOG(MODULE_CONNECTION, "Starting kernel: " + kernel)
                bool status = kernelObject->StartKernel();
                connect(kernelObject.get(), &QvPluginKernel::OnKernelCrashed, this, &KernelInstanceHandler::OnKernelCrashed_p);
                connect(kernelObject.get(), &QvPluginKernel::OnKernelLogAvailable, this, &KernelInstanceHandler::OnKernelLog_p);
                hasAllKernelStarted = hasAllKernelStarted && status;
                if (!status)
                {
                    LOG(MODULE_CONNECTION, "Plugin Kernel: " + kernel + " failed to start.")
                    break;
                }
            }
            if (!hasAllKernelStarted)
            {
                StopConnection();
                return tr("A plugin kernel failed to start. Please check the outbound settings.");
            }
            currentId = id;
            //
            // Also start V2ray-core.
            auto result = vCoreInstance->StartConnection(fullConfig);
            //
            if (result.has_value())
            {
                StopConnection();
                PluginHost->Send_ConnectivityEvent({ GetDisplayName(id.connectionId), inboundPorts, Events::Connectivity::Disconnected });
                return result;
            }
            else
            {
                emit OnConnected(id);
                PluginHost->Send_ConnectivityEvent({ GetDisplayName(id.connectionId), inboundPorts, Events::Connectivity::Connected });
            }
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
                    if (_protocol != "http" && _protocol != "socks")
                        continue;
                    pluginInboundPort[k::KERNEL_HTTP_ENABLED] = pluginInboundPort[k::KERNEL_HTTP_ENABLED].toBool() || _protocol == "http";
                    pluginInboundPort[k::KERNEL_SOCKS_ENABLED] = pluginInboundPort[k::KERNEL_SOCKS_ENABLED].toBool() || _protocol == "socks";
                    pluginInboundPort.insert(_protocol.toLower() == "http" ? k::KERNEL_HTTP_PORT : k::KERNEL_SOCKS_PORT, _port);
                }
                connect(activeKernels[protocol].get(), &QvPluginKernel::OnKernelStatsAvailable, this, &KernelInstanceHandler::OnStatsDataRcvd_p);
                connect(activeKernels[protocol].get(), &QvPluginKernel::OnKernelCrashed, this, &KernelInstanceHandler::OnKernelCrashed_p);
                connect(activeKernels[protocol].get(), &QvPluginKernel::OnKernelLogAvailable, this, &KernelInstanceHandler::OnKernelLog_p);
                currentId = id;
                //
                pluginInboundPort[k::KERNEL_SOCKS_UDP_ENABLED] = GlobalConfig.inboundConfig.socksSettings.enableUDP;
                pluginInboundPort[k::KERNEL_SOCKS_LOCAL_ADDRESS] = GlobalConfig.inboundConfig.socksSettings.localIP;
                pluginInboundPort[k::KERNEL_LISTEN_ADDRESS] = GlobalConfig.inboundConfig.listenip;
                //
                activeKernels[protocol]->SetConnectionSettings(pluginInboundPort, firstOutbound["settings"].toObject());

                bool kernelStarted = activeKernels[protocol]->StartKernel();
                if (kernelStarted)
                {
                    emit OnConnected(id);
                    PluginHost->Send_ConnectivityEvent({ GetDisplayName(id.connectionId), inboundPorts, Events::Connectivity::Connected });
                }
                else
                {
                    return tr("A plugin kernel failed to start. Please check the outbound settings.");
                    StopConnection();
                }
            }
            else
            {
                LOG(MODULE_CONNECTION, "Starting V2ray without kernel")
                currentId = id;
                auto result = vCoreInstance->StartConnection(fullConfig);
                if (result.has_value())
                {
                    PluginHost->Send_ConnectivityEvent({ GetDisplayName(id.connectionId), inboundPorts, Events::Connectivity::Disconnected });
                    StopConnection();
                    return result;
                }
                else
                {
                    emit OnConnected(id);
                    PluginHost->Send_ConnectivityEvent({ GetDisplayName(id.connectionId), inboundPorts, Events::Connectivity::Connected });
                }
            }
        }
        // Return
        return std::nullopt;
    }

    void KernelInstanceHandler::OnKernelCrashed_p(const QString &msg)
    {
        emit OnCrashed(currentId, msg);
        emit OnDisconnected(currentId);
        StopConnection();
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
            for (const auto &[kernel, kernelObject] : activeKernels)
            {
                LOG(MODULE_CONNECTION, "Stopping plugin kernel: " + kernel)
                kernelObject->StopKernel();
            }
            emit OnDisconnected(currentId);
            PluginHost->Send_ConnectivityEvent({ GetDisplayName(currentId.connectionId), inboundPorts, Events::Connectivity::Disconnected });
        }
        else
        {
            LOG(MODULE_CORE_HANDLER, "Cannot disconnect when there's nothing connected.")
        }
        currentId.clear();
        activeKernels.clear();
    }

    void KernelInstanceHandler::OnStatsDataRcvd_p(const quint64 uploadSpeed, const quint64 downloadSpeed)
    {
        if (isConnected)
        {
            emit OnStatsDataAvailable(currentId, uploadSpeed, downloadSpeed);
        }
    }
} // namespace Qv2ray::core::handler
