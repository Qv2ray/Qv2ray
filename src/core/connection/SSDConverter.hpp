#pragma once

#include "base/Qv2rayBase.hpp"
#include "common/QvHelpers.hpp"

namespace Qv2ray::core::handlers
{
    /**
     * A Naive SSD Decoder for Qv2ray
     *
     * @author DuckSoft <realducksoft@gmail.com>
     * @copyright Licensed under GPLv3.
     */
    namespace ssd
    {
        /**
         * pattern for the nodes in ssd links.
         *
         * %1: airport name
         * %2: node name
         * %3: rate
         */
        inline auto DEFAULT_NAME_PATTERN = QString::tr("%1 - %2 (rate %3)");

        /**
         * @brief decodeSSD
         * @param uri     the uri of ssd link.
         * @param pattern the pattern for node names.
         * @return tuple of:
         *          - tuple of:
         *             - airport name (for the sake of grouping)
         *             - shadowsocks configuration list
         *          - log list
         *         in case of error, no objects will be returned.
         */
        std::pair <std::optional<std::pair<QString, QList<std::pair<QString, ShadowSocksServerObject>>>>, QStringList>
        decodeSSD(const QString &uri, const QString &pattern = DEFAULT_NAME_PATTERN);
    }
}

#define MUST_EXIST(fieldName) \
    if (obj[(fieldName)].isUndefined()) {\
        logList << QObject::tr("invalid ssd link: json: field %1 must exist").arg(fieldName);\
        return std::make_pair(std::nullopt, logList);\
    }
#define MUST_PORT(fieldName) MUST_EXIST(fieldName);\
    if (int value = obj[(fieldName)].toInt(-1); value < 0 || value > 65535) { \
        logList << QObject::tr("invalid ssd link: json: field %1 must be valid port number");\
        return std::make_pair(std::nullopt, logList);\
    }
#define MUST_STRING(fieldName) MUST_EXIST(fieldName);\
    if (!obj[(fieldName)].isString()) {\
        logList << QObject::tr("invalid ssd link: json: field %1 must be of type 'string'").arg(fieldName);\
        return std::make_pair(std::nullopt, logList);\
    }
#define MUST_ARRAY(fieldName) MUST_EXIST(fieldName);\
    if (!obj[(fieldName)].isArray()) {\
        logList << QObject::tr("invalid ssd link: json: field %1 must be an array").arg(fieldName);\
        return std::make_pair(std::nullopt, logList);\
    }

#define SERVER_SHOULD_BE_OBJECT(server) \
    if (!server.isObject()) {\
        logList << QObject::tr("skipping invalid ssd server: server must be an object");\
        continue;\
    }
#define SHOULD_EXIST(fieldName) \
    if (serverObject[(fieldName)].isUndefined()) { \
        logList << QObject::tr("skipping invalid ssd server: missing required field %1").arg(fieldName);\
        continue;\
    }
#define SHOULD_STRING(fieldName) SHOULD_EXIST(fieldName); \
    if (!serverObject[(fieldName)].isString()) { \
        logList << QObject::tr("skipping invalid ssd server: field %1 should be of type 'string'").arg(fieldName);\
        continue; \
    }


std::pair <std::optional<std::pair<QString, QList<std::pair<QString, ShadowSocksServerObject>>>>, QStringList>
Qv2ray::core::handlers::ssd::decodeSSD(const QString &uri, const QString &pattern)
{
    // The list for the parsing log.
    QStringList logList;

    // ssd links should begin with "ssd://"
    if (!uri.startsWith("ssd://")) {
        logList << QObject::tr("invalid ssd link: should begin with ssd://");
        return std::make_pair(std::nullopt, logList);
    }

    // decode base64
    const auto ssdURIBody = QStringRef(&uri, 5, uri.length() - 6);
    const auto decodedJSON = QByteArray::fromBase64(ssdURIBody.toUtf8());

    if (decodedJSON.length() == 0) {
        logList << QObject::tr("invalid ssd link: base64 parse failed");
        return std::make_pair(std::nullopt, logList);
    }

    // parse json
    QJsonParseError err;
    QJsonDocument document = QJsonDocument::fromJson(decodedJSON, &err);

    if (document.isNull()) {
        logList << QObject::tr("invalid ssd link: json parse failed: ") % err.errorString();
        return std::make_pair(std::nullopt, logList);
    }

    // json should be an object
    if (!document.isObject()) {
        logList << QObject::tr("invalid ssd link: found non-object json, aborting");
        return std::make_pair(std::nullopt, logList);
    }

    // casting to object
    QJsonObject obj = document.object();
    //
    // obj.airport
    MUST_STRING("airport");
    const QString airport = obj["airport"].toString();
    // obj.port
    MUST_PORT("port");
    const int port = obj["port"].toInt();
    // obj.encryption
    MUST_STRING("encryption");
    const QString encryption = obj["encryption"].toString();

    // check: rc4-md5 is not supported by v2ray-core
    // TODO: more checks, including all algorithms
    if (encryption == "rc4-md5") {
        logList << QObject::tr("invalid ssd link: rc4-md5 encryption is not supported by v2ray-core");
        return std::make_pair(std::nullopt, logList);
    }

    // obj.password
    MUST_STRING("password");
    const QString password = obj["password"].toString();
    // obj.servers
    MUST_ARRAY("servers");
    //
    QList<std::pair<QString, ShadowSocksServerObject>> serverList;
    //

    // iterate through the servers
    for (QJsonValueRef server : obj["servers"].toArray()) {
        SERVER_SHOULD_BE_OBJECT(server);
        QJsonObject serverObject = server.toObject();
        ShadowSocksServerObject ssObject;
        // encryption
        ssObject.method = encryption;
        // password
        ssObject.password = password;
        // address :-> "server"
        SHOULD_STRING("server");
        const QString serverAddress = serverObject["server"].toString();
        ssObject.address = serverAddress;

        // port selection:
        //   normal:     use global settings
        //   overriding: use current config
        if (serverObject["port"].isUndefined()) {
            ssObject.port = port;
        } else if (auto currPort = serverObject["port"].toInt(-1); port >= 0 && port <= 65535) {
            ssObject.port = currPort;
        } else {
            logList << QObject::tr("warning: invalid port encountered. using fallback value.");
            ssObject.port = port;
        }

        // name decision:
        //   untitled: using server:port as name
        //   entitled: using given name
        QString nodeName;

        if (serverObject["remarks"].isUndefined()) {
            nodeName = QString("%1:%2").arg(ssObject.address).arg(ssObject.port);
        } else if (serverObject["remarks"].isString()) {
            nodeName = serverObject["remarks"].toString();
        } else {
            logList << QObject::tr("warning: invalid name encountered. using fallback value.");
            nodeName = QString("%1:%2").arg(ssObject.address).arg(ssObject.port);
        }

        // ratio decision:
        //   unspecified: ratio = 1
        //   specified:   use given value
        double ratio = 1.0;

        if (auto currRatio = serverObject["ratio"].toDouble(-1.0); currRatio != -1.0) {
            ratio = currRatio;
        } else if (!serverObject["ratio"].isUndefined()) {
            logList << QObject::tr("warning: invalid ratio encountered. using fallback value.");
        }

        // format the total name of the node.
        const QString totalName = pattern.arg(airport, nodeName).arg(ratio);
        // appending to the total list
        serverList.append(make_pair(totalName, ssObject));
    }

    // returns the current result
    return std::make_pair(std::make_pair(airport, serverList), logList);
}

#undef MUST_EXIST
#undef MUST_PORT
#undef MUST_ARRAY
#undef MUST_STRING
#undef SERVER_SHOULD_BE_OBJECT
#undef SHOULD_EXIST
#undef SHOULD_STRING
