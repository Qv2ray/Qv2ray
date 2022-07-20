#include "SubscriptionAdapter.hpp"

#include "BuiltinSubscriptionAdapter.hpp"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QSslKey>
#include <QUrl>
#include <QUrlQuery>

const inline QStringList SplitLines(const QString &_string)
{
    return _string.split(QRegularExpression(u"[\r\n]"_qs), Qt::SkipEmptyParts);
}

static QString SafeBase64Decode(QString string)
{
    QByteArray ba = string.replace(QChar('-'), QChar('+')).replace(QChar('_'), QChar('/')).toUtf8();
    return QByteArray::fromBase64(ba, QByteArray::Base64Option::OmitTrailingEquals);
}

QString SafeBase64Encode(const QString &string)
{
    QString base64 = string.toUtf8().toBase64();
    return base64.replace(QChar('+'), QChar('-')).replace(QChar('/'), QChar('_'));
}

// Simple Base64 Decoder
SubscriptionResult SimpleBase64Decoder::DecodeSubscription(const QByteArray &data) const
{
    const auto source = QString::fromUtf8(data).trimmed();
    const auto resultList = source.contains(u"://"_qs) ? source : SafeBase64Decode(source);

    SubscriptionResult result;
    result.SetValue<SR_Links>(SplitLines(resultList));
    return result;
}

// SIP008 Decoder
SubscriptionResult SIP008Decoder::DecodeSubscription(const QByteArray &data) const
{
    const auto root = QJsonDocument::fromJson(data).object();
    //
    // const auto version = root["version"].toString();
    // const auto username = root["username"].toString();
    // const auto user_uuid = root["user_uuid"].toString();
    const auto servers = root[u"servers"_qs].toArray();

    // ss://Y2hhY2hhMjAtaWV0Zi1wb2x5MTMwNTpwYXNzQGhvc3Q6MTIzNA/?plugin=plugin%3Bopt#sssip003

    SubscriptionResult result;
    SubscriptionResult::result_type_t<SR_Tags> tags;
    SubscriptionResult::result_type_t<SR_OutboundObjects> outbounds;
    for (const auto &servVal : servers)
    {
        const auto serverObj = servVal.toObject();
#define GetVal(x) const auto x = serverObj[u## #x##_qs].toString()
        GetVal(server);
        GetVal(password);
        GetVal(method);
        GetVal(plugin);
        GetVal(remarks);
        // GetVal(plugin_opts);
        // GetVal(id);
#undef GetVal

        // id, group, owner omitted.
        if (!plugin.isEmpty())
        {
            // SIP003 plugins not supported.
            qDebug() << "Unsupported node:" << remarks;
            continue;
        }

        tags.insert(remarks, serverObj[u"tags"_qs].toVariant().toStringList());

        IOProtocolSettings protocolSettings;
        protocolSettings.insert(u"method"_qs, method);
        protocolSettings.insert(u"password"_qs, password);

        IOConnectionSettings connectionSettings;
        connectionSettings.address = server;
        connectionSettings.port = serverObj[u"server_port"_qs].toInt();
        connectionSettings.protocol = u"shadowsocks"_qs;
        connectionSettings.protocolSettings = protocolSettings;

        outbounds.insert(remarks, { connectionSettings });
    }
    result.SetValue<SR_Tags>(tags);
    result.SetValue<SR_OutboundObjects>(outbounds);
    return result;
}

// OOCv1 Decoder
SubscriptionResult OOCProvider::FetchDecodeSubscription(const SubscriptionProviderOptions &options) const
{
#ifdef OOCv1_DETAIL_CONFIGURATION
    const auto url = u"%1/%2/ooc/v%3/%4"_qs.arg(options.value(u"baseUrl"_qs).toString(),               //
                                                options.value(u"secret"_qs).toString(),                //
                                                QString::number(options.value(u"version"_qs).toInt()), //
                                                options.value(u"userId"_qs).toString());
#else
    QString option = options.value(u"token"_qs).toString();
    QUrl url;
    if (QUrl(option).isValid())
    {
        url = QUrl{ option };
    }
    else
    {
        const auto json = QJsonDocument::fromJson(options.value(u"token"_qs).toString().toUtf8()).object();
        url = u"%1/%2/ooc/v%3/%4"_qs.arg(json.value(u"baseUrl"_qs).toString(),               //
                                         json.value(u"secret"_qs).toString(),                //
                                         QString::number(json.value(u"version"_qs).toInt()), //
                                         json.value(u"userId"_qs).toString());
    }
#endif

    const auto pinnedCertChecker = [](QNetworkReply *reply)
    {
        QSslCertificate cert = reply->sslConfiguration().peerCertificate();
        QString serverHash = QCryptographicHash::hash(cert.publicKey().toDer(), QCryptographicHash::Sha256).toHex();
        qWarning() << "Pinned certificate not checked, not implemented";
        Q_UNUSED(serverHash);
    };

    const auto &[err, errorString, data] = InternalSubscriptionSupportPlugin::NetworkRequestHelper()->Get(url, pinnedCertChecker);

    if (err != QNetworkReply::NoError)
    {
        qCritical().noquote() << errorString;
        InternalSubscriptionSupportPlugin::ShowMessageBox(QObject::tr("Cannot Contact OOC API Server"), errorString);
        return {};
    }

    const auto shadowsocks = QJsonDocument::fromJson(data).object().value(u"shadowsocks"_qs).toArray();

    SubscriptionResult result;
    SubscriptionResult::result_type_t<SR_Tags> tags;
    SubscriptionResult::result_type_t<SR_OutboundObjects> outbounds;

    QStringList unsupportedNodes;
    for (const auto &ss : shadowsocks)
    {
        // id, group, owner omitted.
        const auto name = ss[u"name"_qs].toString();
        if (ss.toObject().contains(u"pluginName"_qs))
        {
            // SIP003 plugins not supported.
            qWarning() << "Unsupported node:" << name;
            unsupportedNodes << name;
            continue;
        }

        tags.insert(name, ss[u"tags"_qs].toVariant().toStringList());

        IOProtocolSettings protocolSettings;
        protocolSettings.insert(u"method"_qs, ss[u"method"_qs].toString());
        protocolSettings.insert(u"password"_qs, ss[u"password"_qs].toString());

        IOConnectionSettings connectionSettings;
        connectionSettings.address = ss[u"address"_qs].toString();
        connectionSettings.port = ss[u"port"_qs].toInt();
        connectionSettings.protocol = u"shadowsocks"_qs;
        connectionSettings.protocolSettings = protocolSettings;

        outbounds.insert(name, { connectionSettings });
    }

    if (!unsupportedNodes.isEmpty())
        InternalSubscriptionSupportPlugin::ShowMessageBox(QObject::tr("Subscription contains unsupported nodes"), unsupportedNodes.join('\n'));

    result.SetValue<SR_Tags>(tags);
    result.SetValue<SR_OutboundObjects>(outbounds);
    return result;
}
