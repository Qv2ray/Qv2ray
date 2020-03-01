#include "base/Qv2rayBase.hpp"
#include "core/CoreSafeTypes.hpp"

namespace Qv2ray::core::connection
{
    namespace Serialization
    {
        // int VerifyVMessProtocolString(QString vmess);
        QString DecodeSubscriptionString(QByteArray arr);

        // General
        CONFIGROOT ConvertConfigFromString(const QString &link, QString *alias, QString *errMessage);
        const QString ConvertConfigToString(const ConnectionId &id, bool isSip002 = false);
        const QString ConvertConfigToString(const QString &alias, const CONFIGROOT &server, bool isSip002);

        // VMess URI Protocol
        CONFIGROOT ConvertConfigFromVMessString(const QString &vmess, QString *alias, QString *errMessage);
        QString ConvertConfigToVMessString(const StreamSettingsObject &transfer, const VMessServerObject &serverConfig, const QString &alias);

        // SS URI Protocol
        CONFIGROOT ConvertConfigFromSSString(const QString &ss, QString *alias, QString *errMessage);
        QString ConvertConfigToSSString(const ShadowSocksServerObject &server, const QString &alias, bool isSip002);
    } // namespace Serialization
} // namespace Qv2ray::core::connection

using namespace Qv2ray::core;
using namespace Qv2ray::core::connection;
using namespace Qv2ray::core::connection::Serialization;
