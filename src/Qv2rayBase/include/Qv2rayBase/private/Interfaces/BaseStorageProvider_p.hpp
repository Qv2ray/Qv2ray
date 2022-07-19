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

// ************************ WARNING ************************
//
// This file is NOT part of the Qv2rayBase API.
// It may change at any time without notice, or even be removed.
// USE IT AT YOUR OWN RISK
//
// ************************ WARNING ************************

#include "Qv2rayBase/Interfaces/IStorageProvider.hpp"

namespace Qv2rayBase::Interfaces
{
    class Qv2rayBasePrivateStorageProvider : public IStorageProvider
    {
      public:
        Qv2rayBasePrivateStorageProvider();
        virtual ~Qv2rayBasePrivateStorageProvider() = default;

        virtual QString StorageLocation() const override;
        virtual bool LookupConfigurations(const StorageContext &) override;
        virtual void EnsureSaved() override;

        virtual QJsonObject GetBaseConfiguration() override;
        virtual bool StoreBaseConfiguration(const QJsonObject &) override;

        virtual QHash<ConnectionId, ConnectionObject> GetConnections() override;
        virtual void StoreConnections(const QHash<ConnectionId, ConnectionObject> &) override;

        virtual QHash<GroupId, GroupObject> GetGroups() override;
        virtual void StoreGroups(const QHash<GroupId, GroupObject> &) override;

        virtual QHash<RoutingId, RoutingObject> GetRoutings() override;
        virtual void StoreRoutings(const QHash<RoutingId, RoutingObject> &) override;

        virtual ProfileContent GetConnectionContent(const ConnectionId &) override;
        virtual bool StoreConnection(const ConnectionId &, const ProfileContent &) override;
        virtual bool DeleteConnection(const ConnectionId &id) override;

        virtual QDir GetUserPluginDirectory() override;
        virtual QDir GetPluginWorkingDirectory(const PluginId &pid) override;

        virtual QJsonObject GetPluginSettings(const PluginId &pid) override;
        virtual void SetPluginSettings(const PluginId &pid, const QJsonObject &obj) override;

        virtual QStringList GetAssetsPath(const QString &) override;

        virtual QJsonObject GetExtraSettings(const QString &) override;
        virtual bool StoreExtraSettings(const QString &, const QJsonObject &) override;

      private:
        QString ConfigFilePath;

        ///
        /// \brief ConfigDirPath, with trailing slash
        ///
        QString ConfigDirPath;
        StorageContext RuntimeContext;
        QString ExecutableDirPath;
    };
} // namespace Qv2rayBase::Interfaces
