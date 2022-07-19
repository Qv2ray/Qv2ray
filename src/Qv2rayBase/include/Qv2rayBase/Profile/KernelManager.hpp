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
#include "Qv2rayBase/Common/ProfileHelpers.hpp"
#include "Qv2rayBase/Plugin/PluginAPIHost.hpp"
#include "Qv2rayBase/Qv2rayBaseLibrary.hpp"

namespace Qv2rayBase::Profile
{
    class KernelManagerPrivate;
    class QV2RAYBASE_EXPORT KernelManager : public QObject
    {
        Q_OBJECT
      public:
        explicit KernelManager(QObject *parent = nullptr);
        ~KernelManager();

        std::optional<QString> StartConnection(const ProfileId &id, const ProfileContent &root);
        void StopConnection();
        const ProfileId CurrentConnection() const;
        size_t ActiveKernelCount() const;
        const QMap<QString, IOBoundData> GetCurrentConnectionInboundInfo() const;

      signals:
        void OnConnected(const ProfileId &id);
        void OnDisconnected(const ProfileId &id);
        void OnCrashed(const ProfileId &id, const QString &errMessage);
        void OnKernelLogAvailable(const ProfileId &id, const QString &log);
        void OnStatsDataAvailable(const ProfileId &id, StatisticsObject);

      private slots:
        void OnKernelStatsDataRcvd_p(const StatisticsObject &);
        void OnKernelCrashed_p(const QString &msg);
        void OnKernelLog_p(const QString &log);

      private:
        QScopedPointer<KernelManagerPrivate> d_ptr;
        Q_DECLARE_PRIVATE(KernelManager)
    };
} // namespace Qv2rayBase::Profile
