#pragma once
#include "base/Qv2rayBase.hpp"
#include "common/QvHelpers.hpp"

namespace Qv2ray::core::connection::serialization
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
    inline const QString TryDecodeSubscriptionString(const QByteArray &arr)
    {
        auto result = QString::fromUtf8(arr).trimmed();
        return result.contains("://") ? result : SafeBase64Decode(result);
    }
    QList<QPair<QString, CONFIGROOT>> ConvertConfigFromString(const QString &link, QString *aliasPrefix, QString *errMessage,
                                                              QString *newGroupName = nullptr);
    const QString ConvertConfigToString(const ConnectionGroupPair &id, bool isSip002 = true);
    const QString ConvertConfigToString(const QString &alias, const QString &groupName, const CONFIGROOT &server, bool isSip002 = true);

    namespace vmess
    {
        CONFIGROOT Deserialize(const QString &vmess, QString *alias, QString *errMessage);
        const QString Serialize(const StreamSettingsObject &transfer, const VMessServerObject &server, const QString &alias);
    } // namespace vmess

    namespace vmess_new
    {
        CONFIGROOT Deserialize(const QString &vmess, QString *alias, QString *errMessage);
        const QString Serialize(const StreamSettingsObject &transfer, const VMessServerObject &server, const QString &alias);
    } // namespace vmess_new

    namespace ss
    {
        CONFIGROOT Deserialize(const QString &ss, QString *alias, QString *errMessage);
        const QString Serialize(const ShadowSocksServerObject &server, const QString &alias, bool isSip002);
    } // namespace ss

    namespace ssd
    {
        QList<QPair<QString, CONFIGROOT>> Deserialize(const QString &uri, QString *groupName, QStringList *logList);
    } // namespace ssd

} // namespace Qv2ray::core::connection::serialization

using namespace Qv2ray::core;
using namespace Qv2ray::core::connection;
using namespace Qv2ray::core::connection::serialization;
