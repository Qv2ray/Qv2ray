#pragma once
#include <QString>
#include <QtCore>
#include "3rdparty/x2struct/x2struct.hpp"
namespace Qv2ray::base
{
    using namespace std::chrono;
    // Common struct for Groups and Subscriptions
    struct GroupObjectBase {
        QString displayName;
        QList<QString> connections;
        int64_t importDate;
        GroupObjectBase(): displayName(), connections(), importDate() { }
    };

    struct GroupObject_Config : GroupObjectBase {
        GroupObject_Config() { }
        XTOSTRUCT(O(displayName, connections, importDate))
    };

    struct SubscriptionObject_Config : GroupObjectBase {
        //
        QString address;
        int64_t lastUpdated;
        float updateInterval;
        SubscriptionObject_Config(): address(""), lastUpdated(system_clock::to_time_t(system_clock::now())), updateInterval(10) { }
        XTOSTRUCT(O(lastUpdated, updateInterval, address, connections, displayName, importDate))
    };

    struct ConnectionObject_Config {
        QString displayName;
        int64_t importDate;
        int64_t lastConnected;
        int64_t latency;
        int64_t upLinkData;
        int64_t downLinkData;
        ConnectionObject_Config(): displayName(), importDate(system_clock::to_time_t(system_clock::now())), lastConnected(), latency(0), upLinkData(0), downLinkData(0) { }
        XTOSTRUCT(O(displayName, importDate, lastConnected, latency, upLinkData, downLinkData))
    };
}

using namespace Qv2ray::base;
