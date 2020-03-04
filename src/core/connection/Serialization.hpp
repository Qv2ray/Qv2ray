#include "base/Qv2rayBase.hpp"
#include "core/CoreSafeTypes.hpp"

namespace Qv2ray::core::connection
{
    namespace Serialization
    {
        // General
        const auto QV2RAY_SERIALIZATION_COMPLEX_CONFIG_PLACEHOLDER = QObject::tr("(Complex Connection Config)");
        QString DecodeSubscriptionString(QByteArray arr);
        CONFIGROOT ConvertConfigFromString(const QString &link, QString *alias, QString *errMessage);
        const QString ConvertConfigToString(const ConnectionId &id, bool isSip002 = false);
        //
        // VMess URI Protocol
        namespace vmess
        {
            CONFIGROOT ConvertConfigFromVMessString(const QString &vmess, QString *alias, QString *errMessage);
            const QString ConvertConfigToVMessString(const StreamSettingsObject &transfer, const VMessServerObject &server,
                                                     const QString &alias);
        } // namespace vmess
        //
        // SS URI Protocol
        namespace ss
        {
            CONFIGROOT ConvertConfigFromSSString(const QString &ss, QString *alias, QString *errMessage);
            const QString ConvertConfigToSSString(const ShadowSocksServerObject &server, const QString &alias, bool isSip002);
        } // namespace ss
        //
    } // namespace Serialization
} // namespace Qv2ray::core::connection

using namespace Qv2ray::core;
using namespace Qv2ray::core::connection;
using namespace Qv2ray::core::connection::Serialization;
using namespace Qv2ray::core::connection::Serialization::ss;
using namespace Qv2ray::core::connection::Serialization::vmess;
