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

            // auto ssUri = _ssUri.toStdString();
            if (trojanUri.length() < 5)
            {
                LOG("trojan:// string too short");
                *errMessage = QObject::tr("SS URI is too short");
            }

            auto uri = trojanUri.mid(5);
            auto hashPos = uri.lastIndexOf("#");
            DEBUG("Hash sign position: " + QSTRN(hashPos));

            if (hashPos >= 0)
            {
                // Get the name/remark
                d_name = uri.mid(uri.lastIndexOf("#") + 1);
                uri.truncate(hashPos);
            }

            auto atPos = uri.indexOf('@');
            DEBUG("At sign position: " + QSTRN(atPos));

            if (atPos < 0)
            {
                QString decoded = QByteArray::fromBase64(uri.toUtf8(), QByteArray::Base64Option::OmitTrailingEquals);
                atPos = decoded.lastIndexOf('@');
                DEBUG("At sign position: " + QSTRN(atPos));

                if (atPos < 0)
                {
                    *errMessage = QObject::tr("Can't find the at separator between password and hostname");
                }

                server.password = decoded.mid(0, atPos);
                decoded.remove(0, atPos + 1);
                auto colonPos = decoded.lastIndexOf(':');
                DEBUG("Colon position: " + QSTRN(colonPos));

                if (colonPos < 0)
                {
                    *errMessage = QObject::tr("Can't find the colon separator between hostname and port");
                }

                server.address = decoded.mid(0, colonPos);
                server.port = decoded.mid(colonPos + 1).toInt();
            }
            else
            {
                auto x = QUrl::fromUserInput(uri);
                server.address = x.host();
                server.port = x.port();
                server.password = x.userName();
            }

            d_name = QUrl::fromPercentEncoding(d_name.toUtf8());
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
            const auto userinfo = server.password;
            url.setUserInfo(userinfo);
            url.setScheme("trojan");
            url.setHost(server.address);
            url.setPort(server.port);
            url.setFragment(alias);
            return url.toString(QUrl::ComponentFormattingOption::FullyEncoded);
        }
    } // namespace serialization::trojan
} // namespace Qv2ray::core::connection

