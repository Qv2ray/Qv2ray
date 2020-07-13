#include "KernelInstanceHandler.hpp"

#include "common/QvHelpers.hpp"
#include "components/port/QvPortDetector.hpp"
#include "core/connection/Generation.hpp"

namespace Qv2ray::core::handler
{
#define isConnected (vCoreInstance->KernelStarted || !activeKernels.empty())
    KernelInstanceHandler::KernelInstanceHandler(QObject *parent) : QObject(parent)
    {
        KernelInstance = this;
        vCoreInstance = new V2RayKernelInstance(this);
        connect(vCoreInstance, &V2RayKernelInstance::OnNewStatsDataArrived, this, &KernelInstanceHandler::OnV2RayStatsDataRcvd_p);
        connect(vCoreInstance, &V2RayKernelInstance::OnProcessOutputReadyRead, this, &KernelInstanceHandler::OnKernelLog_p);
        connect(vCoreInstance, &V2RayKernelInstance::OnProcessErrored, this, &KernelInstanceHandler::OnKernelCrashed_p);
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

    std::optional<QString> KernelInstanceHandler::CheckPort(const QMap<QString, InboundInfoObject> &info, int plugins)
    {
        //
        // Check inbound port allocation issue.
        QStringList portDetectionErrorMessage;
        auto portDetectionMsg = tr("Another process is using the port required to start the connection:") + NEWLINE + NEWLINE;
        for (const auto &key : info.keys())
        {
            auto result = components::port::CheckTCPPortStatus(info[key].listenIp, info[key].port);
            if (!result)
            {
                portDetectionErrorMessage << tr("Port: %1 for listening IP: %2 for inbound tag: \"%3\"") //
                                                 .arg(info[key].port)
                                                 .arg(info[key].listenIp)
                                                 .arg(key);
            }
        }
        if (GlobalConfig.pluginConfig.v2rayIntegration)
        {
            for (int i = 0; i < plugins; i++)
            {
                auto result = components::port::CheckTCPPortStatus("127.0.0.1", GlobalConfig.pluginConfig.portAllocationStart + i);
                if (!result)
                {
                    portDetectionErrorMessage << tr("Port: %1 for listening IP: 127.0.0.1 for plugin integration.")
                                                     .arg(GlobalConfig.pluginConfig.portAllocationStart + i);
                }
            }
        }
        if (!portDetectionErrorMessage.isEmpty())
        {
            portDetectionMsg += portDetectionErrorMessage.join(NEWLINE);
            return portDetectionMsg;
        }
        else
        {
            return std::nullopt;
        }
    }

    std::optional<QString> KernelInstanceHandler::StartConnection(const ConnectionGroupPair &id, CONFIGROOT fullConfig)
    {
        StopConnection();
        inboundInfo = GetConfigInboundInfo(fullConfig);
        //
        const auto inboundPorts = GetInboundPorts();
        const auto inboundHosts = GetInboundHosts();
        PluginHost->Send_ConnectivityEvent({ GetDisplayName(id.connectionId), inboundPorts, Events::Connectivity::Connecting });
        // QList<std::tuple<QString, int, QString>> inboundInfo;
        // for (const auto &inbound_v : fullConfig["inbounds"].toArray())
        //{
        //    const auto &inbound = inbound_v.toObject();
        //    inboundInfo.push_back({ inbound["protocol"].toString(), inbound["port"].toInt(), inbound["tag"].toString() });
        //}
        //
        using _k = Qv2rayPlugin::QvPluginKernel;
        if (GlobalConfig.pluginConfig.v2rayIntegration)
        {
            //
            // Process outbounds.
            OUTBOUNDS processedOutbounds;
            auto pluginPort = GlobalConfig.pluginConfig.portAllocationStart;
            //
            for (auto i = 0; i < fullConfig["outbounds"].toArray().count(); i++)
            {
                auto outbound = QJsonIO::GetValue(fullConfig, "outbounds", i).toObject();
                const auto outProtocol = outbound["protocol"].toString();
                //
                if (!outboundKernelMap.contains(outProtocol))
                {
                    // Normal outbound, or the one without a plugin supported.
                    // Marked as processed.
                    processedOutbounds.push_back(outbound);
                    LOG(MODULE_CONNECTION, "Outbound protocol " + outProtocol + " is not a registered plugin outbound.")
                    continue;
                }
                LOG(MODULE_CONNECTION, "Creating kernel plugin instance for protocol" + outProtocol)
                auto kernel = PluginHost->CreatePluginKernel(outboundKernelMap[outProtocol]);
                // New object does not need disconnect?
                // disconnect(kernel, &QvPluginKernel::OnKernelStatsAvailable, this, &KernelInstanceHandler::OnStatsDataArrived_p);
                //
                QMap<QvPluginKernel::KernelSetting, QVariant> _inboundSettings;

                LOG(MODULE_VCORE, "V2RayIntegration: " + QSTRN(pluginPort) + " = " + outProtocol)
                _inboundSettings[_k::KERNEL_HTTP_ENABLED] = false;
                _inboundSettings[_k::KERNEL_SOCKS_ENABLED] = true;
                _inboundSettings[_k::KERNEL_SOCKS_PORT] = pluginPort;
                _inboundSettings[_k::KERNEL_SOCKS_UDP_ENABLED] = GlobalConfig.inboundConfig.socksSettings.enableUDP;
                _inboundSettings[_k::KERNEL_SOCKS_LOCAL_ADDRESS] = GlobalConfig.inboundConfig.socksSettings.localIP;
                _inboundSettings[_k::KERNEL_LISTEN_ADDRESS] = "127.0.0.1";
                LOG(MODULE_CONNECTION, "Sending connection settings to kernel.")
                kernel->SetConnectionSettings(_inboundSettings, outbound["settings"].toObject());
                activeKernels.push_back({ outProtocol, std::move(kernel) });
                //
                const auto pluginOutSettings = GenerateHTTPSOCKSOut("127.0.0.1", pluginPort, false, "", "");
                //
                const auto pluginOut = GenerateOutboundEntry("socks", pluginOutSettings, {}, {}, "0.0.0.0", outbound["tag"].toString());
                //
                // Add the integration outbound to the list.
                processedOutbounds.push_back(pluginOut);
                pluginPort++;
            }
            LOG(MODULE_CONNECTION, "Applying new outbound settings.")
            fullConfig["outbounds"] = processedOutbounds;
            RemoveEmptyMuxFilter(fullConfig);
        }

        //
        // ======================================================================= Start Kernels
        //
        {
            const auto portResult = CheckPort(inboundInfo, activeKernels.size());
            if (portResult)
            {
                LOG(MODULE_CONNECTION, ACCESS_OPTIONAL_VALUE(portResult))
                return portResult;
            }
            auto firstOutbound = fullConfig["outbounds"].toArray().first().toObject();
            const auto firstOutboundProtocol = firstOutbound["protocol"].toString();
            if (GlobalConfig.pluginConfig.v2rayIntegration)
            {
                LOG(MODULE_VCORE, "Starting kernels with V2RayIntegration.")
                bool hasAllKernelStarted = true;
                for (auto &[outboundProtocol, kernelObject] : activeKernels)
                {
                    LOG(MODULE_CONNECTION, "Starting kernel for protocol: " + outboundProtocol)
                    bool status = kernelObject->StartKernel();
                    connect(kernelObject.get(), &QvPluginKernel::OnKernelCrashed, this, &KernelInstanceHandler::OnKernelCrashed_p,
                            Qt::QueuedConnection);
                    connect(kernelObject.get(), &QvPluginKernel::OnKernelLogAvailable, this, &KernelInstanceHandler::OnKernelLog_p,
                            Qt::QueuedConnection);
                    hasAllKernelStarted = hasAllKernelStarted && status;
                    if (!status)
                    {
                        LOG(MODULE_CONNECTION, "Plugin Kernel: " + outboundProtocol + " failed to start.")
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
                // Also start V2Ray-core.
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
            else if (outboundKernelMap.contains(firstOutboundProtocol))
            {
                // Connections without V2Ray Integration will have and ONLY have ONE kernel.
                LOG(MODULE_CONNECTION, "Starting kernel " + firstOutboundProtocol + " without V2Ray Integration")
                {
                    auto kernel = PluginHost->CreatePluginKernel(outboundKernelMap[firstOutbound["protocol"].toString()]);
                    activeKernels.push_back({ firstOutboundProtocol, std::move(kernel) });
                }
                Q_ASSERT(activeKernels.size() == 1);
#define theKernel (activeKernels.front().second.get())
                connect(theKernel, &QvPluginKernel::OnKernelStatsAvailable, this, &KernelInstanceHandler::OnPluginStatsDataRcvd_p,
                        Qt::QueuedConnection);
                connect(theKernel, &QvPluginKernel::OnKernelCrashed, this, &KernelInstanceHandler::OnKernelCrashed_p, Qt::QueuedConnection);
                connect(theKernel, &QvPluginKernel::OnKernelLogAvailable, this, &KernelInstanceHandler::OnKernelLog_p, Qt::QueuedConnection);
                currentId = id;
                //
                QMap<QvPluginKernel::KernelSetting, QVariant> pluginSettings;

                for (const auto &v : inboundInfo)
                {
                    if (v.protocol != "http" && v.protocol != "socks")
                        continue;
                    pluginSettings[_k::KERNEL_HTTP_ENABLED] = pluginSettings[_k::KERNEL_HTTP_ENABLED].toBool() || v.protocol == "http";
                    pluginSettings[_k::KERNEL_SOCKS_ENABLED] = pluginSettings[_k::KERNEL_SOCKS_ENABLED].toBool() || v.protocol == "socks";
                    pluginSettings.insert(v.protocol.toLower() == "http" ? _k::KERNEL_HTTP_PORT : _k::KERNEL_SOCKS_PORT, v.port);
                }

                pluginSettings[_k::KERNEL_SOCKS_UDP_ENABLED] = GlobalConfig.inboundConfig.socksSettings.enableUDP;
                pluginSettings[_k::KERNEL_SOCKS_LOCAL_ADDRESS] = GlobalConfig.inboundConfig.socksSettings.localIP;
                pluginSettings[_k::KERNEL_LISTEN_ADDRESS] = GlobalConfig.inboundConfig.listenip;
                //
                theKernel->SetConnectionSettings(pluginSettings, firstOutbound["settings"].toObject());
                bool kernelStarted = theKernel->StartKernel();
#undef theKernel
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
                LOG(MODULE_CONNECTION, "Starting V2Ray without plugin.")
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
            const auto inboundPorts = GetInboundPorts();
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

    void KernelInstanceHandler::OnV2RayStatsDataRcvd_p(const QMap<StatisticsType, QvStatsSpeed> &data)
    {
        if (isConnected)
        {
            emit OnStatsDataAvailable(currentId, data);
        }
    }

    void KernelInstanceHandler::OnPluginStatsDataRcvd_p(const long uploadSpeed, const long downloadSpeed)
    {
        OnV2RayStatsDataRcvd_p({ { API_OUTBOUND_PROXY, { uploadSpeed, downloadSpeed } } });
    }
} // namespace Qv2ray::core::handler
