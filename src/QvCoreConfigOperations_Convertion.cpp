#include "QvCoreConfigOperations.h"
namespace Qv2ray
{
    namespace ConfigOperations
    {
        // -------------------------- BEGIN CONFIG CONVERSIONS ----------------------------------------------------------------------------
        int SaveConnectionConfig(QJsonObject obj, const QString *alias)
        {
            QFile config(QV2RAY_CONFIG_PATH + alias + QV2RAY_CONNECTION_FILE_EXTENSION);

            if (config.exists()) {
                return -1;
            }

            QJsonDocument doc;
            doc.setObject(obj);
            SaveStringToFile(doc.toJson(), &config);
            return 0;
        }

        // This generates an "OutBoundObject"
        QJsonObject ConvertConfigFromVMessString(QString str)
        {
            DROOT
            QStringRef vmessJsonB64(&str, 8, str.length() - 8);
            auto vmessConf = StructFromJSON<VMessProtocolConfigObject>(Base64Decode(vmessJsonB64.toString()).toStdString());
            VMessOut vConf;
            VMessOut::ServerObject serv;
            serv.port = stoi(vmessConf.port);
            serv.address = vmessConf.add;
            // User
            VMessOut::ServerObject::UserObject user;
            user.id = vmessConf.id;
            user.alterId = stoi(vmessConf.aid);
            // Server
            serv.users.push_back(user);
            // VMess root config
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
            // Root
            root.insert("sendThrough", "0.0.0.0");
            root.insert("protocol", "vmess");
            root.insert("settings", GetRootObject(vConf));
            root.insert("tag", OUTBOUND_TAG_PROXY);
            root.insert("streamSettings", GetRootObject(streaming));
            root.insert("QV2RAY_ALIAS", QString::fromStdString(vmessConf.ps));
            RROOT
        }

        QJsonObject ConvertConfigFromFile(QString sourceFilePath, bool overrideInbounds)
        {
            auto globalConf = GetGlobalConfig();
            QFile configFile(sourceFilePath);
            configFile.open(QIODevice::ReadOnly | QIODevice::ExistingOnly);
            QByteArray allData = configFile.readAll();
            configFile.close();
            QJsonDocument v2conf = QJsonDocument::fromJson(allData);
            QJsonObject root = v2conf.object();

            if (overrideInbounds) {
                JSON_ROOT_TRY_REMOVE("inbounds")
            }

            //
            JSON_ROOT_TRY_REMOVE("log")
            JSON_ROOT_TRY_REMOVE("api")
            JSON_ROOT_TRY_REMOVE("stats")
            JSON_ROOT_TRY_REMOVE("policy")
            JSON_ROOT_TRY_REMOVE("dns")
            //
            return root;
        }
        /*
         * {
            QFile configFile(path);

            if (!configFile.open(QIODevice::ReadOnly)) {
                QvMessageBox(this, tr("ImportConfig"), tr("CannotOpenFile"));
                qDebug() << "ImportConfig::CannotOpenFile";
                return -1;
            }

            QByteArray allData = configFile.readAll();
            configFile.close();
            QJsonDocument v2conf(QJsonDocument::fromJson(allData));
            QJsonObject rootobj = v2conf.object();
            QJsonObject outbound;

            if (rootobj.contains("outbounds")) {
                outbound = rootobj.value("outbounds").toArray().first().toObject();
            } else {
                outbound = rootobj.value("outbound").toObject();
            }

            if (!QFile::copy(path, "newFile")) {
                QvMessageBox(this, tr("ImportConfig"), tr("CannotCopyCustomConfig"));
                qDebug() << "ImportConfig::CannotCopyCustomConfig";
            }
        }
        */
    }
}
