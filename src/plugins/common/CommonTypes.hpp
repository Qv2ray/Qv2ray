#pragma once
#include "3rdparty/QJsonStruct/QJsonStruct.hpp"

// GUI TOOLS
#define RED(obj)                                                                                                                                \
    {                                                                                                                                           \
        auto _temp = obj->palette();                                                                                                            \
        _temp.setColor(QPalette::Text, Qt::red);                                                                                                \
        obj->setPalette(_temp);                                                                                                                 \
    }

#define BLACK(obj) obj->setPalette(QWidget::palette());

//
// Socks, OutBound
struct SocksServerObject
{
    struct UserObject
    {
        QString user;
        QString pass;
        int level;
        UserObject() : user(), pass(), level(0){};
        JSONSTRUCT_REGISTER(UserObject, F(user, pass, level))
    };
    QString address;
    int port;
    QList<UserObject> users;
    SocksServerObject() : address("0.0.0.0"), port(0), users(){};
    JSONSTRUCT_REGISTER(SocksServerObject, F(address, port, users))
};

//
// Http, OutBound
struct HttpServerObject
{
    struct UserObject
    {
        QString user;
        QString pass;
        int level;
        UserObject() : user(), pass(), level(0){};
        JSONSTRUCT_REGISTER(UserObject, F(user, pass, level))
    };
    QString address;
    int port;
    QList<UserObject> users;
    HttpServerObject() : address("0.0.0.0"), port(0), users(){};
    JSONSTRUCT_REGISTER(HttpServerObject, F(address, port, users))
};

//
// ShadowSocks Server
struct ShadowSocksServerObject
{
    QString email;
    QString address;
    QString method;
    QString password;
    bool ota;
    int level;
    int port;
    ShadowSocksServerObject() : email(""), address("0.0.0.0"), method("aes-256-cfb"), password(""), ota(false), level(0), port(0){};
    JSONSTRUCT_REGISTER(ShadowSocksServerObject, F(email, address, port, method, password, ota, level))
};

//
// VLESS Server
struct VLESSServerObject
{
    struct UserObject
    {
        QString id;
        QString encryption;
        QString flow;
        UserObject() : id(), encryption("none"), flow(){};
        JSONSTRUCT_REGISTER(UserObject, F(id, encryption, flow))
    };

    QString address;
    int port;
    QList<UserObject> users;
    VLESSServerObject() : address(""), port(0), users(){};
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
        int alterId;
        QString security;
        int level;
        QString testsEnabled;
        UserObject() : id(), alterId(VMESS_USER_ALTERID_DEFAULT), security("auto"), level(0), testsEnabled("none"){};
        JSONSTRUCT_REGISTER(UserObject, F(id, alterId, security, level, testsEnabled))
    };

    QString address;
    int port;
    QList<UserObject> users;
    VMessServerObject() : address(""), port(0), users(){};
    JSONSTRUCT_REGISTER(VMessServerObject, F(address, port, users))
};
