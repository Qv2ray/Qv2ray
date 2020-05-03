#pragma once
#include "libs/QJsonStruct/QJsonStruct.hpp"

#include <QString>
#include <QtCore>
namespace Qv2ray::base
{
    constexpr unsigned int QVTCPING_VALUE_ERROR = 99999;
    constexpr unsigned int QVTCPING_VALUE_NODATA = QVTCPING_VALUE_ERROR - 1;
    using namespace std::chrono;
    // Common struct for Groups and Subscriptions
    struct GroupObject_Config
    {
        QString displayName;
        QList<QString> connections;
        qint64 importDate;
        GroupObject_Config() : displayName(), connections(), importDate(){};
        JSONSTRUCT_REGISTER(GroupObject_Config, F(displayName, connections, importDate))
    };

    struct SubscriptionObject_Config : GroupObject_Config
    {
        //
        QString address;
        qint64 lastUpdated;
        float updateInterval;
        SubscriptionObject_Config() : address(""), lastUpdated(system_clock::to_time_t(system_clock::now())), updateInterval(10){};
        JSONSTRUCT_REGISTER(SubscriptionObject_Config, F(lastUpdated, updateInterval, address), B(GroupObject_Config))
    };

    struct ConnectionObject_Config
    {
        QString displayName;
        qint64 importDate;
        qint64 lastConnected;
        qint64 latency;
        qint64 upLinkData;
        qint64 downLinkData;
        ConnectionObject_Config()
            : displayName(), importDate(system_clock::to_time_t(system_clock::now())), lastConnected(), latency(QVTCPING_VALUE_NODATA),
              upLinkData(0), downLinkData(0){};
        JSONSTRUCT_REGISTER(ConnectionObject_Config, F(displayName, importDate, lastConnected, latency, upLinkData, downLinkData))
    };
} // namespace Qv2ray::base

using namespace Qv2ray::base;
