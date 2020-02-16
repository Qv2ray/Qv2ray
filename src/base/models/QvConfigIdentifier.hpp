#pragma once
#include <QString>
#include <QtCore>
#include "3rdparty/x2struct/x2struct.hpp"
namespace Qv2ray::base
{
    using namespace std::chrono;
    // Common struct for Groups and Subscriptions
    struct _QvGroupObjectBase {
        QString displayName;
        QList<QString> connections;
        _QvGroupObjectBase(): displayName(), connections() { }
    };

    struct QvGroupObject : _QvGroupObjectBase {
        QvGroupObject() { }
        XTOSTRUCT(O(displayName, connections))
    };

    struct QvSubscriptionObject : _QvGroupObjectBase {
        QString address;
        int64_t lastUpdated;
        float updateInterval;
        QvSubscriptionObject(): address(""), lastUpdated(system_clock::to_time_t(system_clock::now())), updateInterval(10) { }
        XTOSTRUCT(O(lastUpdated, updateInterval, address, connections, displayName))
    };

    struct QvConnectionObject {
        QString displayName;
        int64_t importDate;
        int64_t lastConnected;
        int64_t latency;
        int64_t upLinkData;
        int64_t downLinkData;
        QvConnectionObject(): displayName(), importDate(system_clock::to_time_t(system_clock::now())), lastConnected(), latency(0), upLinkData(0), downLinkData(0) { }
        XTOSTRUCT(O(displayName, importDate, lastConnected, latency, upLinkData, downLinkData))
    };
}
