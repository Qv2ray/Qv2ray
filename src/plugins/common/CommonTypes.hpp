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
    Q_GADGET
    QJS_CONSTRUCTOR(HTTPSOCKSUserObject)
    QJS_PROP(QString, user)
    QJS_PROP(QString, pass)
    QJS_PROP(int, level)
    QJS_FUNCTION(F(user, pass, level))
};
//
// Socks, OutBound
struct SocksServerObject
{
    Q_GADGET
    QJS_CONSTRUCTOR(SocksServerObject)
    QJS_PROP_D(QString, address, "0.0.0.0")
    QJS_PROP_D(int, port, 0)
    QJS_PROP(QList<HTTPSOCKSUserObject>, users)
    QJS_FUNCTION(F(address, port, users))
};

//
// Http, OutBound
struct HttpServerObject
{
    Q_GADGET
    QJS_CONSTRUCTOR(HttpServerObject)
    QJS_PROP_D(QString, address, "0.0.0.0")
    QJS_PROP_D(int, port, 0)
    QJS_PROP(QList<HTTPSOCKSUserObject>, users)
    QJS_FUNCTION(F(address, port, users))
};

//
// ShadowSocks Server
struct ShadowSocksServerObject
{
    Q_GADGET
    QJS_CONSTRUCTOR(ShadowSocksServerObject)
    QJS_PROP_D(QString, address, "0.0.0.0")
    QJS_PROP_D(int, port, 0)
    QJS_PROP_D(QString, method, "aes-256-gcm")
    QJS_PROP(QString, password)
    QJS_FUNCTION(F(address, method, password, port))
};

struct VLESSUserObject
{
    Q_GADGET
    QJS_CONSTRUCTOR(VLESSUserObject)
    QJS_PROP_D(QString, encryption, "none");
    QJS_PROP(QString, id)
    QJS_PROP(QString, flow);
    QJS_FUNCTION(F(encryption, id, flow))
};

//
// VLESS Server
struct VLESSServerObject
{
    Q_GADGET
    QJS_CONSTRUCTOR(VLESSServerObject)
    QJS_PROP_D(QString, address, "0.0.0.0")
    QJS_PROP_D(int, port, 0)
    QJS_PROP(QList<VLESSUserObject>, users)
    QJS_FUNCTION(F(address, port, users))
};

constexpr auto VMESS_USER_ALTERID_DEFAULT = 0;
struct VMessUserObject
{
    Q_GADGET
    QJS_CONSTRUCTOR(VMessUserObject)
    QJS_PROP_D(int, alterId, VMESS_USER_ALTERID_DEFAULT)
    QJS_PROP_D(QString, security, "auto")
    QJS_PROP_D(int, level, 0)
    QJS_PROP(QString, id, REQUIRED)
    QJS_FUNCTION(F(id, alterId, security, level))
};

//
// VMess Server
struct VMessServerObject
{
    Q_GADGET
    QJS_CONSTRUCTOR(VMessServerObject)
    QJS_PROP_D(QString, address, "0.0.0.0", REQUIRED)
    QJS_PROP_D(int, port, 0, REQUIRED)
    QJS_PROP(QList<VMessUserObject>, users)
    QJS_FUNCTION(F(address, port, users))
};
