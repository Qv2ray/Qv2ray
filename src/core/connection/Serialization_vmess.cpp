#include "Generation.hpp"
#include "Serialization.hpp"
#include "common/QvHelpers.hpp"
#include "core/CoreUtils.hpp"
#include "core/handler/ConfigHandler.hpp"

namespace Qv2ray::core::connection
{
    namespace Serialization::vmess
    {

        // From https://github.com/2dust/v2rayN/wiki/分享链接格式说明(ver-2)
        const QString ConvertConfigToVMessString(const StreamSettingsObject &transfer, const VMessServerObject &server, const QString &alias)
        {
            QJsonObject vmessUriRoot;
            // Constant
            vmessUriRoot["v"] = 2;
            vmessUriRoot["ps"] = alias;
            vmessUriRoot["add"] = server.address;
            vmessUriRoot["port"] = server.port;
            vmessUriRoot["id"] = server.users.front().id;
            vmessUriRoot["aid"] = server.users.front().alterId;
            vmessUriRoot["net"] = transfer.network == "http" ? "h2" : transfer.network;
            vmessUriRoot["tls"] = transfer.security;

            if (transfer.network == "tcp")
            {
                vmessUriRoot["type"] = transfer.tcpSettings.header.type;
            }
            else if (transfer.network == "kcp")
            {
                vmessUriRoot["type"] = transfer.kcpSettings.header.type;
            }
            else if (transfer.network == "quic")
            {
                vmessUriRoot["type"] = transfer.quicSettings.header.type;
                vmessUriRoot["host"] = transfer.quicSettings.security;
                vmessUriRoot["path"] = transfer.quicSettings.key;
            }
            else if (transfer.network == "ws")
            {
                auto x = transfer.wsSettings.headers;
                auto host = x.contains("host");
                auto CapHost = x.contains("Host");
                auto realHost = host ? x["host"] : (CapHost ? x["Host"] : "");
                //
                vmessUriRoot["host"] = realHost;
                vmessUriRoot["path"] = transfer.wsSettings.path;
            }
            else if (transfer.network == "h2" || transfer.network == "http")
            {
                vmessUriRoot["host"] = transfer.httpSettings.host.join(",");
                vmessUriRoot["path"] = transfer.httpSettings.path;
            }

            //
            auto vmessPart = Base64Encode(JsonToString(vmessUriRoot, QJsonDocument::JsonFormat::Compact));
            return "vmess://" + vmessPart;
        }
        // This generates global config containing only one outbound....
        CONFIGROOT ConvertConfigFromVMessString(const QString &vmessStr, QString *alias, QString *errMessage)
        {
#define default CONFIGROOT()
            LOG(MODULE_SETTINGS, "Trying to convert from a vmess string.")
            QString vmess = vmessStr;

            if (vmess.trimmed() != vmess)
            {
                LOG(MODULE_SETTINGS, "VMess string has some prefix/postfix spaces, trimming.")
                vmess = vmessStr.trimmed();
            }

            // Reset errMessage
            *errMessage = "";

            if (!vmess.toLower().startsWith("vmess://"))
            {
                *errMessage = QObject::tr("VMess string should start with 'vmess://'");
                return default;
            }

            QStringRef vmessJsonB64(&vmess, 8, vmess.length() - 8);
            auto b64Str = vmessJsonB64.toString();

            if (b64Str.isEmpty())
            {
                *errMessage = QObject::tr("VMess string should be a valid base64 string");
                return default;
            }

            auto vmessString = Base64Decode(b64Str);
            auto jsonErr = VerifyJsonString(vmessString);

            if (!jsonErr.isEmpty())
            {
                *errMessage = jsonErr;
                return default;
            }

            auto vmessConf = JsonFromString(vmessString);

            if (vmessConf.isEmpty())
            {
                *errMessage = QObject::tr("JSON should not be empty");
                return default;
            }

            // Explicitly don't support v1 vmess links.
            if (!vmessConf.contains("v")) {
                *errMessage = QObject::tr("seems like a v1 vmess, we don't support it");
                return default;
            }
            bool flag = true;
            // C is a quick hack...
#define C(k) (flag = (vmessConf.contains(k) ? (errMessage->clear(), true) : (*errMessage += (k " does not exist"), false)))
            // id, aid, port and add are mandatory fields of a vmess://
            // link.
            flag = flag && C("id") && (C("aid") || C("alterId")) && C("port") && C("add");
            // Stream Settings
            auto net = vmessConf["net"].toString();

            if (net == "http" || net == "ws")
                flag = flag && C("host") && C("path");
            else if (net == "domainsocket")
                flag = flag && C("path");
            else if (net == "quic")
                flag = flag && C("host") && C("type") && C("path");

#undef C
            // return flag ? 0 : 1;

            // --------------------------------------------------------------------------------------
            CONFIGROOT root;
            QString ps, add, id, /*net,*/ type, host, path, tls;
            int port, aid;
            //
            // __vmess_checker__func(key, values)
            //
            //   - Key     =    Key in JSON and the variable name.
            //   - Values  =    Candidate variable list, if not match, the first one is used as default.
            //
            //   - [[val.size() <= 1]] is used when only the default value exists.
            //
            //   - It can be empty, if so,           if the key is not in the JSON, or the value is empty,  report an error.
            //   - Else if it contains one thing.    if the key is not in the JSON, or the value is empty,  use that one.
            //   - Else if it contains many things,  when the key IS in the JSON but not within the THINGS, use the first in the THINGS
            //   - Else,                                                                                    use the JSON value
            //
#define __vmess_checker__func(key, values)                                                                                                      \
    {                                                                                                                                           \
        auto val = QStringList() values;                                                                                                        \
        if (vmessConf.contains(#key) && !vmessConf[#key].toVariant().toString().trimmed().isEmpty() &&                                          \
            (val.size() <= 1 || val.contains(vmessConf[#key].toVariant().toString())))                                                          \
        {                                                                                                                                       \
            key = vmessConf[#key].toVariant().toString();                                                                                       \
        }                                                                                                                                       \
        else if (!val.isEmpty())                                                                                                                \
        {                                                                                                                                       \
            key = val.first();                                                                                                                  \
            DEBUG(MODULE_IMPORT, "Using key \"" #key "\" from the first candidate list: " + key)                                                \
        }                                                                                                                                       \
        else                                                                                                                                    \
        {                                                                                                                                       \
            *errMessage = QObject::tr(#key " does not exist.");                                                                                 \
            LOG(MODULE_IMPORT, "Cannot process \"" #key "\" since it's not included in the json object.")                                       \
            LOG(MODULE_IMPORT, " --> values: " + val.join(";"))                                                                                 \
            LOG(MODULE_IMPORT, " --> PS: " + ps)                                                                                                \
        }                                                                                                                                       \
    }
            // Strict check of VMess protocol, to check if the specified value
            // is in the correct range.
            //
            // Get Alias (AKA ps) from address and port.
            {
                // Some idiot vmess:// links are using alterId...
                aid = vmessConf.contains("aid") ? vmessConf.value("aid").toInt() : vmessConf.value("alterId").toInt();
                //
                //
                __vmess_checker__func(ps, << vmessConf["add"].toVariant().toString() + ":" + vmessConf["port"].toVariant().toString()); //
                __vmess_checker__func(add, nothing);                                                                                    //
                __vmess_checker__func(id, nothing);                                                                                     //
                __vmess_checker__func(type, << "none"                                                                                   //
                                            << "http"                                                                                   //
                                            << "srtp"                                                                                   //
                                            << "utp"                                                                                    //
                                            << "wechat-video");                                                                         //
                                                                                                                                        //
                __vmess_checker__func(net, << "tcp"                                                                                     //
                                           << "http"                                                                                    //
                                           << "h2"                                                                                      //
                                           << "ws"                                                                                      //
                                           << "kcp"                                                                                     //
                                           << "domainsocket"                                                                            //
                                           << "quic");                                                                                  //
                                                                                                                                        //
                __vmess_checker__func(path, << "");                                                                                     //
                __vmess_checker__func(host, << "");                                                                                     //
                __vmess_checker__func(tls, << "");                                                                                      //
            }
            // Repect connection type rather than obfs type //
            if (QStringList{ "srtp", "utp", "wechat-video" }.contains(type))                //
            {                                                                               //
                if (net != "quic" && net != "kcp")                                          //
                {                                                                           //
                    LOG(MODULE_CONNECTION, "Reset obfs settings from " + type + " to none") //
                    type = "none";                                                          //
                }                                                                           //
            }
            port = vmessConf["port"].toVariant().toInt();
            aid = vmessConf["aid"].toVariant().toInt();
            //
            // Apply the settings.
            // User
            VMessServerObject::UserObject user;
            user.id = id;
            user.alterId = aid;
            //
            // Server
            VMessServerObject serv;
            serv.port = port;
            serv.address = add;
            serv.users.push_back(user);
            //
            // VMess root config
            OUTBOUNDSETTING vConf;
            QJsonArray vnextArray;
            vnextArray.append(JsonFromString(StructToJsonString(serv)));
            vConf["vnext"] = vnextArray;
            //
            // Stream Settings
            StreamSettingsObject streaming;

            if (net == "tcp")
            {
                streaming.tcpSettings.header.type = type;
            }
            else if (net == "http" || net == "h2")
            {
                // Fill hosts for HTTP
                for (auto _host : host.split(','))
                {
                    if (!_host.isEmpty())
                    {
                        streaming.httpSettings.host.push_back(_host.trimmed());
                    }
                }

                streaming.httpSettings.path = path;
            }
            else if (net == "ws")
            {
                streaming.wsSettings.headers["Host"] = host;
                streaming.wsSettings.path = path;
            }
            else if (net == "kcp")
            {
                streaming.kcpSettings.header.type = type;
            }
            else if (net == "domainsocket")
            {
                streaming.dsSettings.path = path;
            }
            else if (net == "quic")
            {
                streaming.quicSettings.security = host;
                streaming.quicSettings.header.type = type;
                streaming.quicSettings.key = path;
            }

            // FIXME: makeshift patch for #290.
            //        to be rewritten after refactoring.
            if (tls == "tls" && host != "" && (net == "tcp" || net == "ws"))
            {
                streaming.tlsSettings.serverName = host;
                streaming.tlsSettings.allowInsecure = false;
            }

            streaming.security = tls;
            //
            // Network type
            // NOTE(DuckSoft): Damn vmess:// just don't write 'http' properly
            if (net == "h2")
                net = "http";
            streaming.network = net;
            //
            // WARN Mux is missing here.
            auto outbound = GenerateOutboundEntry("vmess", vConf, GetRootObject(streaming), QJsonObject(), "0.0.0.0", OUTBOUND_TAG_PROXY);
            //
            root["outbounds"] = QJsonArray() << outbound;
            // If previous alias is empty, just the PS is needed, else, append a "_"
            *alias = alias->trimmed().isEmpty() ? ps : *alias + "_" + ps;
            return root;
#undef default
        }
    } // namespace Serialization::vmess
} // namespace Qv2ray::core::connection
