#pragma once
#include "QJsonStruct.hpp"

// GUI TOOLS
#define RED(obj)                                                                                                                                     \
    {                                                                                                                                                \
        auto _temp = obj->palette();                                                                                                                 \
        _temp.setColor(QPalette::Text, Qt::red);                                                                                                     \
        obj->setPalette(_temp);                                                                                                                      \
    }

#define BLACK(obj) obj->setPalette(QWidget::palette());

struct HTTPSOCKSUserObject
{
    QString user;
    QString pass;
    int level = 0;
    JSONSTRUCT_COMPARE(HTTPSOCKSUserObject, user, pass, level)
    JSONSTRUCT_REGISTER(HTTPSOCKSUserObject, F(user, pass, level))
};
//
// Socks, OutBound
struct SocksServerObject
{
    QString address = "0.0.0.0";
    int port = 0;
    QList<HTTPSOCKSUserObject> users;
    JSONSTRUCT_COMPARE(SocksServerObject, address, port, users)
    JSONSTRUCT_REGISTER(SocksServerObject, F(address, port, users))
};

//
// Http, OutBound
struct HttpServerObject
{
    QString address = "0.0.0.0";
    int port = 0;
    QList<HTTPSOCKSUserObject> users;
    JSONSTRUCT_COMPARE(HttpServerObject, address, port, users)
    JSONSTRUCT_REGISTER(HttpServerObject, F(address, port, users))
};

//
// ShadowSocks Server
struct ShadowSocksServerObject
{
    QString address = "0.0.0.0";
    QString method = "aes-256-gcm";
    QString password;
    int port = 0;
    JSONSTRUCT_COMPARE(ShadowSocksServerObject, address, method, password)
    JSONSTRUCT_REGISTER(ShadowSocksServerObject, A(method), F(address, port, password))
};

//
// VLESS Server
struct VLESSServerObject
{
    struct UserObject
    {
        QString id;
        QString encryption = "none";
        QString flow;
        JSONSTRUCT_COMPARE(UserObject, id, encryption, flow)
        JSONSTRUCT_REGISTER(UserObject, A(encryption), F(id, flow))
    };

    QString address;
    int port = 0;
    QList<UserObject> users;
    JSONSTRUCT_COMPARE(VLESSServerObject, address, port, users)
    JSONSTRUCT_REGISTER(VLESSServerObject, F(address, port, users))
};

//
// VMess Server
constexpr auto VMESS_USER_ALTERID_DEFAULT = 0;
struct VMessServerObject
{
    struct UserObject
    {
        QString id;
        int alterId = VMESS_USER_ALTERID_DEFAULT;
        QString security = "auto";
        int level = 0;
        JSONSTRUCT_COMPARE(UserObject, id, alterId, security, level)
        JSONSTRUCT_REGISTER(UserObject, F(id, alterId, security, level))
    };

    QString address;
    int port = 0;
    QList<UserObject> users;
    JSONSTRUCT_COMPARE(VMessServerObject, address, port, users)
    JSONSTRUCT_REGISTER(VMessServerObject, F(address, port, users))
};
