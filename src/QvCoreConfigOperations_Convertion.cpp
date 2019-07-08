#include "QvCoreConfigOperations.h"
namespace Qv2ray
{
    namespace ConfigOperations
    {
        // -------------------------- BEGIN CONFIG CONVERSIONS ----------------------------------------------------------------------------
        int SaveConnectionConfig(QJsonObject obj, const QString *alias)
        {
            QFile config(QV2RAY_CONFIG_PATH + *alias + QV2RAY_CONNECTION_FILE_EXTENSION);

            if (config.exists()) {
                return -1;
            }

            StringToFile(JSONToString(obj), &config);
            return 0;
        }

        // This generates global config containing only one outbound....
        QJsonObject ConvertConfigFromVMessString(QString str)
        {
            DROOT
            QStringRef vmessJsonB64(&str, 8, str.length() - 8);
            auto vmessConf = StructFromJSONString<VMessProtocolConfigObject>(Base64Decode(vmessJsonB64.toString()));
            //
            // User
            VMessOut::ServerObject::UserObject user;
            user.id = vmessConf.id;
            user.alterId = stoi(vmessConf.aid);
            //
            // Server
            VMessOut::ServerObject serv;
            serv.port = stoi(vmessConf.port);
            serv.address = vmessConf.add;
            serv.users.push_back(user);
            //
            // VMess root config
            VMessOut vConf;
            vConf.vnext.push_back(serv);
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
            auto outbound = GenerateOutboundEntry("vmess", GetRootObject(vConf), GetRootObject(streaming), GetRootObject(GetGlobalConfig().mux), "0.0.0.0", OUTBOUND_TAG_PROXY);
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

            //
            JSON_ROOT_TRY_REMOVE("log")
            JSON_ROOT_TRY_REMOVE("api")
            JSON_ROOT_TRY_REMOVE("stats")
            JSON_ROOT_TRY_REMOVE("policy")
            JSON_ROOT_TRY_REMOVE("dns")
            JSON_ROOT_TRY_REMOVE("routing")
            QJsonArray outbounds;

            //
            // Currently, we only support VMess. So remove all other types of outbounds.
            for (int i = root["outbounds"].toArray().count(); i >= 0 ; i--) {
                if (root["outbounds"].toArray()[i].toObject()["protocol"].toString() == "vmess") {
                    auto conn = root["outbounds"].toArray()[i].toObject();
                    conn.insert("tag", OUTBOUND_TAG_PROXY);
                    outbounds.append(conn);
                }
            }

            JSON_ROOT_TRY_REMOVE("outbounds")
            root.insert("outbounds", outbounds);
            return root;
        }

        QMap<QString, QJsonObject> LoadAllConnectionList(list<string> connections)
        {
            QMap<QString, QJsonObject> list;

            foreach (auto conn, connections) {
                QString jsonString = StringFromFile(new QFile(QV2RAY_CONFIG_PATH + QString::fromStdString(conn) + QV2RAY_CONNECTION_FILE_EXTENSION));
                QJsonObject connectionObject = JSONFromString(jsonString);
                list.insert(QString::fromStdString(conn), connectionObject);
            }

            return list;
        }

        VMessOut ConvertOutBoundJSONToStruct(QJsonObject vmessJson)
        {
            auto str = JSONToString(vmessJson);
            return StructFromJSONString<VMessOut>(str);
        }

        int StartPreparation(QJsonObject fullConfig)
        {
            QString json = JSONToString(fullConfig);
            StringToFile(json, new QFile(QV2RAY_GENERATED_CONFIG_FILE_PATH));
            return 0;
        }
    }
}
