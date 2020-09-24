#pragma once
#include "CommonTypes.hpp"
#include "QvPluginProcessor.hpp"

const inline QStringList SplitLines(const QString &_string)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return _string.split(QRegularExpression("[\r\n]"), Qt::SkipEmptyParts);
#elif QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    return _string.split(QRegExp("[\r\n]"), Qt::SkipEmptyParts);
#else
    return _string.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);
#endif
}

class BuiltinSerializer : public Qv2rayPlugin::SubscriptionDecoder
{
  public:
    explicit BuiltinSerializer() : Qv2rayPlugin::SubscriptionDecoder(){};
    SubscriptionDecodeResult DecodeData(const QByteArray &data) const override
    {
        const static auto SafeBase64Decode = [](QString string) -> QString {
            QByteArray ba = string.replace(QChar('-'), QChar('+')).replace(QChar('_'), QChar('/')).toUtf8();
            return QByteArray::fromBase64(ba, QByteArray::Base64Option::OmitTrailingEquals);
        };

        auto source = QString::fromUtf8(data).trimmed();
        const auto resultList = source.contains("://") ? source : SafeBase64Decode(source);
        //
        SubscriptionDecodeResult result;
        result.links = SplitLines(resultList);
        return result;
    }
};

class BuiltinSubscriptionAdapterInterface : public Qv2rayPlugin::SubscriptionInterface
{
  public:
    explicit BuiltinSubscriptionAdapterInterface() : Qv2rayPlugin::SubscriptionInterface()
    {
        simple_base64 = std::make_shared<BuiltinSerializer>();
    }
    QList<Qv2rayPlugin::ProtocolInfoObject> SupportedSubscriptionTypes() const override
    {
        // "simple_base64" = magic value in Qv2ray main application
        return { Qv2rayPlugin::ProtocolInfoObject{ "simple_base64", "Basic Base64" } };
    }
    std::shared_ptr<Qv2rayPlugin::SubscriptionDecoder> GetSubscriptionDecoder(const QString &type) const override
    {
        if (type == "simple_base64")
            return simple_base64;
        return nullptr;
    }
    std::shared_ptr<Qv2rayPlugin::SubscriptionDecoder> simple_base64;
};
