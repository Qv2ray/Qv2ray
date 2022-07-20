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

#pragma once
#include "QvPlugin/Handlers/LatencyTestHandler.hpp"

#include <QThread>

namespace uvw
{
    class TimerHandle;
}

namespace Qv2rayBase::Plugin
{
    class LatencyTestHost;
    class LatencyTestThread : public QThread
    {
        Q_OBJECT
      public:
        explicit LatencyTestThread(QObject *parent = nullptr);
        void stopLatencyTest();
        void pushRequest(const ConnectionId &id, const LatencyTestEngineId &engine);

      protected:
        void run() override;

      private:
#ifndef QV2RAYBASE_NO_LIBUV
        void doTest(Qv2rayBase::Plugin::LatencyTestHost *parent, uvw::TimerHandle &handle);
#else
        void doTest(Qv2rayBase::Plugin::LatencyTestHost *parent);
#endif

      private:
        std::shared_ptr<uvw::Loop> loop;
        bool isStop = false;
        std::shared_ptr<uvw::TimerHandle> stopTimer;
        std::vector<Qv2rayPlugin::Latency::LatencyTestRequest> requests;
        std::mutex m;
    };
} // namespace Qv2rayBase::Plugin
