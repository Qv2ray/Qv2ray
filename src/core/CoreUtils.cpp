#include "CoreUtils.hpp"
#include "common/QvHelpers.hpp"
#include "QApplication"

namespace Qv2ray::core
{
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
                LOG(UI, "Unknown outbound entry: " + outboundType + ", cannot deduce host and port.")
            }
        }

        return make_tuple(QObject::tr("N/A"), 0, QObject::tr("N/A"));
    }

    bool GetOutboundData(const OUTBOUND &out, QString *host, int *port, QString *protocol)
    {
        // Set initial values.
        *host = QObject::tr("N/A");
        *port = 0;
        *protocol = out["protocol"].toString(QObject::tr("N/A")).toLower();

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

    bool IsComplexConfig(CONFIGROOT root)
    {
        bool cRouting = root.contains("routing");
        bool cRule = cRouting && root["routing"].toObject().contains("rules");
        bool cRules = cRule && root["routing"].toObject()["rules"].toArray().count() > 0;
        //
        bool cInbounds = root.contains("inbounds");
        bool cInboundCount = cInbounds && root["inbounds"].toArray().count() > 0;
        //
        bool cOutbounds = root.contains("outbounds");
        bool cOutboundCount = cOutbounds && root["outbounds"].toArray().count() > 1;
        return cRules || cInboundCount || cOutboundCount;
    }
}

