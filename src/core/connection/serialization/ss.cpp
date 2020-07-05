#include "common/QvHelpers.hpp"
#include "core/CoreUtils.hpp"
#include "core/connection/Generation.hpp"
#include "core/connection/Serialization.hpp"

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
                LOG(MODULE_CONNECTION, "ss:// string too short")
                *errMessage = QObject::tr("SS URI is too short");
            }

            auto uri = ssUri.mid(5);
            auto hashPos = uri.lastIndexOf("#");
            DEBUG(MODULE_CONNECTION, "Hash sign position: " + QSTRN(hashPos))

            if (hashPos >= 0)
            {
                // Get the name/remark
                d_name = uri.mid(uri.lastIndexOf("#") + 1);
                uri.truncate(hashPos);
            }

            // No plugins for Qv2ray so disable those lnes.i
            // size_t pluginPos = uri.find_first_of('/');
            //
            // if (pluginPos != std::string::npos) {
            //    // TODO: support plugins. For now, just ignore them
            //    uri.erase(pluginPos);
            //}
            auto atPos = uri.indexOf('@');
            DEBUG(MODULE_CONNECTION, "At sign position: " + QSTRN(atPos))

            if (atPos < 0)
            {
                // Old URI scheme
                QString decoded = QByteArray::fromBase64(uri.toUtf8(), QByteArray::Base64Option::OmitTrailingEquals);
                auto colonPos = decoded.indexOf(':');
                DEBUG(MODULE_CONNECTION, "Colon position: " + QSTRN(colonPos))

                if (colonPos < 0)
                {
                    *errMessage = QObject::tr("Can't find the colon separator between method and password");
                }

                server.method = decoded.left(colonPos);
                decoded.remove(0, colonPos + 1);
                atPos = decoded.lastIndexOf('@');
                DEBUG(MODULE_CONNECTION, "At sign position: " + QSTRN(atPos))

                if (atPos < 0)
                {
                    *errMessage = QObject::tr("Can't find the at separator between password and hostname");
                }

                server.password = decoded.mid(0, atPos);
                decoded.remove(0, atPos + 1);
                colonPos = decoded.lastIndexOf(':');
                DEBUG(MODULE_CONNECTION, "Colon position: " + QSTRN(colonPos))

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
                DEBUG(MODULE_CONNECTION, "Userinfo splitter position: " + QSTRN(userInfoSp))

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
            outbounds.append(GenerateOutboundEntry("shadowsocks", GenerateShadowSocksOUT({ server }), {}));
            JADD(outbounds)
            *alias = alias->isEmpty() ? d_name : *alias + "_" + d_name;
            LOG(MODULE_CONNECTION, "Deduced alias: " + *alias)
            return root;
        }

        const QString Serialize(const ShadowSocksServerObject &server, const QString &alias, bool isSip002)
        {
            auto myAlias = QUrl::toPercentEncoding(alias);

            if (isSip002)
            {
                LOG(MODULE_CONNECTION, "Converting an ss-server config to Sip002 ss:// format")
                const auto plainUserInfo = server.method + ":" + server.password;
                const auto userinfo = plainUserInfo.toUtf8().toBase64(QByteArray::Base64UrlEncoding | QByteArray::OmitTrailingEquals);
                return "ss://" + userinfo + "@" + server.address + ":" + QSTRN(server.port) + "/#" + myAlias;
            }
            else
            {
                LOG(MODULE_CONNECTION, "Converting an ss-server config to old ss:// string format")
                QString ssUri = server.method + ":" + server.password + "@" + server.address + ":" + QSTRN(server.port);
                return "ss://" + ssUri.toUtf8().toBase64(QByteArray::Base64Option::OmitTrailingEquals) + "/#" + myAlias;
            }
        }
    } // namespace serialization::ss
} // namespace Qv2ray::core::connection
