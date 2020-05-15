#pragma once
#include "base/Qv2rayBase.hpp"

namespace Qv2ray::core::connection
{
    namespace Serialization
    {
        const inline auto QV2RAY_SERIALIZATION_COMPLEX_CONFIG_PLACEHOLDER = QObject::tr("(Complex config)");
        /**
         * pattern for the nodes in ssd links.
         * %1: airport name
         * %2: node name
         * %3: rate
         */
        const inline auto QV2RAY_SSD_DEFAULT_NAME_PATTERN = QObject::tr("%1 - %2 (rate %3)");
        //
        // General
        QString DecodeSubscriptionString(const QByteArray &arr);
        QMultiHash<QString, CONFIGROOT> ConvertConfigFromString(const QString &link, QString *aliasPrefix, QString *errMessage,
                                                                QString *newGroupName = nullptr);
        const QString ConvertConfigToString(const ConnectionGroupPair &id, bool isSip002 = false);
        const QString ConvertConfigToString(const QString &alias, const QString &groupName, const CONFIGROOT &server, bool isSip002);
        //
        // VMess URI Protocol
        namespace vmess
        {
            CONFIGROOT Deserialize(const QString &vmess, QString *alias, QString *errMessage);
            const QString Serialize(const StreamSettingsObject &transfer, const VMessServerObject &server, const QString &alias);
        } // namespace vmess
        //
        // SS URI Protocol
        namespace ss
        {
            CONFIGROOT Deserialize(const QString &ss, QString *alias, QString *errMessage);
            const QString Serialize(const ShadowSocksServerObject &server, const QString &alias, bool isSip002);
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
            QMultiHash<QString, CONFIGROOT> Deserialize(const QString &uri, QString *groupName, QStringList *logList);
        } // namespace ssd
        //
    } // namespace Serialization
} // namespace Qv2ray::core::connection

using namespace Qv2ray::core;
using namespace Qv2ray::core::connection;
using namespace Qv2ray::core::connection::Serialization;
