#ifndef CONFIGGENERATION_H
#define CONFIGGENERATION_H

#include "QvUtils.h"
#include "QJsonObjectInsertMacros.h"

#define OUTBOUND_TAG_DIRECT "outBound_DIRECT"
#define OUTBOUND_TAG_PROXY "outBound_PROXY"

#define DROOT QJsonObject root;
#define RROOT return root;

namespace Qv2ray
{
    namespace ConfigOperations
    {
        // -------------------------- BEGIN CONFIG GENERATIONS ---------------------------------------------
        QJsonObject GenerateRoutes(bool globalProxy, bool cnProxy);
        QJsonObject GenerateSingleRouteRule(QStringList list, bool isDomain, QString outboundTag, QString type = "field");
        QJsonObject GenerateDNS(bool withLocalhost, QStringList dnsServers);
        //
        // OutBoundProtocols
        QJsonObject GenerateFreedomOUT(QString domainStrategy, QString redirect, int userLevel);
        QJsonObject GenerateBlackHoleOUT(bool useHTTP);
        QJsonObject GenerateShadowSocksServerOUT(QString email, QString address, int port, QString method, QString password, bool ota, int level);
        QJsonObject GenerateShadowSocksOUT(QList<QJsonObject> servers);
        //
        // InBoundsProtocols
        QJsonObject GenerateDokodemoIN(QString address, int port, QString  network, int timeout, bool followRedirect, int userLevel);
        QJsonObject GenerateHTTPIN(int timeout, QList<AccountObject> accounts, bool allowTransparent, bool userLevel);
        QJsonObject GenerateSocksIN(QString auth, QList<AccountObject> _accounts, bool udp, QString ip, int userLevel);
        //
        // Misc
        template<typename T>
        QJsonObject GetRootObject(T out);
        //
        // -------------------------- BEGIN CONFIG VALIDATIONS ---------------------------------------------
        int VerifyVMessProtocolString(QString vmess);
        // -------------------------- BEGIN CONFIG CONVERSIONS ---------------------------------------------
        // Save Connection Config
        int SaveConnectionConfig(QJsonObject obj, const QString *alias);
        // VMess Protocol
        QJsonObject ConvertOutboundFromVMessString(QString vmess);

    }
}
using namespace Qv2ray::ConfigOperations;
#endif // CONFIGGENERATION_H
