#pragma once
#include "base/Qv2rayBase.hpp"
#include "core/CoreSafeTypes.hpp"

namespace Qv2ray::core::connection
{
    namespace Serialization
    {
        /**
         * pattern for the nodes in ssd links.
         * %1: airport name
         * %2: node name
         * %3: rate
         */
        inline auto QV2RAY_SSD_DEFAULT_NAME_PATTERN = QObject::tr("%1 - %2 (rate %3)");
        //
        // General
        QString DecodeSubscriptionString(const QByteArray &arr);
        QMultiHash<QString, CONFIGROOT> ConvertConfigFromString(const QString &link, QString *aliasPrefix, QString *errMessage,
                                                                QString *newGroupName = nullptr);
        const QString ConvertConfigToString(const ConnectionId &id, bool isSip002 = false);
        const QString ConvertConfigToString(const QString &alias, const QString &groupName, const CONFIGROOT &server, bool isSip002);
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
        // SSD URI Protocol
        namespace ssd
        {
            /**
             * @brief decodeSSD
             * @param uri     the uri of ssd link.
             * @param pattern the pattern for node names.
             * @return tuple of:
             *          - tuple of:
             *             - airport name (for the sake of grouping)
             *             - shadowsocks configuration list
             *          - log list
             *         in case of error, no objects will be returned.
             */
            QMultiHash<QString, CONFIGROOT> ConvertConfigFromSSDString(const QString &uri, QString *groupName, QStringList *logList);
        } // namespace ssd
        //
    } // namespace Serialization
} // namespace Qv2ray::core::connection

using namespace Qv2ray::core;
using namespace Qv2ray::core::connection;
using namespace Qv2ray::core::connection::Serialization;
using namespace Qv2ray::core::connection::Serialization::ss;
using namespace Qv2ray::core::connection::Serialization::ssd;
using namespace Qv2ray::core::connection::Serialization::vmess;
