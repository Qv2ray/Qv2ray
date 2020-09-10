#pragma once
#include "3rdparty/QJsonStruct/QJsonStruct.hpp"

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
