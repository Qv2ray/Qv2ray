#include "Serialization.hpp"
#include "Generation.hpp"
#include "common/QvHelpers.hpp"
#include "core/CoreUtils.hpp"

namespace Qv2ray::core::connection
{
    namespace Serialization
    {
        CONFIGROOT ConvertConfigFromString(const QString &link, QString *alias, QString *errMessage)
        {
            CONFIGROOT config;

            if (link.startsWith("vmess://")) {
                config = ConvertConfigFromVMessString(link, alias, errMessage);
            } else if (link.startsWith("ss://")) {
                config = ConvertConfigFromSSString(link, alias, errMessage);
            } else {
                *errMessage = QObject::tr("Unsupported share link format.");
            }

            return config;
        }


        QString ConvertConfigToString(const CONFIGROOT &server, const QString &alias, bool isSip002)
        {
            OUTBOUND outbound = OUTBOUND(server["outbounds"].toArray().first().toObject());
            auto info = GetConnectionInfo(server);
            auto type = get<2>(info);
            QString sharelink = "";

            if (type == "vmess") {
                auto vmessServer = StructFromJsonString<VMessServerObject>(JsonToString(outbound["settings"].toObject()["vnext"].toArray().first().toObject()));
                auto transport = StructFromJsonString<StreamSettingsObject>(JsonToString(outbound["streamSettings"].toObject()));
                sharelink = ConvertConfigToVMessString(transport, vmessServer, alias);
            } else if (type == "shadowsocks") {
                auto ssServer = StructFromJsonString<ShadowSocksServerObject>(JsonToString(outbound["settings"].toObject()["servers"].toArray().first().toObject()));
                sharelink = ConvertConfigToSSString(ssServer, alias, isSip002);
            } else {
                LOG(CONNECTION, "Unsupported outbound type: " + type)
            }

            return sharelink;
        }

        // From https://github.com/2dust/v2rayN/wiki/%E5%88%86%E4%BA%AB%E9%93%BE%E6%8E%A5%E6%A0%BC%E5%BC%8F%E8%AF%B4%E6%98%8E(ver-2)
        QString ConvertConfigToVMessString(const StreamSettingsObject &transfer, const VMessServerObject &serverConfig, const QString &alias)
        {
            QJsonObject vmessUriRoot;
            // Constant
            vmessUriRoot["v"] = 2;
            vmessUriRoot["ps"] = alias;
            vmessUriRoot["add"] = serverConfig.address;
            vmessUriRoot["port"] = serverConfig.port;
            vmessUriRoot["id"] = serverConfig.users.front().id;
            vmessUriRoot["aid"] = serverConfig.users.front().alterId;
            vmessUriRoot["net"] = transfer.network;
            vmessUriRoot["tls"] = transfer.security;

            if (transfer.network == "tcp") {
                vmessUriRoot["type"] = transfer.tcpSettings.header.type;
            } else if (transfer.network == "kcp") {
                vmessUriRoot["type"] = transfer.kcpSettings.header.type;
            } else if (transfer.network == "quic") {
                vmessUriRoot["type"] = transfer.quicSettings.header.type;
                vmessUriRoot["host"] = transfer.quicSettings.security;
                vmessUriRoot["path"] = transfer.quicSettings.key;
            } else if (transfer.network == "ws") {
                auto x = transfer.wsSettings.headers;
                auto host = x.contains("host");
                auto CapHost = x.contains("Host");
                auto realHost = host ? x["host"] : (CapHost ? x["Host"] : "");
                //
                vmessUriRoot["host"] = realHost;
                vmessUriRoot["path"] = transfer.wsSettings.path;
            } else if (transfer.network == "h2" || transfer.network == "http") {
                vmessUriRoot["host"] = transfer.httpSettings.host.join(",");
                vmessUriRoot["path"] = transfer.httpSettings.path;
            }

            //
            auto vmessPart = Base64Encode(JsonToString(vmessUriRoot, QJsonDocument::JsonFormat::Compact));
            return "vmess://" + vmessPart;
        }

        QString DecodeSubscriptionString(QByteArray arr)
        {
            // String may start with: vmess:// and ss://
            // We only process vmess:// here
            // Some subscription providers may use plain vmess:// saperated by lines
            // But others may use base64 of above.
            auto result = QString::fromUtf8(arr).trimmed();
            return result.startsWith("vmess://") ? result : Base64Decode(result);
        }

        CONFIGROOT ConvertConfigFromSSString(const QString &ssUri, QString *alias, QString *errMessage)
        {
            ShadowSocksServerObject server;
            QString d_name;

            //auto ssUri = _ssUri.toStdString();
            if (ssUri.length() < 5) {
                LOG(CONNECTION, "ss:// string too short")
                *errMessage = QObject::tr("SS URI is too short");
            }

            auto uri = ssUri.mid(5);
            auto hashPos = uri.lastIndexOf("#");
            DEBUG(CONNECTION, "Hash sign position: " + QSTRN(hashPos))

            if (hashPos >= 0) {
                // Get the name/remark
                d_name = uri.mid(uri.lastIndexOf("#") + 1);
                uri.truncate(hashPos);
            }

            // No plugins for Qv2ray so disable those lnes.i
            //size_t pluginPos = uri.find_first_of('/');
            //
            //if (pluginPos != std::string::npos) {
            //    // TODO: support plugins. For now, just ignore them
            //    uri.erase(pluginPos);
            //}
            auto atPos = uri.indexOf('@');
            DEBUG(CONNECTION, "At sign position: " + QSTRN(atPos))

            if (atPos < 0) {
                // Old URI scheme
                QString decoded = QByteArray::fromBase64(uri.toUtf8(), QByteArray::Base64Option::OmitTrailingEquals);
                auto colonPos = decoded.indexOf(':');
                DEBUG(CONNECTION, "Colon position: " + QSTRN(colonPos))

                if (colonPos < 0) {
                    *errMessage = QObject::tr("Can't find the colon separator between method and password");
                }

                server.method = decoded.left(colonPos);
                decoded.remove(0, colonPos + 1);
                atPos = decoded.lastIndexOf('@');
                DEBUG(CONNECTION, "At sign position: " + QSTRN(atPos))

                if (atPos < 0) {
                    *errMessage = QObject::tr("Can't find the at separator between password and hostname");
                }

                server.password = decoded.mid(0, atPos);
                decoded.remove(0, atPos + 1);
                colonPos = decoded.lastIndexOf(':');
                DEBUG(CONNECTION, "Colon position: " + QSTRN(colonPos))

                if (colonPos < 0) {
                    *errMessage = QObject::tr("Can't find the colon separator between hostname and port");
                }

                server.address = decoded.mid(0, colonPos);
                server.port = decoded.mid(colonPos + 1).toInt();
            } else {
                // SIP002 URI scheme
                auto x = QUrl::fromUserInput(uri);
                server.address = x.host();
                server.port = x.port();
                QString userInfo = Base64Decode(x.userName());
                auto userInfoSp = userInfo.indexOf(':');
                //
                DEBUG(CONNECTION, "Userinfo splitter position: " + QSTRN(userInfoSp))

                if (userInfoSp < 0) {
                    *errMessage = QObject::tr("Can't find the colon separator between method and password");
                }

                QString method = userInfo.mid(0, userInfoSp);
                server.method = method;
                server.password = userInfo.mid(userInfoSp + 1);
            }

            d_name = QUrl::fromPercentEncoding(d_name.toUtf8());
            CONFIGROOT root;
            OUTBOUNDS outbounds;
            outbounds.append(GenerateOutboundEntry("shadowsocks", GenerateShadowSocksOUT(QList<ShadowSocksServerObject>() << server), QJsonObject()));
            JADD(outbounds)
            *alias = alias->isEmpty() ? d_name : *alias + "_" + d_name;
            LOG(CONNECTION, "Deduced alias: " + *alias)
            return root;
        }

        QString ConvertConfigToSSString(const ShadowSocksServerObject &server, const QString &alias, bool isSip002)
        {
            auto myAlias = QUrl::toPercentEncoding(alias);

            if (isSip002) {
                LOG(CONNECTION, "Converting an ss-server config to Sip002 ss:// format")
                QString plainUserInfo = server.method + ":" + server.password;
                QString userinfo(plainUserInfo.toUtf8().toBase64(QByteArray::Base64Option::Base64UrlEncoding).data());
                return "ss://" + userinfo + "@" + server.address + ":" + QSTRN(server.port) + "#" + myAlias;
            } else {
                LOG(CONNECTION, "Converting an ss-server config to old ss:// string format")
                QString ssUri = server.method + ":" + server.password + "@" + server.address + ":" + QSTRN(server.port);
                return "ss://" + ssUri.toUtf8().toBase64(QByteArray::Base64Option::OmitTrailingEquals) + "#" + myAlias;
            }
        }

        //
        // This generates global config containing only one outbound....
        CONFIGROOT ConvertConfigFromVMessString(const QString &vmessStr, QString *alias, QString *errMessage)
        {
#define default CONFIGROOT()
            LOG(SETTINGS, "Trying to convert from a vmess string.")
            QString vmess = vmessStr;

            if (vmess.trimmed() != vmess) {
                LOG(SETTINGS, "VMess string has some prefix/postfix spaces, trimming.")
                vmess = vmessStr.trimmed();
            }

            // Reset errMessage
            *errMessage = "";

            if (!vmess.toLower().startsWith("vmess://")) {
                *errMessage = QObject::tr("VMess string should start with 'vmess://'");
                return default;
            }

            try {
                QStringRef vmessJsonB64(&vmess, 8, vmess.length() - 8);
                auto b64Str = vmessJsonB64.toString();

                if (b64Str.isEmpty()) {
                    *errMessage = QObject::tr("VMess string should be a valid base64 string");
                    return default;
                }

                auto vmessString = Base64Decode(b64Str);
                auto jsonErr = VerifyJsonString(vmessString);

                if (!jsonErr.isEmpty()) {
                    *errMessage = jsonErr;
                    return default;
                }

                auto vmessConf = JsonFromString(vmessString);

                if (vmessConf.isEmpty()) {
                    *errMessage = QObject::tr("JSON should not be empty");
                    return default;
                }

                bool flag = true;
                // C is a quick hack...
#define C(k) vmessConf.contains(k)
                // id, aid, port and add are mandatory fields of a vmess:// link.
                flag = flag && C("id") && C("aid") && C("port") && C("add");
                // Stream Settings
                auto net = C("net") ? vmessConf["net"].toString() : "tcp";

                if (net == "http" || net == "ws")
                    flag = flag && C("host") && C("path");
                else if (net == "domainsocket")
                    flag = flag && C("path");
                else if (net == "quic")
                    flag = flag && C("host") && C("type") && C("path");

#undef C
                //return flag ? 0 : 1;
            } catch (exception *e) {
                LOG(IMPORT, "Failed to decode vmess string: " + QString(e->what()))
                *errMessage = e->what();
                return default;
            }

            // --------------------------------------------------------------------------------------
            CONFIGROOT root;
            auto b64String = QStringRef(&vmess, 8, vmess.length() - 8).toString();
            auto vmessConf = JsonFromString(Base64Decode(b64String));
            //
            QString ps, add, id, net, type, host, path, tls;
            int port, aid;
            //
            // key = key in JSON and the variable name.
            // values = Candidate variable list, if not match, the first one is used as default.
            //          [[val.size() <= 1]] is used when only the default value exists.
            //          - It can be empty, if so,           if the key is not in the JSON, or the value is empty,  it'll report an error.
            //          - Else if it contains one thing.    if the key is not in the JSON, or the value is empty,  it'll use that one.
            //          - Else if it contains many things,  when the key IS in the JSON but not in those THINGS,   it'll use the first one in the THINGS
            //          - Else, it'll use the value found from the JSON object.
            //
#define empty_arg
#define __vmess_checker__func(key, values) \
    {\
        auto val = QStringList() values;\
        if (vmessConf.contains(#key) && !vmessConf[#key].toVariant().toString().trimmed().isEmpty() \
            && (val.size() <= 1 || val.contains(vmessConf[#key].toVariant().toString()))) {\
            key = vmessConf[#key].toVariant().toString();\
            DEBUG(IMPORT, "Found key \"" #key "\" within the vmess object.")\
        } else if (!val.isEmpty()) {\
            key = val.first(); \
            DEBUG(IMPORT, "Using key \"" #key "\" from the first candidate list: " + key)\
        } else{\
            *errMessage = QObject::tr(#key " does not exist."); \
            LOG(IMPORT, "Cannot process \"" #key "\" since it's not included in the json object." ) \
            LOG(IMPORT, " --> values: " + val.join(";")) \
            LOG(IMPORT, " --> PS: " + ps) \
        }\
    }
            // Strict check of VMess protocol, to check if the specified value is in the correct range.
            //
            // Get Alias (AKA ps) from address and port.
            __vmess_checker__func(ps, << vmessConf["add"].toVariant().toString() + ":" + vmessConf["port"].toVariant().toString());
            __vmess_checker__func(add, empty_arg)
            __vmess_checker__func(id, empty_arg)
            __vmess_checker__func(net, << "tcp" << "http" << "h2" << "ws" << "kcp" << "domainsocket" << "quic")
            __vmess_checker__func(type, << "none" << "http" << "srtp" << "utp" << "wechat-video")
            __vmess_checker__func(path, << "")
            __vmess_checker__func(host, << "")
            __vmess_checker__func(tls, << "")
            //
            port = vmessConf["port"].toVariant().toInt();
            aid = vmessConf["aid"].toVariant().toInt();
            // Apply the settings.
            //
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

            if (net == "tcp") {
                streaming.tcpSettings.header.type = type;
            } else if (net == "http" || net == "h2") {
                // Fill hosts for HTTP
                for (auto _host : host.split(',')) {
                    streaming.httpSettings.host.push_back(_host.trimmed());
                }

                streaming.httpSettings.path = path;
            } else if (net == "ws") {
                streaming.wsSettings.headers["Host"] = host;
                streaming.wsSettings.path = path;
            } else if (net == "kcp") {
                streaming.kcpSettings.header.type = type;
            } else if (net == "domainsocket") {
                streaming.dsSettings.path = path;
            } else if (net == "quic") {
                streaming.quicSettings.security = host;
                streaming.quicSettings.header.type = type;
                streaming.quicSettings.key = path;
            }

            // FIXME: makeshift patch for #290.
            //        to be rewritten after refactoring.
            if (tls == "tls" && host != "" && (net == "tcp" || net == "ws")) {
                streaming.tlsSettings.serverName = host;
                streaming.tlsSettings.allowInsecure = false;
            }

            streaming.security = tls;
            //
            // Network type
            streaming.network = net;
            //
            // WARN Mux is missing here.
            auto outbound = GenerateOutboundEntry("vmess", vConf, GetRootObject(streaming), QJsonObject(), "0.0.0.0", OUTBOUND_TAG_PROXY);
            //
            root["outbounds"] = QJsonArray() << outbound;
            // If previous alias is empty, just the PS is needed, else, append a "_"
            *alias = alias->trimmed().isEmpty() ? ps : *alias + "_" + ps;
#undef default
            return root;
        }
    }
}
