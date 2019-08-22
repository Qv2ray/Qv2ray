#include "QvCoreConfigOperations.h"
namespace Qv2ray
{
    namespace ConfigOperations
    {
        // -------------------------- BEGIN CONFIG CONVERSIONS ----------------------------------------------------------------------------
        bool SaveConnectionConfig(QJsonObject obj, const QString *alias)
        {
            QFile config(QV2RAY_CONFIG_DIR_PATH + *alias + QV2RAY_CONNECTION_FILE_EXTENSION);
            return StringToFile(JSONToString(obj), &config);
        }

        // This generates global config containing only one outbound....
        QJsonObject ConvertConfigFromVMessString(QString str)
        {
            DROOT
            QStringRef vmessJsonB64(&str, 8, str.length() - 8);
            auto vmessConf = StructFromJSONString<VMessProtocolConfigObject>(Base64Decode(vmessJsonB64.toString()));
            //
            // User
            VMessServerObject::UserObject user;
            user.id = vmessConf.id;
            user.alterId = stoi(vmessConf.aid);
            //
            // Server
            VMessServerObject serv;
            serv.port = stoi(vmessConf.port);
            serv.address = vmessConf.add;
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

            // Fill hosts for HTTP
            foreach (auto host, QString::fromStdString(vmessConf.host).split(',')) {
                streaming.httpSettings.host.push_back(host.toStdString());
            }

            // hosts for ws, h2 and security for QUIC
            streaming.wsSettings.headers.insert(make_pair("Host", vmessConf.host));
            streaming.quicSettings.security = vmessConf.host;
            //
            // Fake type for tcp, kcp and QUIC
            streaming.tcpSettings.header.type = vmessConf.type;
            streaming.kcpSettings.header.type = vmessConf.type;
            streaming.quicSettings.header.type = vmessConf.type;
            //
            // Path for ws, h2, Quic
            streaming.wsSettings.path = vmessConf.path;
            streaming.httpSettings.path = vmessConf.path;
            streaming.quicSettings.key = vmessConf.path;
            streaming.security = vmessConf.tls;
            //
            // Network type
            streaming.network = vmessConf.net;
            //
            auto outbound = GenerateOutboundEntry("vmess", vConf, GetRootObject(streaming), GetRootObject(GetGlobalConfig().mux), "0.0.0.0", OUTBOUND_TAG_PROXY);
            //
            QJsonArray outbounds;
            outbounds.append(outbound);
            root.insert("outbounds", outbounds);
            root.insert("QV2RAY_ALIAS", QString::fromStdString(vmessConf.ps));
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
            StringToFile(json, new QFile(QV2RAY_GENERATED_FILE_PATH));
            return 0;
        }
    }
}
