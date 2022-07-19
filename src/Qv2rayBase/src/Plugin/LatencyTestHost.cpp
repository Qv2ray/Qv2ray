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

#include "Qv2rayBase/Plugin/LatencyTestHost.hpp"

#include "Qv2rayBase/private/Plugin/LatencyTestHost_p.hpp"
#include "Qv2rayBase/private/Plugin/LatencyTestThread_p.hpp"

namespace Qv2rayBase::Plugin
{
    LatencyTestHost::LatencyTestHost(QObject *parent) : QObject(parent)
    {
        d_ptr.reset(new LatencyTestHostPrivate);
        Q_D(LatencyTestHost);
        d->latencyThread = new LatencyTestThread(this);
        d->latencyThread->start();
    }

    LatencyTestHost::~LatencyTestHost()
    {
        Q_D(LatencyTestHost);
        d->latencyThread->stopLatencyTest();
        d->latencyThread->wait();
    }

    void LatencyTestHost::onLatencyTestCompleted_p(const ConnectionId &id, const Qv2rayPlugin::Latency::LatencyTestResponse &data)
    {
        emit OnLatencyTestCompleted(id, data);
    }

    void LatencyTestHost::TestLatency(const ConnectionId &id, const LatencyTestEngineId &engineId)
    {
        Q_D(LatencyTestHost);
        d->latencyThread->pushRequest(id, engineId);
    }

    void LatencyTestHost::StopAllLatencyTest()
    {
        Q_D(LatencyTestHost);
        d->latencyThread->stopLatencyTest();
        d->latencyThread->wait();
        d->latencyThread->start();
    }

} // namespace Qv2rayBase::Plugin
