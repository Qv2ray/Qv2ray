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

struct HTTPSOCKSUserObject : public QObject
{
    Q_GADGET
    QJS_PROP(QString, user)
    QJS_PROP(QString, pass)
    QJS_PROP(int, level)
    QJS_FUNCTION(HTTPSOCKSUserObject, F(user, pass, level))
};
//
// Socks, OutBound
struct SocksServerObject : public QObject
{
    Q_GADGET
    QJS_PROP_D(QString, address, "0.0.0.0")
    QJS_PROP_D(int, port, 0)
    QJS_PROP(QList<HTTPSOCKSUserObject>, users)
    QJS_FUNCTION(SocksServerObject, F(address, port, users))
};

//
// Http, OutBound
struct HttpServerObject : public QObject
{
    Q_GADGET
    QJS_PROP_D(QString, address, "0.0.0.0")
    QJS_PROP_D(int, port, 0)
    QJS_PROP(QList<HTTPSOCKSUserObject>, users)
    QJS_FUNCTION(HttpServerObject, F(address, port, users))
};

//
// ShadowSocks Server
struct ShadowSocksServerObject : public QObject
{
    Q_GADGET
    QJS_PROP_D(QString, address, "0.0.0.0")
    QJS_PROP_D(int, port, 0)
    QJS_PROP_D(QString, method, "aes-256-gcm")
    QJS_PROP(QString, password)
    QJS_FUNCTION(ShadowSocksServerObject, F(address, method, password, port))
};

struct VLESSUserObject : public QObject
{
    Q_GADGET
    QJS_PROP_D(QString, encryption, "none")
    QJS_PROP(QString, id)
    QJS_PROP(QString, flow)
    QJS_FUNCTION(VLESSUserObject, F(encryption, id, flow))
};

//
// VLESS Server
struct VLESSServerObject : public QObject
{
    Q_GADGET
    QJS_PROP_D(QString, address, "0.0.0.0")
    QJS_PROP_D(int, port, 0)
    QJS_PROP(QList<VLESSUserObject>, users)
    QJS_FUNCTION(VLESSServerObject, F(address, port, users))
};

constexpr auto VMESS_USER_ALTERID_DEFAULT = 0;
struct VMessUserObject : public QObject
{
    Q_GADGET
    QJS_PROP_D(int, alterId, VMESS_USER_ALTERID_DEFAULT)
    QJS_PROP_D(QString, security, "auto")
    QJS_PROP_D(int, level, 0)
    QJS_PROP(QString, id, REQUIRED)
    QJS_FUNCTION(VMessUserObject, F(id, alterId, security, level))
};

//
// VMess Server
struct VMessServerObject : public QObject
{
    Q_GADGET
    QJS_PROP_D(QString, address, "0.0.0.0", REQUIRED)
    QJS_PROP_D(int, port, 0, REQUIRED)
    QJS_PROP(QList<VMessUserObject>, users)
    QJS_FUNCTION(VMessServerObject, F(address, port, users))
};
