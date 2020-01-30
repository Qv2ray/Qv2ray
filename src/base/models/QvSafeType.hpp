#pragma once

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

#define SAFE_TYPEDEF(Base, name) \
    class name : public Base { \
        public: \
            template <class... Args> \
            explicit name (Args... args) : Base(args...) {} \
            const Base& raw() const { return *this; } \
    };

namespace Qv2ray
{
    // To prevent anonying QJsonObject misuse
    SAFE_TYPEDEF(QJsonObject, INBOUNDSETTING)
    SAFE_TYPEDEF(QJsonObject, OUTBOUNDSETTING)
    SAFE_TYPEDEF(QJsonObject, INBOUND)
    SAFE_TYPEDEF(QJsonObject, OUTBOUND)
    SAFE_TYPEDEF(QJsonObject, CONFIGROOT)
    SAFE_TYPEDEF(QJsonObject, PROXYSETTING)
    //
    SAFE_TYPEDEF(QJsonArray, INOUTLIST)
    SAFE_TYPEDEF(INOUTLIST, OUTBOUNDS)
    SAFE_TYPEDEF(INOUTLIST, INBOUNDS)
    SAFE_TYPEDEF(QJsonObject, ROUTING)
    SAFE_TYPEDEF(QJsonObject, ROUTERULE)
    SAFE_TYPEDEF(QJsonArray, ROUTERULELIST)
}
