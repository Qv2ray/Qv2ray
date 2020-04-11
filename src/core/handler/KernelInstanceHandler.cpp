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
        pluginKernels.clear();
        this->root = root;
        bool isComplex = IsComplexConfig(root);
        auto fullConfig = GenerateRuntimeConfig(root);
        //
        if (!isComplex)
        {
            QList<std::tuple<QString, int, QString>> inboundInfo;
            for (const auto &inbound_v : fullConfig["inbounds"].toArray())
            {
                const auto &inbound = inbound_v.toObject();
                inboundInfo.push_back({ inbound["protocol"].toString(), inbound["port"].toInt(), inbound["tag"].toString() });
            }
            //
            auto kernels = PluginHost->GetPluginKernels();

            auto pluginPort = 18900;
            OUTBOUNDS new_outbounds;
            auto rules = fullConfig["routing"].toObject()["rules"].toArray();
            QJsonArray newRules;
            QJsonArray newRulesAppend;

            for (const auto &outbound_v : fullConfig["outbounds"].toArray())
            {
                const auto &outbound = outbound_v.toObject();
                const auto &outProtocol = outbound["protocol"].toString();
                const auto &outTag = outbound["tag"].toString();
                //
                if (!kernels.contains(outProtocol))
                {
                    new_outbounds.push_back(outbound);
                    continue;
                }
                LOG(MODULE_CONNECTION, "Get kernel plugin: " + outProtocol)
                auto kernel = kernels[outProtocol].get();
                connect(kernel, &QvPluginKernel::OnKernelCrashed, this, &KernelInstanceHandler::OnKernelCrashed_p);
                // connect(kernel, &QvPluginKernel::OnKernelStatsAvailable, this, &KernelInstanceHandler::OnStatsDataArrived_p);
                connect(kernel, &QvPluginKernel::OnKernelLogAvaliable, this, &KernelInstanceHandler::OnKernelLogAvailable_p);
                pluginKernels.insert(outProtocol, kernel);
                //
                auto pluginOutboundSettings = outbound["settings"].toObject();
                //
                QMap<QString, int> pluginInboundPort;
                for (const auto &[inProtocol, inPort, inTag] : inboundInfo)
                {
                    if (!QStringList{ "http", "socks" }.contains(inProtocol))
                        continue;
                    //
                    pluginInboundPort.insert(inProtocol, pluginPort);
                    LOG(MODULE_VCORE, "Plugin Integration: " + QSTRN(pluginPort) + " = " + inProtocol + "(" + inTag + ") --> " + outProtocol)
                    const auto &freedomTag = "plugin_" + inTag + "_" + inProtocol + ":" + QSTRN(inPort) + "_" + QSTRN(pluginPort);
                    const auto &pluginOutSettings = GenerateHTTPSOCKSOut("127.0.0.1", pluginPort, false, "", "");
                    const auto &direct = GenerateOutboundEntry(inProtocol, pluginOutSettings, {}, {}, "0.0.0.0", freedomTag);
                    //
                    //
                    //
                    // Add the integration outbound to the list.
                    new_outbounds.push_back(direct);
                    //
                    bool hasRuleProcessed = false;
                    auto copyRules = rules;
                    for (auto i = 0; i < rules.count(); i++)
                    {
                        const auto &rule = rules.at(i).toObject();
                        if (rule["outboundTag"].toString() == outTag)
                        {
                            auto newRule = rule;
                            newRule["outboundTag"] = freedomTag;
                            newRule["inboundTag"] = QJsonArray{ inTag };
                            newRulesAppend.push_back(newRule);
                            hasRuleProcessed = true;
                        }
                        else
                        {
                            auto val = copyRules.takeAt(i);
                            if (!(val.isNull() || val.isUndefined()))
                            {
                                newRules.push_back(val);
                            }
                        }
                    }
                    rules = copyRules;
                    if (!hasRuleProcessed)
                    {
                        QJsonObject integrationRule;
                        integrationRule["outboundTag"] = freedomTag;
                        integrationRule["inboundTag"] = QJsonArray{ inTag };
                        integrationRule["type"] = "field";
                        newRulesAppend.push_back(integrationRule);
                    }
                    //
                    pluginPort++;
                }
                kernel->SetConnectionSettings(GlobalConfig.inboundConfig.listenip, pluginInboundPort, pluginOutboundSettings);
            }
            auto routing = fullConfig["routing"].toObject();
            for (const auto &appended : newRulesAppend)
            {
                newRules.append(appended);
            }
            routing["rules"] = newRules;
            fullConfig["routing"] = routing;
            fullConfig["outbounds"] = new_outbounds;
        }
        //
        auto inboundPorts = GetInboundPorts(fullConfig);
        PluginHost->Send_ConnectivityEvent(QvConnectivityEventObject{ GetDisplayName(id), inboundPorts, QvConnecticity_Connecting });
        //
        currentConnectionId = id;
        lastConnectionId = id;
        bool success = true;
        for (auto &kernel : pluginKernels.keys())
        {
            bool status = pluginKernels[kernel]->StartKernel();
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
            //
            for (auto &kernel : pluginKernels.keys())
            {
                LOG(MODULE_CONNECTION, "Stopping plugin kernel: " + kernel)
                pluginKernels[kernel]->StopKernel();
            }
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
