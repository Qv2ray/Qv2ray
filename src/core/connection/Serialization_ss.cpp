#include "Generation.hpp"
#include "Serialization.hpp"
#include "common/QvHelpers.hpp"
#include "core/CoreUtils.hpp"
#include "core/handler/ConfigHandler.hpp"

namespace Qv2ray::core::connection
{
    namespace Serialization::ss
    {
        CONFIGROOT ConvertConfigFromSSString(const QString &ssUri, QString *alias, QString *errMessage)
        {
            ShadowSocksServerObject server;
            QString d_name;

            // auto ssUri = _ssUri.toStdString();
            if (ssUri.length() < 5)
            {
                LOG(MODULE_CONNECTION, "ss:// string too short")
                *errMessage = QObject::tr("SS URI is too short");
            }

            auto uri = ssUri.mid(5);
            auto hashPos = uri.lastIndexOf("#");
            DEBUG(MODULE_CONNECTION, "Hash sign position: " + QSTRN(hashPos))

            if (hashPos >= 0)
            {
                // Get the name/remark
                d_name = uri.mid(uri.lastIndexOf("#") + 1);
                uri.truncate(hashPos);
            }

            // No plugins for Qv2ray so disable those lnes.i
            // size_t pluginPos = uri.find_first_of('/');
            //
            // if (pluginPos != std::string::npos) {
            //    // TODO: support plugins. For now, just ignore them
            //    uri.erase(pluginPos);
            //}
            auto atPos = uri.indexOf('@');
            DEBUG(MODULE_CONNECTION, "At sign position: " + QSTRN(atPos))

            if (atPos < 0)
            {
                // Old URI scheme
                QString decoded = QByteArray::fromBase64(uri.toUtf8(), QByteArray::Base64Option::OmitTrailingEquals);
                auto colonPos = decoded.indexOf(':');
                DEBUG(MODULE_CONNECTION, "Colon position: " + QSTRN(colonPos))

                if (colonPos < 0)
                {
                    *errMessage = QObject::tr("Can't find the colon separator between method and password");
                }

                server.method = decoded.left(colonPos);
                decoded.remove(0, colonPos + 1);
                atPos = decoded.lastIndexOf('@');
                DEBUG(MODULE_CONNECTION, "At sign position: " + QSTRN(atPos))

                if (atPos < 0)
                {
                    *errMessage = QObject::tr("Can't find the at separator between password and hostname");
                }

                server.password = decoded.mid(0, atPos);
                decoded.remove(0, atPos + 1);
                colonPos = decoded.lastIndexOf(':');
                DEBUG(MODULE_CONNECTION, "Colon position: " + QSTRN(colonPos))

                if (colonPos < 0)
                {
                    *errMessage = QObject::tr("Can't find the colon separator between hostname and port");
                }

                server.address = decoded.mid(0, colonPos);
                server.port = decoded.mid(colonPos + 1).toInt();
            }
            else
            {
                // SIP002 URI scheme
                auto x = QUrl::fromUserInput(uri);
                server.address = x.host();
                server.port = x.port();
                QString userInfo = Base64Decode(x.userName());
                auto userInfoSp = userInfo.indexOf(':');
                //
                DEBUG(MODULE_CONNECTION, "Userinfo splitter position: " + QSTRN(userInfoSp))

                if (userInfoSp < 0)
                {
                    *errMessage = QObject::tr("Can't find the colon separator between method and password");
                }

                QString method = userInfo.mid(0, userInfoSp);
                server.method = method;
                server.password = userInfo.mid(userInfoSp + 1);
            }

            d_name = QUrl::fromPercentEncoding(d_name.toUtf8());
            CONFIGROOT root;
            OUTBOUNDS outbounds;
            outbounds.append(
                GenerateOutboundEntry("shadowsocks", GenerateShadowSocksOUT(QList<ShadowSocksServerObject>{ server }), QJsonObject()));
            JADD(outbounds)
            *alias = alias->isEmpty() ? d_name : *alias + "_" + d_name;
            LOG(MODULE_CONNECTION, "Deduced alias: " + *alias)
            return root;
        }

        const QString ConvertConfigToSSString(const ShadowSocksServerObject &server, const QString &alias, bool isSip002)
        {
            auto myAlias = QUrl::toPercentEncoding(alias);

            if (isSip002)
            {
                LOG(MODULE_CONNECTION, "Converting an ss-server config to Sip002 ss:// format")
                QString plainUserInfo = server.method + ":" + server.password;
                QString userinfo(plainUserInfo.toUtf8().toBase64(QByteArray::Base64Option::Base64UrlEncoding).data());
                return "ss://" + userinfo + "@" + server.address + ":" + QSTRN(server.port) + "#" + myAlias;
            }
            else
            {
                LOG(MODULE_CONNECTION, "Converting an ss-server config to old ss:// string format")
                QString ssUri = server.method + ":" + server.password + "@" + server.address + ":" + QSTRN(server.port);
                return "ss://" + ssUri.toUtf8().toBase64(QByteArray::Base64Option::OmitTrailingEquals) + "#" + myAlias;
            }
        }
	CONFIGROOT ConvertConfigFromSSRString(const QString &ssrBase64Uri, QString *alias, QString *errMessage)
        {
            ShadowSocksRServerObject server;
            QString d_name;
            if(ssrBase64Uri.length()<6){
                LOG(MODULE_CONNECTION, "ssr:// string too short")
                *errMessage = QObject::tr("SSR URI is too short");
            }
            QRegExp regex("^(.+):([^:]+):([^:]*):([^:]+):([^:]*):([^:]+)");
            auto data= Base64Decode(ssrBase64Uri.mid(6));
            //bool matchSuccess = regex.exactMatch(ssrUrl);
            for (int nTry = 0; nTry < 2; ++nTry)
            {
                    int param_start_pos = data.indexOf("?");
                    if (param_start_pos > 0)
                    {
                            auto paramas = data.mid(param_start_pos + 1).split('&');
                            for(auto& parama:paramas)
                            {
                                auto pos=parama.indexOf('=');
                                auto key=parama.mid(0,pos);
                                auto val=parama.mid(pos+1);
                                if(key=="obfsparam"){
                                    server.obfs_param=common::SafeBase64Decode(val);
                                } else if(key=="protoparam"){
                                    server.protocol_param=common::SafeBase64Decode(val);
                                } else if(key=="remarks") {
                                    server.remarks=common::SafeBase64Decode(val);
                                    d_name=server.remarks;
                                } else if(key=="group") {
                                    server.group=common::SafeBase64Decode(val);
                                }
                            }
                            //params_dict = ParseParam(data.mid(param_start_pos + 1));
                            data = data.mid(0, param_start_pos);
                    }
                    if (data.indexOf("/") >= 0)
                    {
                            data = data.mid(0, data.lastIndexOf("/"));
                    }

                    auto matched = regex.exactMatch(data);
                    auto list=regex.capturedTexts();
                    if (matched&&list.length()==7){
                        server.address=list[1];
                        server.port=list[2].toInt();
                        server.protocol=list[3].length()==0?QString("origin"):list[3];
                        server.protocol=server.protocol.replace("_compatible","");
                        server.method=list[4];
                        server.obfs=list[5].length()==0?QString("plain"):list[5];
                        server.password=common::SafeBase64Decode(list[6]);
                        break;
                    }
                    *errMessage = QObject::tr("SSRUrl not matched regex \"^(.+):([^:]+):([^:]*):([^:]+):([^:]*):([^:]+)\"");
            }
            CONFIGROOT root;
            OUTBOUNDS outbounds;
            outbounds.append(GenerateOutboundEntry("shadowsocksr", GenerateShadowSocksROUT(QList<ShadowSocksRServerObject>() << server), QJsonObject()));
            JADD(outbounds)
            *alias = alias->isEmpty() ? d_name.isEmpty()?server.address+server.port+server.group+server.protocol+server.method+server.password:d_name : *alias + "_" + d_name;
            *alias = alias->trimmed();
            if(alias->isEmpty())
                    *errMessage = QObject::tr("SSRUrl empty");
            LOG(MODULE_CONNECTION, "Deduced alias: " + *alias)
            return root;
        }

        QString ConvertConfigToSSRString(const ShadowSocksRServerObject &server)
        {
            QString main_part = server.address + ":" + QString::number(server.port) + ":" + server.protocol + ":" + server.method + ":" + server.obfs + ":" + common::SafeBase64Encode(server.password);
            QString param_str = "obfsparam=" + common::SafeBase64Encode(server.obfs_param);
            if (!server.protocol_param.isEmpty())
            {
                param_str += "&protoparam=" + common::SafeBase64Encode(server.protocol_param);
            }
            if (!server.remarks.isEmpty())
            {
                param_str += "&remarks=" + common::SafeBase64Encode(server.remarks);
            }
            if (!server.group.isEmpty())
            {
                param_str += "&group=" + common::SafeBase64Encode(server.group);
            }
            QString base64 = common::SafeBase64Encode(main_part + "/?" + param_str);
            return "ssr://" + base64;
        }

        
    } // namespace Serialization::ss
} // namespace Qv2ray::core::connection
