#include "base/Qv2rayBase.hpp"

namespace Qv2ray::core::connection
{
    namespace Serialization
    {
        //int VerifyVMessProtocolString(QString vmess);
        QString DecodeSubscriptionString(QByteArray arr);

        // VMess URI Protocol
        CONFIGROOT ConvertConfigFromVMessString(const QString &vmess, QString *alias, QString *errMessage);
        QString ConvertConfigToVMessString(const StreamSettingsObject &transfer, const VMessServerObject &serverConfig, const QString &alias);

        // SS URI Protocol
        CONFIGROOT ConvertConfigFromSSString(const QString &ss, QString *alias, QString *errMessage);
        QString ConvertConfigToSSString(const ShadowSocksServerObject &serverConfig, const QString &alias);
    }
}

using namespace Qv2ray::core;
using namespace Qv2ray::core::connection;
using namespace Qv2ray::core::connection::Serialization;
