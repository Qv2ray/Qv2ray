#include "QvCoreConfigOperations.hpp"

namespace Qv2ray
{
    namespace ConfigOperations
    {
        namespace Conversion
        {
            QString DecodeSubscriptionString(QByteArray arr)
            {
                // Some subscription providers may use plain vmess:// saperated by lines
                // But others may use base64 of above.
                auto result = QString::fromUtf8(arr).trimmed();
                return result.startsWith("vmess://") ? result : Base64Decode(result);
            }
            //
            /// Save Connection to a place, with checking if there's existing file.
            /// If so, append "_N" to the name.
            bool SaveConnectionConfig(QJsonObject obj, QString *alias, bool canOverrideExisting)
            {
                auto str = JsonToString(obj);
                QFile *config = new QFile(QV2RAY_CONFIG_DIR + *alias + QV2RAY_CONFIG_FILE_EXTENSION);

                // If there's already a file AND we CANNOT override existing file.
                if (config->exists() && !canOverrideExisting) {
                    // Alias is a pointer to a QString.
                    DeducePossibleFileName(QV2RAY_CONFIG_DIR, alias, QV2RAY_CONFIG_FILE_EXTENSION);
                    config = new QFile(QV2RAY_CONFIG_DIR + *alias + QV2RAY_CONFIG_FILE_EXTENSION);
                }

                LOG(MODULE_CONFIG, "Saving a config named: " + alias->toStdString())
                return StringToFile(&str, config);
            }

            bool RemoveConnection(const QString &alias)
            {
                QFile config(QV2RAY_CONFIG_DIR + alias + QV2RAY_CONFIG_FILE_EXTENSION);

                if (!config.exists()) {
                    LOG(MODULE_FILE, "Trying to remove a non-existing file?")
                    return false;
                } else {
                    return config.remove();
                }
            }

            // This generates global config containing only one outbound....
            QJsonObject ConvertConfigFromVMessString(QString vmess, QString *alias, QString *errMessage)
            {
                // Reset errMessage
                *errMessage = "";

                if (!vmess.toLower().startsWith("vmess://")) {
                    *errMessage = QObject::tr("VMess string should start with 'vmess://'");
                    return QJsonObject();
                }

                try {
                    QStringRef vmessJsonB64(&vmess, 8, vmess.length() - 8);
                    auto b64Str = vmessJsonB64.toString();

                    if (b64Str.isEmpty()) {
                        *errMessage = QObject::tr("VMess string should be a valid base64 string");
                        return QJsonObject();
                    }

                    auto vmessString = Base64Decode(b64Str);
                    auto jsonErr = VerifyJsonString(&vmessString);

                    if (!jsonErr.isEmpty()) {
                        *errMessage = jsonErr;
                        return QJsonObject();
                    }

                    auto vmessConf = JsonFromString(vmessString);

                    if (vmessConf.isEmpty()) {
                        *errMessage = QObject::tr("JSON should not be empty");
                        return QJsonObject();
                    }

                    // C is a quick hack...
#define C(k) vmessConf.contains(k)
                    bool flag = true;
                    flag = flag && C("id");
                    flag = flag && C("aid");
                    flag = flag && C("port");
                    flag = flag && C("add");
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
                    LOG(MODULE_CONNECTION_VMESS, "Failed to decode vmess string: " << e->what())
                    *errMessage = QSTRING(e->what());
                    return QJsonObject();
                }

                // --------------------------------------------------------------------------------------
                DROOT
                QStringRef vmessJsonB64(&vmess, 8, vmess.length() - 8);
                auto vmessConf = JsonFromString(Base64Decode(vmessJsonB64.toString()));
                //
                string ps, add, id, net, type, host, path, tls;
                int port, aid;
                //
                ps = vmessConf.contains("ps") ? vmessConf["ps"].toVariant().toString().toStdString()
                     : (vmessConf["add"].toVariant().toString().toStdString() + ":" + vmessConf["port"].toVariant().toString().toStdString());
                add = vmessConf["add"].toVariant().toString().toStdString();
                id = vmessConf["id"].toVariant().toString().toStdString();
                net = vmessConf.contains("net") ? vmessConf["net"].toVariant().toString().toStdString() : "tcp";
                type = vmessConf.contains("type") ? vmessConf["type"].toVariant().toString().toStdString() : "none";
                host = vmessConf["host"].toVariant().toString().toStdString();
                path = vmessConf["path"].toVariant().toString().toStdString();
                tls = vmessConf.contains("tls") ? vmessConf["tls"].toVariant().toString().toStdString() : "";
                //
                port = vmessConf["port"].toVariant().toInt();
                aid = vmessConf["aid"].toVariant().toInt();
                //
                // More strict check could be implemented, such as to check if the specified value is
                // in the currect format.
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
                QJsonObject vConf;
                QJsonArray vnextArray;
                vnextArray.append(JsonFromString(StructToJsonString(serv)));
                vConf["vnext"] = vnextArray;
                //
                // Stream Settings
                StreamSettingsObject streaming;

                if (net == "tcp") {
                    streaming.tcpSettings.header.type = type;
                } else if (net == "http") {
                    // Fill hosts for HTTP
                    for (auto _host : QString::fromStdString(host).split(',')) {
                        streaming.httpSettings.host.push_back(_host.toStdString());
                    }

                    streaming.httpSettings.path = path;
                } else if (net == "ws") {
                    streaming.wsSettings.headers.insert(make_pair("Host", host));
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

                streaming.security = tls;
                //
                // Network type
                streaming.network = net;
                //
                // WARN Mux is missing here.
                auto outbound = GenerateOutboundEntry("vmess", vConf, GetRootObject(streaming), QJsonObject(), "0.0.0.0", OUTBOUND_TAG_PROXY);
                //
                QJsonArray outbounds;
                outbounds.append(outbound);
                root["outbounds"] = outbounds;
                *alias = *alias + "_" + QSTRING(ps);
                RROOT
            }

            QJsonObject ConvertConfigFromFile(QString sourceFilePath, bool overrideInbounds)
            {
                QFile source(sourceFilePath);

                if (!source.exists()) {
                    LOG(MODULE_FILE, "Trying to import from an non-existing file.")
                    return QJsonObject();
                }

                auto root = JsonFromString(StringFromFile(new QFile(sourceFilePath)));

                if (overrideInbounds) {
                    JSON_ROOT_TRY_REMOVE("inbounds")
                }

                JSON_ROOT_TRY_REMOVE("log")
                JSON_ROOT_TRY_REMOVE("api")
                JSON_ROOT_TRY_REMOVE("stats")
                JSON_ROOT_TRY_REMOVE("dns")
                return root;
            }

            QMap<QString, QJsonObject> GetConnections(list<string> connectionNames)
            {
                QMap<QString, QJsonObject> list;

                for (auto conn : connectionNames) {
                    QString jsonString = StringFromFile(new QFile(QV2RAY_CONFIG_DIR + QSTRING(conn) + QV2RAY_CONFIG_FILE_EXTENSION));
                    QJsonObject connectionObject = JsonFromString(jsonString);
                    list.insert(QString::fromStdString(conn), connectionObject);
                }

                return list;
            }

            bool RenameConnection(QString originalName, QString newName)
            {
                LOG(MODULE_FILE, "[RENAME] --> ORIGINAL: " + originalName.toStdString() + ", NEW: " + newName.toStdString())
                return QFile::rename(QV2RAY_CONFIG_DIR + originalName + QV2RAY_CONFIG_FILE_EXTENSION, QV2RAY_CONFIG_DIR + newName + QV2RAY_CONFIG_FILE_EXTENSION);
            }

            int StartPreparation(QJsonObject fullConfig)
            {
                // Writes the final configuration to the disk.
                QString json = JsonToString(fullConfig);
                StringToFile(&json, new QFile(QV2RAY_GENERATED_FILE_PATH));
                return 0;
            }

            int FindIndexByTag(QJsonArray list, QString *tag)
            {
                for (int i = 0; i < list.count(); i++) {
                    auto value = list[i].toObject();

                    if (value.contains("tag") && value["tag"].toString() == *tag)
                        return i;
                }

                return -1;
            }
        }
    }
}
