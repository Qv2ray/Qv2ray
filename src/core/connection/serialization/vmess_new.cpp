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
            Q_UNUSED(vmessStr)
            Q_UNUSED(alias)
            Q_UNUSED(errMessage)
#define default CONFIGROOT()
            LOG(MODULE_CONNECTION, "咕咕")
            return default;
#if 0
            QUrl url{ vmessStr };
            QUrlQuery query{ url };
            //
            if (!url.isValid())
            {
                *errMessage = QObject::tr("vmess:// url is invalid");
                return default;
            }

            QString net;
            bool tls;

            for (const auto &_protocol : url.userName().split("+"))
            {
                tls = tls || _protocol == "tls";
                net = _protocol == "tls" ? net : _protocol;
            }

            if (!QStringList{ "tcp", "http", "ws", "kcp", "quic" }.contains(net))
            {
                *errMessage = QObject::tr("Invalid streamSettings protocol:  ") + net;
                return default;
            }

            QString uuid;
            int aid;
            {
                const auto pswd = url.password();
                const auto index = pswd.lastIndexOf("-");
                uuid = pswd.mid(0, index);
                aid = pswd.right(pswd.length() - index - 1).toInt();
            }
            const auto host = url.host();
            int port = url.port();
#endif
#undef default
        }

        const QString Serialize(const StreamSettingsObject &transfer, const VMessServerObject &server, const QString &alias)
        {
            Q_UNUSED(transfer)
            Q_UNUSED(server)
            Q_UNUSED(alias)
            LOG(MODULE_CONNECTION, "咕咕")
            return "";
        }
    } // namespace serialization::vmess_new
} // namespace Qv2ray::core::connection
