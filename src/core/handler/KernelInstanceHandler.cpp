#include "KernelInstanceHandler.hpp"

#include "components/port/QvPortDetector.hpp"
#include "core/connection/Generation.hpp"
#include "utils/QvHelpers.hpp"

#define QV_MODULE_NAME "KernelHandler"

namespace Qv2ray::core::handler
{
#define isConnected (vCoreInstance->IsKernelRunning() || !activeKernels.empty())
    KernelInstanceHandler::KernelInstanceHandler(QObject *parent) : QObject(parent)
    {
        KernelInstance = this;
        vCoreInstance = new V2RayKernelInstance(this);
        connect(vCoreInstance, &V2RayKernelInstance::OnNewStatsDataArrived, this, &KernelInstanceHandler::OnV2RayStatsDataRcvd_p);
        connect(vCoreInstance, &V2RayKernelInstance::OnProcessOutputReadyRead, this, &KernelInstanceHandler::OnV2RayKernelLog_p);
        connect(vCoreInstance, &V2RayKernelInstance::OnProcessErrored, this, &KernelInstanceHandler::OnKernelCrashed_p);
        //
        auto kernelList = PluginHost->UsablePlugins();
        for (const auto &internalName : kernelList)
        {
            const auto info = PluginHost->GetPlugin(internalName);
            if (!info->hasComponent(COMPONENT_KERNEL))
                continue;
            auto kernel = info->pluginInterface->GetKernel();
            for (const auto &protocol : kernel->GetKernelProtocols())
            {
                if (kernelMap.contains(protocol))
                {
                    LOG("Found multiple kernel providers for a protocol: " + protocol);
                    continue;
                }
                kernelMap.insert(protocol, internalName);
            }
        }
    }

    KernelInstanceHandler::~KernelInstanceHandler()
    {
        StopConnection();
    }

    std::optional<QString> KernelInstanceHandler::CheckPort(const QMap<QString, ProtocolSettingsInfoObject> &info, int plugins)
    {
        QStringList portDetectionErrorMessage;
        auto portDetectionMsg = tr("There are other processes occupying the ports necessary to start the connection:") + NEWLINE + NEWLINE;
        for (const auto &key : info.keys())
        {
            const auto result = components::port::CheckTCPPortStatus(info[key].address, info[key].port);
            if (!result)
                portDetectionErrorMessage << tr("Endpoint: %1:%2 for inbound: \"%3\"").arg(info[key].address).arg(info[key].port).arg(key);
        }
        if (GlobalConfig.pluginConfig.v2rayIntegration)
        {
            for (auto i = 0; i < plugins; i++)
            {
                const auto thisPort = GlobalConfig.pluginConfig.portAllocationStart + i;
                const auto result = components::port::CheckTCPPortStatus("127.0.0.1", thisPort);
                if (!result)
                    portDetectionErrorMessage << tr("Local port: %1 for plugin integration.").arg(thisPort);
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
        inboundInfo = GetInboundInfo(fullConfig);
        //
        const auto inboundPorts = GetInboundPorts();
        const auto inboundHosts = GetInboundHosts();
        PluginHost->SendEvent({ GetDisplayName(id.connectionId), inboundPorts, Events::Connectivity::Connecting });
        // QList<std::tuple<QString, int, QString>> inboundInfo;
        // for (const auto &inbound_v : fullConfig["inbounds"].toArray())
        //{
        //    const auto &inbound = inbound_v.toObject();
        //    inboundInfo.push_back({ inbound["protocol"].toString(), inbound["port"].toInt(), inbound["tag"].toString() });
        //}
        //
        if (GlobalConfig.pluginConfig.v2rayIntegration)
        {
            // Process outbounds.
            OUTBOUNDS processedOutbounds;
            auto pluginPort = GlobalConfig.pluginConfig.portAllocationStart;
            //
            for (auto i = 0; i < fullConfig["outbounds"].toArray().count(); i++)
            {
                auto outbound = QJsonIO::GetValue(fullConfig, "outbounds", i).toObject();
                const auto outProtocol = outbound["protocol"].toString();
                //
                if (!kernelMap.contains(outProtocol))
                {
                    // Normal outbound, or the one without a plugin supported.
                    // Marked as processed.
                    processedOutbounds.push_back(outbound);
                    LOG("Outbound protocol " + outProtocol + " is not a registered plugin outbound.");
                    continue;
                }
                LOG("Creating kernel plugin instance for protocol" + outProtocol);
                auto kernel = PluginHost->GetPlugin(kernelMap[outProtocol])->pluginInterface->GetKernel()->CreateKernel();
                // New object does not need disconnect?
                // disconnect(kernel, &QvPluginKernel::OnKernelStatsAvailable, this, &KernelInstanceHandler::OnStatsDataArrived_p);
                //
                QMap<KernelOptionFlags, QVariant> _inboundSettings;

                LOG("V2RayIntegration: " + QSTRN(pluginPort) + " = " + outProtocol);
                _inboundSettings[KERNEL_HTTP_ENABLED] = false;
                _inboundSettings[KERNEL_SOCKS_ENABLED] = true;
                _inboundSettings[KERNEL_SOCKS_PORT] = pluginPort;
                _inboundSettings[KERNEL_SOCKS_UDP_ENABLED] = GlobalConfig.inboundConfig.socksSettings.enableUDP;
                _inboundSettings[KERNEL_SOCKS_LOCAL_ADDRESS] = GlobalConfig.inboundConfig.socksSettings.localIP;
                _inboundSettings[KERNEL_LISTEN_ADDRESS] = "127.0.0.1";
                LOG("Sending connection settings to kernel.");
                kernel->SetConnectionSettings(_inboundSettings, outbound["settings"].toObject());
                activeKernels.push_back({ outProtocol, std::move(kernel) });
                //
                const auto pluginOutSettings = GenerateHTTPSOCKSOut("127.0.0.1", pluginPort, false, "", "");
                //
                const auto pluginOut = GenerateOutboundEntry(outbound["tag"].toString(), "socks", pluginOutSettings, {});
                //
                // Add the integration outbound to the list.
                processedOutbounds.push_back(pluginOut);
                pluginPort++;
            }
            LOG("Applying new outbound settings.");
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
                LOG(*portResult);
                return portResult;
            }
            auto firstOutbound = fullConfig["outbounds"].toArray().first().toObject();
            const auto firstOutboundProtocol = firstOutbound["protocol"].toString();
            if (GlobalConfig.pluginConfig.v2rayIntegration)
            {
                LOG("Starting kernels with V2RayIntegration.");
                bool hasAllKernelStarted = true;
                for (auto &[outboundProtocol, kernelObject] : activeKernels)
                {
                    LOG("Starting kernel for protocol: " + outboundProtocol);
                    bool status = kernelObject->StartKernel();
                    connect(kernelObject.get(), &PluginKernel::OnKernelCrashed, this, &KernelInstanceHandler::OnKernelCrashed_p,
                            Qt::QueuedConnection);
                    connect(kernelObject.get(), &PluginKernel::OnKernelLogAvailable, this, &KernelInstanceHandler::OnPluginKernelLog_p,
                            Qt::QueuedConnection);
                    hasAllKernelStarted = hasAllKernelStarted && status;
                    if (!status)
                    {
                        LOG("Plugin Kernel: " + outboundProtocol + " failed to start.");
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
                    PluginHost->SendEvent({ GetDisplayName(id.connectionId), inboundPorts, Events::Connectivity::Disconnected });
                    return result;
                }
                else
                {
                    emit OnConnected(id);
                    PluginHost->SendEvent({ GetDisplayName(id.connectionId), inboundPorts, Events::Connectivity::Connected });
                }
            }
            else if (kernelMap.contains(firstOutboundProtocol))
            {
                // Connections without V2Ray Integration will have and ONLY have ONE kernel.
                LOG("Starting kernel " + firstOutboundProtocol + " without V2Ray Integration");
                {
                    auto kernel =
                        PluginHost->GetPlugin(kernelMap[firstOutbound["protocol"].toString()])->pluginInterface->GetKernel()->CreateKernel();
                    activeKernels.push_back({ firstOutboundProtocol, std::move(kernel) });
                }
                Q_ASSERT(activeKernels.size() == 1);
#define theKernel (activeKernels.front().second.get())
                connect(theKernel, &PluginKernel::OnKernelStatsAvailable, this, &KernelInstanceHandler::OnPluginStatsDataRcvd_p,
                        Qt::QueuedConnection);
                connect(theKernel, &PluginKernel::OnKernelCrashed, this, &KernelInstanceHandler::OnKernelCrashed_p, Qt::QueuedConnection);
                connect(theKernel, &PluginKernel::OnKernelLogAvailable, this, &KernelInstanceHandler::OnPluginKernelLog_p, Qt::QueuedConnection);
                currentId = id;
                //
                QMap<KernelOptionFlags, QVariant> pluginSettings;

                for (const auto &v : inboundInfo)
                {
                    if (v.protocol != "http" && v.protocol != "socks")
                        continue;
                    pluginSettings[KERNEL_HTTP_ENABLED] = pluginSettings[KERNEL_HTTP_ENABLED].toBool() || v.protocol == "http";
                    pluginSettings[KERNEL_SOCKS_ENABLED] = pluginSettings[KERNEL_SOCKS_ENABLED].toBool() || v.protocol == "socks";
                    pluginSettings.insert(v.protocol.toLower() == "http" ? KERNEL_HTTP_PORT : KERNEL_SOCKS_PORT, v.port);
                }

                pluginSettings[KERNEL_SOCKS_UDP_ENABLED] = GlobalConfig.inboundConfig.socksSettings.enableUDP;
                pluginSettings[KERNEL_SOCKS_LOCAL_ADDRESS] = GlobalConfig.inboundConfig.socksSettings.localIP;
                pluginSettings[KERNEL_LISTEN_ADDRESS] = GlobalConfig.inboundConfig.listenip;
                //
                theKernel->SetConnectionSettings(pluginSettings, firstOutbound["settings"].toObject());
                bool kernelStarted = theKernel->StartKernel();
#undef theKernel
                if (kernelStarted)
                {
                    emit OnConnected(id);
                    PluginHost->SendEvent({ GetDisplayName(id.connectionId), inboundPorts, Events::Connectivity::Connected });
                }
                else
                {
                    return tr("A plugin kernel failed to start. Please check the outbound settings.");
                    StopConnection();
                }
            }
            else
            {
                LOG("Starting V2Ray without plugin.");
                currentId = id;
                auto result = vCoreInstance->StartConnection(fullConfig);
                if (result.has_value())
                {
                    PluginHost->SendEvent({ GetDisplayName(id.connectionId), inboundPorts, Events::Connectivity::Disconnected });
                    StopConnection();
                    return result;
                }
                else
                {
                    emit OnConnected(id);
                    PluginHost->SendEvent({ GetDisplayName(id.connectionId), inboundPorts, Events::Connectivity::Connected });
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

    void KernelInstanceHandler::emitLogMessage(const QString &l)
    {
        emit OnKernelLogAvailable(currentId, l);
    }

    void KernelInstanceHandler::OnPluginKernelLog_p(const QString &log)
    {
        if (pluginLogPrefixPadding <= 0)
            for (const auto &[_, kernel] : activeKernels)
                pluginLogPrefixPadding = std::max(pluginLogPrefixPadding, kernel->GetKernelName().length());

        const auto kernel = static_cast<PluginKernel *>(sender());
        const auto name = kernel ? kernel->GetKernelName() : "UNKNOWN";
        for (const auto &line : SplitLines(log))
            emitLogMessage(QString("[%1] ").arg(name, pluginLogPrefixPadding) + line.trimmed());
    }

    void KernelInstanceHandler::OnV2RayKernelLog_p(const QString &log)
    {
        for (auto line : SplitLines(log))
            emitLogMessage(line.trimmed());
    }

    void KernelInstanceHandler::StopConnection()
    {
        if (isConnected)
        {
            const auto inboundPorts = GetInboundPorts();
            PluginHost->SendEvent({ GetDisplayName(currentId.connectionId), inboundPorts, Events::Connectivity::Disconnecting });
            if (vCoreInstance->IsKernelRunning())
            {
                vCoreInstance->StopConnection();
            }
            for (const auto &[kernel, kernelObject] : activeKernels)
            {
                LOG("Stopping plugin kernel: " + kernel);
                kernelObject->StopKernel();
            }
            pluginLogPrefixPadding = 0;
            emit OnDisconnected(currentId);
            PluginHost->SendEvent({ GetDisplayName(currentId.connectionId), inboundPorts, Events::Connectivity::Disconnected });
        }
        else
        {
            LOG("Cannot disconnect when there's nothing connected.");
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
