#include "core/CoreUtils.hpp"
#include "core/connection/Generation.hpp"
#include "core/connection/Serialization.hpp"
#include "utils/QvHelpers.hpp"

#define QV_MODULE_NAME "ShadowsocksImporter"

namespace Qv2ray::core::connection
{
    namespace serialization::ss
    {
        CONFIGROOT Deserialize(const QString &ssUri, QString *alias, QString *errMessage)
        {
            ShadowSocksServerObject server;
            QString d_name;

            // auto ssUri = _ssUri.toStdString();
            if (ssUri.length() < 5)
            {
                LOG("ss:// string too short");
                *errMessage = QObject::tr("SS URI is too short");
            }

            auto uri = ssUri.mid(5);
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
                // Old URI scheme
                QString decoded = QByteArray::fromBase64(uri.toUtf8(), QByteArray::Base64Option::OmitTrailingEquals);
                auto colonPos = decoded.indexOf(':');
                DEBUG("Colon position: " + QSTRN(colonPos));

                if (colonPos < 0)
                {
                    *errMessage = QObject::tr("Can't find the colon separator between method and password");
                }

                server.method = decoded.left(colonPos);
                decoded.remove(0, colonPos + 1);
                atPos = decoded.lastIndexOf('@');
                DEBUG("At sign position: " + QSTRN(atPos));

                if (atPos < 0)
                {
                    *errMessage = QObject::tr("Can't find the at separator between password and hostname");
                }

                server.password = decoded.mid(0, atPos);
                decoded.remove(0, atPos + 1);
                colonPos = decoded.lastIndexOf(':');
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
                // SIP002 URI scheme
                auto x = QUrl::fromUserInput(uri);
                server.address = x.host();
                server.port = x.port();
                const auto userInfo = SafeBase64Decode(x.userName());
                const auto userInfoSp = userInfo.indexOf(':');
                //
                DEBUG("Userinfo splitter position: " + QSTRN(userInfoSp));

                if (userInfoSp < 0)
                {
                    *errMessage = QObject::tr("Can't find the colon separator between method and password");
                    return CONFIGROOT{};
                }

                const auto method = userInfo.mid(0, userInfoSp);
                server.method = method;
                server.password = userInfo.mid(userInfoSp + 1);
            }

            d_name = QUrl::fromPercentEncoding(d_name.toUtf8());
            CONFIGROOT root;
            OUTBOUNDS outbounds;
            outbounds.append(GenerateOutboundEntry(OUTBOUND_TAG_PROXY, "shadowsocks", GenerateShadowSocksOUT({ server }), {}));
            JADD(outbounds)
            *alias = alias->isEmpty() ? d_name : *alias + "_" + d_name;
            LOG("Deduced alias: " + *alias);
            return root;
        }

        const QString Serialize(const ShadowSocksServerObject &server, const QString &alias, bool)
        {
            QUrl url;
            const auto plainUserInfo = server.method + ":" + server.password;
            const auto userinfo = plainUserInfo.toUtf8().toBase64(QByteArray::Base64UrlEncoding | QByteArray::OmitTrailingEquals);
            url.setUserInfo(userinfo);
            url.setScheme("ss");
            url.setHost(server.address);
            url.setPort(server.port);
            url.setFragment(alias);
            return url.toString(QUrl::ComponentFormattingOption::FullyEncoded);
        }
    } // namespace serialization::ss
} // namespace Qv2ray::core::connection
