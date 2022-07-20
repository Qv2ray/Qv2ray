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
#include "QvPlugin/Handlers/LatencyTestHandler.hpp"

namespace Qv2rayBase::Plugin
{
    class LatencyTestHostPrivate;
    class QV2RAYBASE_EXPORT LatencyTestHost : public QObject
    {
        Q_OBJECT
      public:
        explicit LatencyTestHost(QObject *parent = nullptr);
        virtual ~LatencyTestHost();
        void TestLatency(const ConnectionId &id, const LatencyTestEngineId &engineId);
        void StopAllLatencyTest();

      signals:
        void OnLatencyTestCompleted(const ConnectionId &id, const Qv2rayPlugin::Latency::LatencyTestResponse &data);

      private slots:
        void onLatencyTestCompleted_p(const ConnectionId &id, const Qv2rayPlugin::Latency::LatencyTestResponse &data);

      private:
        QScopedPointer<LatencyTestHostPrivate> d_ptr;
        Q_DECLARE_PRIVATE(LatencyTestHost)
    };
} // namespace Qv2rayBase::Plugin
