#include "QvUtils.hpp"
#include "QApplication"

namespace Qv2ray
{
    namespace Utils
    {
        static bool _isQv2rayExiting = false;
        Qv2rayConfig GlobalConfig = Qv2rayConfig();
        QString Qv2rayConfigPath = "";
        void SaveGlobalConfig(Qv2rayConfig conf)
        {
            GlobalConfig = conf;
            QFile config(QV2RAY_CONFIG_FILE);
            QString str = StructToJsonString(conf);
            StringToFile(&str, &config);
        }

        void SetConfigDirPath(const QString &path)
        {
            Qv2rayConfigPath = path;

            if (!path.endsWith("/")) {
                Qv2rayConfigPath += "/";
            }
        }

        void LoadGlobalConfig()
        {
            QFile file(QV2RAY_CONFIG_FILE);
            file.open(QFile::ReadOnly);
            QTextStream stream(&file);
            auto str = stream.readAll();
            auto config  = StructFromJsonString<Qv2rayConfig>(str);
            SaveGlobalConfig(config);
            file.close();
        }

        void ExitQv2ray()
        {
            _isQv2rayExiting = true;
            QApplication::quit();
        }

        bool isExiting()
        {
            return _isQv2rayExiting;
        }
        tuple<QString, int, QString> GetConnectionInfo(const CONFIGROOT &root)
        {
            bool validOutboundFound = false;
            QString host;
            int port;

            for (auto item : root["outbounds"].toArray()) {
                OUTBOUND outBoundRoot = OUTBOUND(item.toObject());
                QString outboundType = "";
                validOutboundFound = GetOutboundData(outBoundRoot, &host, &port, &outboundType);

                if (validOutboundFound) {
                    return make_tuple(host, port, outboundType);
                } else {
                    LOG(MODULE_UI, "Unknown outbound entry: " + outboundType + ", cannot deduce host and port.")
                }
            }

            return make_tuple(QObject::tr("N/A"), 0, QObject::tr("N/A"));
        }

        bool GetOutboundData(const OUTBOUND &out, QString *host, int *port, QString *protocol)
        {
            // Set initial values.
            *host = QObject::tr("N/A");
            *port = 0;
            *protocol = out["protocol"].toString(QObject::tr("N/A"));

            if (*protocol == "vmess") {
                auto Server = StructFromJsonString<VMessServerObject>(JsonToString(out["settings"].toObject()["vnext"].toArray().first().toObject()));
                *host = Server.address;
                *port = Server.port;
                return true;
            } else if (*protocol == "shadowsocks") {
                auto x = JsonToString(out["settings"].toObject()["servers"].toArray().first().toObject());
                auto Server = StructFromJsonString<ShadowSocksServerObject>(x);
                *host = Server.address;
                *port = Server.port;
                return true;
            } else if (*protocol == "socks") {
                auto x = JsonToString(out["settings"].toObject()["servers"].toArray().first().toObject());
                auto Server = StructFromJsonString<SocksServerObject>(x);
                *host = Server.address;
                *port = Server.port;
                return true;
            } else {
                return false;
            }
        }
    }
}
