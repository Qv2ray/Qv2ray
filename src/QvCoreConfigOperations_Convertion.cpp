#include "QvCoreConfigOperations.hpp"
namespace Qv2ray
{
    namespace ConfigOperations
    {

        /// This only returns the file name without extensions.
        void DeducePossibleFileName(const QString &baseDir, QString *fileName, const QString &extension)
        {
            int i = 1;

            if (!QDir(baseDir).exists()) {
                QDir(baseDir).mkpath(baseDir);
                LOG(MODULE_FILE, "Making path: " + baseDir.toStdString())
            }

            while (true) {
                if (!QFile(baseDir + "/" + fileName + "_" + QString::number(i) + extension).exists()) {
                    *fileName = *fileName + "_" + QString::number(i);
                    return;
                } else {
                    //LOG(MODULE_FILE, "File with name: " << (fileName + "_" + QString::number(i) + extension).toStdString() << " already exists")
                }

                i++;
            }
        }

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
        QJsonObject ConvertConfigFromVMessString(QString str)
        {
            DROOT
            QStringRef vmessJsonB64(&str, 8, str.length() - 8);
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
            root.insert("outbounds", outbounds);
            root.insert("QV2RAY_ALIAS", QString::fromStdString(ps));
            RROOT
        }

        QJsonObject ConvertConfigFromFile(QString sourceFilePath, bool overrideInbounds)
        {
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
