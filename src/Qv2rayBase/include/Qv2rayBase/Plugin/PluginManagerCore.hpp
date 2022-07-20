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

#pragma once

#include "Qv2rayBase/Qv2rayBaseFeatures.hpp"
#include "QvPlugin/PluginInterface.hpp"

#include <QObject>

class QPluginLoader;

namespace Qv2rayBase::Plugin
{
    struct PluginInfo
    {
        QString libraryPath;
        QPluginLoader *loader;
        Qv2rayPlugin::Qv2rayInterfaceImpl *pinterface = nullptr;
        Q_ALWAYS_INLINE Qv2rayPlugin::QvPluginMetadata metadata() const
        {
            Q_ASSERT(pinterface);
            return pinterface->GetMetadata();
        }
        Q_ALWAYS_INLINE PluginId id() const
        {
            return metadata().InternalID;
        }
        Q_ALWAYS_INLINE bool hasComponent(Qv2rayPlugin::PLUGIN_COMPONENT_TYPE t) const
        {
            return metadata().Components.contains(t);
        }
    };

    class PluginManagerCorePrivate;
    class QV2RAYBASE_EXPORT PluginManagerCore : public QObject
    {
        Q_OBJECT
      public:
        PluginManagerCore(QObject *parent = nullptr);
        virtual ~PluginManagerCore();

        void LoadPlugins();
        void SetPluginSettings(const PluginId &pid, const QJsonObject &settings);

        QList<const PluginInfo *> GetPlugins(Qv2rayPlugin::PLUGIN_COMPONENT_TYPE c) const;
        const QList<const PluginInfo *> AllPlugins() const;
        const PluginInfo *GetPlugin(const PluginId &pid);

        bool GetPluginEnabled(const PluginId &pid) const;
        void SetPluginEnabled(const PluginId &pid, bool isEnabled) const;
        void SavePluginSettings() const;

      private:
        bool tryLoadPlugin(const QString &pluginFullPath);
        bool loadPluginImpl(const QString &fullPath, QObject *instance, QPluginLoader *loader);

      private slots:
        void PluginLog(QString log);
        void PluginMessageBox(QString title, QString message);

      private:
        QScopedPointer<PluginManagerCorePrivate> d_ptr;
        Q_DECLARE_PRIVATE(PluginManagerCore)
    };
} // namespace Qv2rayBase::Plugin
