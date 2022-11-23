#include "core/CoreUtils.hpp"
#include "core/connection/Generation.hpp"
#include "core/connection/Serialization.hpp"
#include "utils/QvHelpers.hpp"

#define QV_MODULE_NAME "TrojanImporter"

namespace Qv2ray::core::connection
{
    namespace serialization::trojan
    {
        CONFIGROOT Deserialize(const QString &trojanUri, QString *alias, QString *errMessage)
        {
            TrojanServerObject server;
            QString d_name;

            if (trojanUri.length() < 9)
            {
                LOG("trojan:// string too short");
                *errMessage = QObject::tr("SS URI is too short");
            }

            auto url = QUrl::fromUserInput(trojanUri);
            if (url.scheme() != "trojan")
            {
                LOG("not a trojan share link");
                *errMessage = QObject::tr("Not a Trojan share link!");
            }
            server.address = url.host();
            server.port = url.port();
            server.password = url.userInfo();
            if (url.hasFragment())
            {
                d_name = url.fragment(QUrl::FullyDecoded);
            }

            CONFIGROOT root;
            OUTBOUNDS outbounds;
            outbounds.append(GenerateOutboundEntry(OUTBOUND_TAG_PROXY, "trojan", GenerateTrojanOUT({ server }), {}));
            JADD(outbounds)
            *alias = alias->isEmpty() ? d_name : *alias + "_" + d_name;
            LOG("Deduced alias: " + *alias);
            return root;
        }

        const QString Serialize(const TrojanServerObject &server, const QString &alias)
        {
            QUrl url;
            url.setUserInfo(server.password);
            url.setScheme("trojan");
            url.setHost(server.address);
            url.setPort(server.port);
            url.setFragment(alias);
            return url.toString(QUrl::ComponentFormattingOption::FullyEncoded);
        }
    } // namespace serialization::trojan
} // namespace Qv2ray::core::connection
