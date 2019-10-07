#include "QvCoreConfigOperations.h"
namespace Qv2ray
{
    namespace ConfigOperations
    {
        // -------------------------- BEGIN CONFIG CONVERSIONS ----------------------------------------------------------------------------
        bool SaveConnectionConfig(QJsonObject obj, const QString *alias)
        {
            QFile config(QV2RAY_CONFIG_DIR_PATH + *alias + QV2RAY_CONNECTION_FILE_EXTENSION);
            auto str = JSONToString(obj);
            return StringToFile(&str, &config);
        }

        bool RemoveConnection(const QString *alias)
        {
            QFile config(QV2RAY_CONFIG_DIR_PATH + *alias + QV2RAY_CONNECTION_FILE_EXTENSION);
            return config.exists() && config.remove();
        }

        // This generates global config containing only one outbound....
        QJsonObject ConvertConfigFromVMessString(QString str)
        {
            DROOT
            QStringRef vmessJsonB64(&str, 8, str.length() - 8);
            auto vmessConf = JSONFromString(Base64Decode(vmessJsonB64.toString()));
            string ps, add, id, net, type, host, path, tls;
            int port, aid;
            ps = vmessConf["ps"].toVariant().toString().toStdString();
            add = vmessConf["add"].toVariant().toString().toStdString();
            id = vmessConf["id"].toVariant().toString().toStdString();
            net = vmessConf["net"].toVariant().toString().toStdString();
            type = vmessConf["type"].toVariant().toString().toStdString();
            host = vmessConf["host"].toVariant().toString().toStdString();
            path = vmessConf["path"].toVariant().toString().toStdString();
            tls = vmessConf["tls"].toVariant().toString().toStdString();
            //
            port = vmessConf["port"].toVariant().toInt();
            aid = vmessConf["aid"].toVariant().toInt();
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
            vnextArray.append(JSONFromString(StructToJSONString(serv)));
            vConf["vnext"] = vnextArray;
            //
            // Stream Settings
            StreamSettingsObject streaming;

            if (net == "tcp") {
                streaming.tcpSettings.header.type = type;
            } else if (net == "http") {
                // Fill hosts for HTTP
                foreach (auto _host, QString::fromStdString(host).split(',')) {
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
            auto outbound = GenerateOutboundEntry("vmess", vConf, GetRootObject(streaming), GetRootObject(GetGlobalConfig().mux), "0.0.0.0", OUTBOUND_TAG_PROXY);
            //
            QJsonArray outbounds;
            outbounds.append(outbound);
            root.insert("outbounds", outbounds);
            root.insert("QV2RAY_ALIAS", QString::fromStdString(ps));
            RROOT
        }

        QJsonObject ConvertConfigFromFile(QString sourceFilePath, bool overrideInbounds)
        {
            auto root = JSONFromString(StringFromFile(new QFile(sourceFilePath)));

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

            foreach (auto conn, connectionNames) {
                QString jsonString = StringFromFile(new QFile(QV2RAY_CONFIG_DIR_PATH + QString::fromStdString(conn) + QV2RAY_CONNECTION_FILE_EXTENSION));
                QJsonObject connectionObject = JSONFromString(jsonString);
                list.insert(QString::fromStdString(conn), connectionObject);
            }

            return list;
        }

        bool RenameConnection(QString originalName, QString newName)
        {
            return QFile(QV2RAY_CONFIG_DIR_PATH + originalName + QV2RAY_CONNECTION_FILE_EXTENSION).rename(QV2RAY_CONFIG_DIR_PATH + newName + QV2RAY_CONNECTION_FILE_EXTENSION);
        }

        int StartPreparation(QJsonObject fullConfig)
        {
            QString json = JSONToString(fullConfig);
            StringToFile(&json, new QFile(QV2RAY_GENERATED_FILE_PATH));
            return 0;
        }
    }
}
