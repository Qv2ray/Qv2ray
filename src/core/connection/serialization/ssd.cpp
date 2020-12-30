/**
 * A Naive SSD Decoder for Qv2ray
 *
 * @author DuckSoft <realducksoft@gmail.com>
 * @copyright Licensed under GPLv3.
 */

#include "base/Qv2rayBase.hpp"
#include "core/connection/Generation.hpp"
#include "core/connection/Serialization.hpp"
#include "utils/QvHelpers.hpp"

#define QV_MODULE_NAME "SSDConverter"

namespace Qv2ray::core::connection::serialization
{

    namespace ssd
    {
        // These below are super strict checking schemes, but necessary.
#define MUST_EXIST(fieldName)                                                                                                                        \
    if (!obj.contains((fieldName)) || obj[(fieldName)].isUndefined() || obj[(fieldName)].isNull())                                                   \
    {                                                                                                                                                \
        *logList << QObject::tr("Invalid ssd link: json: field %1 must exist").arg(fieldName);                                                       \
        return {};                                                                                                                                   \
    }
#define MUST_PORT(fieldName)                                                                                                                         \
    MUST_EXIST(fieldName);                                                                                                                           \
    if (int value = obj[(fieldName)].toInt(-1); value < 0 || value > 65535)                                                                          \
    {                                                                                                                                                \
        *logList << QObject::tr("Invalid ssd link: json: field %1 must be valid port number");                                                       \
        return {};                                                                                                                                   \
    }
#define MUST_STRING(fieldName)                                                                                                                       \
    MUST_EXIST(fieldName);                                                                                                                           \
    if (!obj[(fieldName)].isString())                                                                                                                \
    {                                                                                                                                                \
        *logList << QObject::tr("Invalid ssd link: json: field %1 must be of type 'string'").arg(fieldName);                                         \
        return {};                                                                                                                                   \
    }
#define MUST_ARRAY(fieldName)                                                                                                                        \
    MUST_EXIST(fieldName);                                                                                                                           \
    if (!obj[(fieldName)].isArray())                                                                                                                 \
    {                                                                                                                                                \
        *logList << QObject::tr("Invalid ssd link: json: field %1 must be an array").arg(fieldName);                                                 \
        return {};                                                                                                                                   \
    }

#define SERVER_SHOULD_BE_OBJECT(server)                                                                                                              \
    if (!server.isObject())                                                                                                                          \
    {                                                                                                                                                \
        *logList << QObject::tr("Skipping invalid ssd server: server must be an object");                                                            \
        continue;                                                                                                                                    \
    }
#define SHOULD_EXIST(fieldName)                                                                                                                      \
    if (serverObject[(fieldName)].isUndefined())                                                                                                     \
    {                                                                                                                                                \
        *logList << QObject::tr("Skipping invalid ssd server: missing required field %1").arg(fieldName);                                            \
        continue;                                                                                                                                    \
    }
#define SHOULD_STRING(fieldName)                                                                                                                     \
    SHOULD_EXIST(fieldName);                                                                                                                         \
    if (!serverObject[(fieldName)].isString())                                                                                                       \
    {                                                                                                                                                \
        *logList << QObject::tr("Skipping invalid ssd server: field %1 should be of type 'string'").arg(fieldName);                                  \
        continue;                                                                                                                                    \
    }

        QList<std::pair<QString, CONFIGROOT>> Deserialize(const QString &uri, QString *groupName, QStringList *logList)
        {
            LOG("SSD Link format is now deprecated.");
            // ssd links should begin with "ssd://"
            if (!uri.startsWith("ssd://"))
            {
                *logList << QObject::tr("Invalid ssd link: should begin with ssd://");
                return {};
            }

            // decode base64
            const auto ssdURIBody = uri.mid(6, uri.length() - 6); //(&uri, 6, uri.length() - 6);
            const auto decodedJSON = SafeBase64Decode(ssdURIBody).toUtf8();

            if (decodedJSON.length() == 0)
            {
                *logList << QObject::tr("Invalid ssd link: base64 parse failed");
                return {};
            }

            const auto decodeError = VerifyJsonString(decodedJSON);
            if (!decodeError.isEmpty())
            {
                *logList << QObject::tr("Invalid ssd link: json parse failed: ") % decodeError;
                return {};
            }

            // casting to object
            const auto obj = JsonFromString(decodedJSON);

            // obj.airport
            MUST_STRING("airport");
            *groupName = obj["airport"].toString();

            // obj.port
            MUST_PORT("port");
            const int port = obj["port"].toInt();

            // obj.encryption
            MUST_STRING("encryption");
            const auto encryption = obj["encryption"].toString();

            // check: rc4-md5 is not supported by v2ray-core
            // TODO: more checks, including all algorithms
            if (encryption.toLower() == "rc4-md5")
            {
                *logList << QObject::tr("Invalid ssd link: rc4-md5 encryption is not supported by v2ray-core");
                return {};
            }

            // obj.password
            MUST_STRING("password");
            const auto password = obj["password"].toString();
            // obj.servers
            MUST_ARRAY("servers");
            //
            QList<std::pair<QString, CONFIGROOT>> serverList;
            //

            // iterate through the servers
            for (const auto &server : obj["servers"].toArray())
            {
                SERVER_SHOULD_BE_OBJECT(server);
                const auto serverObject = server.toObject();
                ShadowSocksServerObject ssObject;

                // encryption
                ssObject.method = encryption;

                // password
                ssObject.password = password;

                // address :-> "server"
                SHOULD_STRING("server");
                const auto serverAddress = serverObject["server"].toString();
                ssObject.address = serverAddress;

                // port selection:
                //   normal:     use global settings
                //   overriding: use current config
                if (serverObject["port"].isUndefined())
                {
                    ssObject.port = port;
                }
                else if (auto currPort = serverObject["port"].toInt(-1); (currPort >= 0 && currPort <= 65535))
                {
                    ssObject.port = currPort;
                }
                else
                {
                    ssObject.port = port;
                }

                // name decision:
                //   untitled: using server:port as name
                //   entitled: using given name
                QString nodeName;
                if (serverObject["remarks"].isUndefined())
                {
                    nodeName = QString("%1:%2").arg(ssObject.address).arg(ssObject.port);
                }
                else if (serverObject["remarks"].isString())
                {
                    nodeName = serverObject["remarks"].toString();
                }
                else
                {
                    nodeName = QString("%1:%2").arg(ssObject.address).arg(ssObject.port);
                }

                // ratio decision:
                //   unspecified: ratio = 1
                //   specified:   use given value
                double ratio = 1.0;
                if (auto currRatio = serverObject["ratio"].toDouble(-1.0); currRatio != -1.0)
                {
                    ratio = currRatio;
                }
                //  else if (!serverObject["ratio"].isUndefined())
                //  {
                //      //*logList << QObject::tr("Invalid ratio encountered. using fallback value.");
                //  }

                // format the total name of the node.
                const auto finalName = QV2RAY_SSD_DEFAULT_NAME_PATTERN.arg(*groupName, nodeName).arg(ratio);
                // appending to the total list
                CONFIGROOT root;
                OUTBOUNDS outbounds;
                outbounds.append(GenerateOutboundEntry(OUTBOUND_TAG_PROXY, "shadowsocks", GenerateShadowSocksOUT({ ssObject }), {}));
                root["outbounds"] = outbounds;
                serverList.append({ finalName, root });
            }

            // returns the current result
            return serverList;
        }
#undef MUST_EXIST
#undef MUST_PORT
#undef MUST_ARRAY
#undef MUST_STRING
#undef SERVER_SHOULD_BE_OBJECT
#undef SHOULD_EXIST
#undef SHOULD_STRING
    } // namespace ssd
} // namespace Qv2ray::core::connection::serialization
