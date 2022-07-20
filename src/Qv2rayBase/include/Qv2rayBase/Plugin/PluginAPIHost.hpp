//  Qv2rayBase, the modular feature-rich infrastructure library for Qv2ray.
//  Copyright (C) 2021 Moody and relavent Qv2ray contributors.
//  Copyright (C) 2022 Moody
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

#pragma once
#include "Qv2rayBase/Qv2rayBaseFeatures.hpp"
#include "QvPlugin/PluginInterface.hpp"

namespace Qv2rayBase::Plugin
{
    struct PluginInfo;
    class GuiPluginAPIHost;
    class PluginAPIHostPrivate;
    class QV2RAYBASE_EXPORT PluginAPIHost
    {
        friend class GuiPluginAPIHost;

      public:
        explicit PluginAPIHost();
        ~PluginAPIHost();
        void InitializePlugins();

      public:
        // Latency tester API
        QList<Qv2rayPlugin::LatencyTestEngineInfo> Latency_GetAllEngines() const;
        Qv2rayPlugin::LatencyTestEngineInfo Latency_GetEngine(const LatencyTestEngineId &id) const;

        // Kernel API
        QList<Qv2rayPlugin::Kernel::KernelFactory> Kernel_GetAllKernels() const;
        Qv2rayPlugin::KernelFactory Kernel_GetInfo(const KernelId &kid) const;
        KernelId Kernel_GetDefaultKernel() const;
        KernelId Kernel_QueryProtocol(const QSet<QString> &protocols) const;

        // Event API
        template<typename E>
        void Event_Send(const typename E::EventObject &object) const
        {
            SendEventInternal(object);
        }

        // Outbound Get/Set Data
        std::optional<PluginIOBoundData> Outbound_GetData(const IOConnectionSettings &) const;
        bool Outbound_SetData(IOConnectionSettings &, const PluginIOBoundData &) const;

        // Outbound De/serialize
        std::optional<QString> Outbound_Serialize(const QString &name, const IOConnectionSettings &outbound) const;
        std::optional<std::pair<QString, IOConnectionSettings>> Outbound_Deserialize(const QString &link) const;

        // Profile Generation
        ProfileContent PreprocessProfile(const ProfileContent &) const;

        // Subscription Adapter API
        std::optional<std::shared_ptr<Qv2rayPlugin::SubscriptionProvider>> Subscription_CreateProvider(const SubscriptionProviderId &id) const;
        QList<std::pair<const PluginInfo *, Qv2rayPlugin::SubscriptionProviderInfo>> Subscription_GetProviderInfoList() const;
        std::pair<const PluginInfo *, Qv2rayPlugin::SubscriptionProviderInfo> Subscription_GetProviderInfo(const SubscriptionProviderId &id) const;

      private:
        void SendEventInternal(const Qv2rayPlugin::ConnectionStats::EventObject &) const;
        void SendEventInternal(const Qv2rayPlugin::Connectivity::EventObject &) const;
        void SendEventInternal(const Qv2rayPlugin::ConnectionEntry::EventObject &) const;

      private:
        QScopedPointer<PluginAPIHostPrivate> d_ptr;
        Q_DECLARE_PRIVATE(PluginAPIHost)
    };

    const inline QStringList GetPluginComponentsString(const QList<Qv2rayPlugin::PLUGIN_GUI_COMPONENT_TYPE> &types)
    {
        QStringList typesList;
        if (types.isEmpty())
            typesList << QObject::tr("None");
        for (const auto &type : types)
        {
            switch (type)
            {
                case Qv2rayPlugin::GUI_COMPONENT_SETTINGS: typesList << QObject::tr("Settings Widget"); break;
                case Qv2rayPlugin::GUI_COMPONENT_INBOUND_EDITOR: typesList << QObject::tr("Inbound Editor"); break;
                case Qv2rayPlugin::GUI_COMPONENT_OUTBOUND_EDITOR: typesList << QObject::tr("Outbound Editor"); break;
                case Qv2rayPlugin::GUI_COMPONENT_MAIN_WINDOW_ACTIONS: typesList << QObject::tr("MainWindow Actions"); break;
                case Qv2rayPlugin::GUI_COMPONENT_TRAY_MENUS: typesList << QObject::tr("Tray Menu"); break;
            }
        }
        return typesList;
    }

    const inline QStringList GetPluginComponentsString(const QList<Qv2rayPlugin::PLUGIN_COMPONENT_TYPE> &types)
    {
        QStringList typesList;
        if (types.isEmpty())
            typesList << QObject::tr("None");
        for (const auto &type : types)
        {
            switch (type)
            {
                case Qv2rayPlugin::COMPONENT_KERNEL: typesList << QObject::tr("Kernel"); break;
                case Qv2rayPlugin::COMPONENT_OUTBOUND_HANDLER: typesList << QObject::tr("Outbound Handler/Parser"); break;
                case Qv2rayPlugin::COMPONENT_SUBSCRIPTION_ADAPTER: typesList << QObject::tr("Subscription Adapter"); break;
                case Qv2rayPlugin::COMPONENT_EVENT_HANDLER: typesList << QObject::tr("Event Handler"); break;
                case Qv2rayPlugin::COMPONENT_GUI: typesList << QObject::tr("GUI Components"); break;
                case Qv2rayPlugin::COMPONENT_LATENCY_TEST_ENGINE: typesList << QObject::tr("Latency Test Engine"); break;
                case Qv2rayPlugin::COMPONENT_PROFILE_PREPROCESSOR: typesList << QObject::tr("Profile Preprocessor"); break;
            }
        }
        return typesList;
    }
} // namespace Qv2rayBase::Plugin
