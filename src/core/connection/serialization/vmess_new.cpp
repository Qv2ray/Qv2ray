#include "common/QvHelpers.hpp"
#include "core/CoreUtils.hpp"
#include "core/connection/Generation.hpp"
#include "core/connection/Serialization.hpp"

#include <QUrl>
#include <QUrlQuery>

namespace Qv2ray::core::connection
{
    namespace serialization::vmess_new
    {
        CONFIGROOT Deserialize(const QString &vmessStr, QString *alias, QString *errMessage)
        {
            return CONFIGROOT{};
        }

        const QString Serialize(const StreamSettingsObject &transfer, const VMessServerObject &server, const QString &alias)
        {
            return {};
        }
    } // namespace serialization::vmess_new
} // namespace Qv2ray::core::connection
