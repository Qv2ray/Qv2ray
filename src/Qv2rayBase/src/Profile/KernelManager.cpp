//  Qv2rayBase, the modular feature-rich infrastructure library for Qv2ray.
//  Copyright (C) 2021 Moody and relavent Qv2ray contributors.
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "Qv2rayBase/Profile/KernelManager.hpp"

#include "Qv2rayBase/Common/Settings.hpp"
#include "Qv2rayBase/Common/Utils.hpp"
#include "Qv2rayBase/private/Profile/KernelManager_p.hpp"
#include "QvPlugin/Handlers/KernelHandler.hpp"

namespace Qv2rayBase::Profile
{
    using namespace Qv2rayPlugin::Kernel;
    using namespace Qv2rayPlugin::Outbound;
    using namespace Qv2rayPlugin::Event;

    KernelManager::KernelManager(QObject *parent) : QObject(parent)
    {
        d_ptr.reset(new KernelManagerPrivate);
    }

    size_t KernelManager::ActiveKernelCount() const
    {
        Q_D(const KernelManager);
        return d->kernels.size();
    }

    const QMap<QString, IOBoundData> KernelManager::GetCurrentConnectionInboundInfo() const
    {
        Q_D(const KernelManager);
        return d->inboundInfo;
    }

    const ProfileId KernelManager::CurrentConnection() const
    {
        Q_D(const KernelManager);
        return d->current;
    }

    KernelManager::~KernelManager()
    {
        StopConnection();
    }

    std::optional<QString> KernelManager::StartConnection(const ProfileId &id, const ProfileContent &_root)
    {
        Q_D(KernelManager);
        StopConnection();
        Q_ASSERT_X(d->kernels.empty(), Q_FUNC_INFO, "Kernel list isn't empty.");

        auto fullProfile = _root;
        //
        // Ensure every inbound, rule and outbound has a name.
        for (auto &in : fullProfile.inbounds)
            if (in.name.isEmpty())
                in.name = GenerateRandomString();
        for (auto &out : fullProfile.outbounds)
            if (out.name.isEmpty())
                out.name = GenerateRandomString();
        for (auto &rule : fullProfile.routing.rules)
            if (rule.name.isEmpty())
                rule.name = GenerateRandomString();

        // In case of the configuration did not specify a kernel explicitly
        // find a kernel with router, and with as many protocols supported as possible.
        auto defaultKid = fullProfile.defaultKernel.isNull() ? Qv2rayBaseLibrary::PluginAPIHost()->Kernel_GetDefaultKernel() : fullProfile.defaultKernel;
        const auto defaultKernelInfo = Qv2rayBaseLibrary::PluginAPIHost()->Kernel_GetInfo(defaultKid);

        // Leave, nothing can be found.
        if (defaultKernelInfo.Name.isEmpty())
            return QObject::tr("Cannot find the specified kernel: ").append(fullProfile.defaultKernel.toString());

        QSet<QString> protocols;
        protocols.reserve(fullProfile.outbounds.size());
        for (const auto &out : fullProfile.outbounds)
        {
            qInfo() << "Found protocol:" << out.outboundSettings.protocol;
            protocols << out.outboundSettings.protocol;
        }

        // Remove protocols which are already supported by the main kernel
        protocols -= defaultKernelInfo.SupportedProtocols;

        // Process outbounds.
        QList<OutboundObject> processedOutbounds;
        auto pluginPort = Qv2rayBaseLibrary::GetConfig()->plugin_config.plugin_port_allocation;
        for (const auto &_out : fullProfile.outbounds)
        {
            auto outbound = _out;
            if (defaultKernelInfo.SupportedProtocols.contains(outbound.outboundSettings.protocol))
            {
                // Use the default kernel
                processedOutbounds << outbound;
                continue;
            }

            const auto kid = Qv2rayBaseLibrary::PluginAPIHost()->Kernel_QueryProtocol(QSet{ outbound.outboundSettings.protocol });
            if (kid.isNull())
            {
                // Expected a plugin, but found nothing
                qInfo() << "Outbound protocol" << outbound.outboundSettings.protocol << "is not a registered plugin outbound.";
                return tr("Cannot find a kernel for outbound protocol: ") + outbound.outboundSettings.protocol;
            }

            const auto kinfo = Qv2rayBaseLibrary::PluginAPIHost()->Kernel_GetInfo(kid);
            auto pkernel = kinfo.Create();

            {
                QMap<KernelOptionFlags, QVariant> kernelOption;
                kernelOption.insert(KERNEL_SOCKS_ENABLED, true);
                kernelOption.insert(KERNEL_SOCKS_PORT, pluginPort);
                kernelOption.insert(KERNEL_LISTEN_ADDRESS, "127.0.0.1");
                qInfo() << "Sending connection settings to kernel.";
                pkernel->SetConnectionSettings(kernelOption, outbound.outboundSettings);
            }

            d->kernels.push_back({ outbound.outboundSettings.protocol, std::move(pkernel) });

            IOConnectionSettings pluginOutSettings;
            pluginOutSettings.protocolSettings = IOProtocolSettings{ QJsonObject{ { "address", "127.0.0.1" }, { "port", pluginPort } } };
            outbound.outboundSettings.protocol = u"socks"_qs;
            outbound.outboundSettings = pluginOutSettings;

            // Add the integration outbound to the list.
            processedOutbounds.append(outbound);
            pluginPort++;
        }

        qInfo() << "Applying new outbound settings.";
        fullProfile.outbounds = processedOutbounds;

        bool hasAllKernelPrepared = true;
        for (auto &[protocol, kernel] : d->kernels)
        {
            qInfo() << "Preparing kernel for starting:" << protocol;
            hasAllKernelPrepared &= kernel->PrepareConfigurations();
            if (!hasAllKernelPrepared)
            {
                qInfo() << "Plugin Kernel:" << protocol << "failed to initialize.";
                break;
            }
        }

        // Start the default kernel
        if (hasAllKernelPrepared)
        {
            auto defaultKernel = defaultKernelInfo.Create();
            defaultKernel->SetProfileContent(fullProfile);
            hasAllKernelPrepared &= defaultKernel->PrepareConfigurations();
            d->kernels.push_back({ d->QV2RAYBASE_DEFAULT_KERNEL_PLACEHOLDER, std::move(defaultKernel) });
            d->current = id;
        }

        if (!hasAllKernelPrepared)
        {
            StopConnection();
            return tr("Cannot start at least one kernel. Please check the profile and the error log.");
        }

        for (auto &k : d->kernels)
        {
            qInfo() << "Starting kernel:" << k.first;

            // We need to use old style runtime connection.
            connect(k.second.get(), SIGNAL(OnCrashed(QString)), this, SLOT(OnKernelCrashed_p(QString)), Qt::QueuedConnection);
            connect(k.second.get(), SIGNAL(OnLog(QString)), this, SLOT(OnKernelLog_p(QString)), Qt::QueuedConnection);
            connect(k.second.get(), SIGNAL(OnStatsAvailable(StatisticsObject)), this, SLOT(OnKernelStatsDataRcvd_p(StatisticsObject)), Qt::QueuedConnection);

            k.second->Start();
        }

        d->inboundInfo = GetInboundInfo(fullProfile);
        d->outboundInfo = GetOutboundInfo(fullProfile);

        emit OnConnected(id);
        Qv2rayBaseLibrary::PluginAPIHost()->Event_Send<Connectivity>({ Connectivity::Connected, id, d->inboundInfo, d->outboundInfo });
        return std::nullopt;
    }

    void KernelManager::OnKernelCrashed_p(const QString &msg)
    {
        Q_D(KernelManager);
        StopConnection();
        emit OnCrashed(d->current, msg);
    }

    void KernelManager::OnKernelLog_p(const QString &log)
    {
        Q_D(KernelManager);
        if (d->logPadding <= 0)
            for (const auto &[_, kernel] : d->kernels)
                d->logPadding = std::max(d->logPadding, Qv2rayBaseLibrary::PluginAPIHost()->Kernel_GetInfo(kernel->GetKernelId()).Name.length());

        const auto kernel = static_cast<PluginKernel *>(sender());
        const auto name = kernel ? Qv2rayBaseLibrary::PluginAPIHost()->Kernel_GetInfo(kernel->GetKernelId()).Name : u"UNKNOWN"_qs;

        for (const auto &line : SplitLines(log))
            emit OnKernelLogAvailable(d->current, u"[%1] "_qs.arg(name, d->logPadding) + line.trimmed());
    }

    void KernelManager::StopConnection()
    {
        Q_D(KernelManager);
        Q_ASSERT(d->kernels.empty() == d->current.isNull());

        if (d->kernels.empty())
        {
            qInfo() << "Cannot disconnect when there's nothing connected.";
            return;
        }

        Qv2rayBaseLibrary::PluginAPIHost()->Event_Send<Connectivity>({ Connectivity::Disconnecting, d->current });

        for (const auto &[kernel, kernelObject] : d->kernels)
        {
            qInfo() << "Stopping plugin kernel:" << kernel;
            kernelObject->Stop();
        }

        d->logPadding = 0;
        Qv2rayBaseLibrary::PluginAPIHost()->Event_Send<Connectivity>({ Connectivity::Disconnected, d->current });
        emit OnDisconnected(d->current);

        d->current.clear();
        d->kernels.clear();
    }

    void KernelManager::OnKernelStatsDataRcvd_p(const StatisticsObject &s)
    {
        Q_D(KernelManager);
        emit OnStatsDataAvailable(d->current, s);
    }
} // namespace Qv2rayBase::Profile
