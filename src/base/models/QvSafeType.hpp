#pragma once
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

#define SAFE_TYPEDEF_EXTRA(Base, name, extra) \
    class name : public Base { \
        public: \
            template <class... Args> \
            explicit name (Args... args) : Base(args...) {} \
            const Base& raw() const { return *this; } \
            extra };

#define nothing
#define SAFE_TYPEDEF(Base, name) SAFE_TYPEDEF_EXTRA(Base, name, nothing)

using namespace std;
namespace Qv2ray::base::safetype
{
    // To prevent anonying QJsonObject misuse
    SAFE_TYPEDEF(QJsonObject, INBOUNDSETTING)
    SAFE_TYPEDEF(QJsonObject, OUTBOUNDSETTING)
    SAFE_TYPEDEF(QJsonObject, INBOUND)
    SAFE_TYPEDEF(QJsonObject, OUTBOUND)
    SAFE_TYPEDEF(QJsonObject, CONFIGROOT)
    SAFE_TYPEDEF(QJsonObject, PROXYSETTING)
    //
    SAFE_TYPEDEF(QJsonArray, ROUTERULELIST)
    SAFE_TYPEDEF(QJsonArray, INOUTLIST)
    SAFE_TYPEDEF(QJsonObject, ROUTING)
    SAFE_TYPEDEF(QJsonObject, ROUTERULE)
    SAFE_TYPEDEF(INOUTLIST, OUTBOUNDS)
    SAFE_TYPEDEF(INOUTLIST, INBOUNDS)
}

