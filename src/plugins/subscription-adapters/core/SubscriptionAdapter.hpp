#pragma once
#include "CommonTypes.hpp"
#include "QvPluginProcessor.hpp"

#include <QRegularExpression>

using namespace Qv2rayPlugin;

const inline QStringList SplitLines(const QString &_string)
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    return _string.split(QRegularExpression("[\r\n]"), Qt::SplitBehaviorFlags::SkipEmptyParts);
#else
    return _string.split(QRegularExpression("[\r\n]"), QString::SkipEmptyParts);
#endif
}

class SimpleBase64Decoder : public Qv2rayPlugin::SubscriptionDecoder
{
  public:
    explicit SimpleBase64Decoder() : SubscriptionDecoder(){};
    SubscriptionDecodeResult DecodeData(const QByteArray &data) const override;
};

class SIP008Decoder : public Qv2rayPlugin::SubscriptionDecoder
{
  public:
    explicit SIP008Decoder() : SubscriptionDecoder(){};
    SubscriptionDecodeResult DecodeData(const QByteArray &data) const override;
};

class BuiltinSubscriptionAdapterInterface : public SubscriptionInterface
{
  public:
    explicit BuiltinSubscriptionAdapterInterface() : SubscriptionInterface()
    {
        simple_base64 = std::make_shared<SimpleBase64Decoder>();
        sip008 = std::make_shared<SIP008Decoder>();
    }

    QList<Qv2rayPlugin::ProtocolInfoObject> SupportedSubscriptionTypes() const override
    {
        // "simple_base64" = magic value in Qv2ray main application
        return {
            ProtocolInfoObject{ "sip008", "SIP008" },             //
            ProtocolInfoObject{ "simple_base64", "Basic Base64" } //
        };
    }

    std::shared_ptr<Qv2rayPlugin::SubscriptionDecoder> GetSubscriptionDecoder(const QString &type) const override
    {
        if (type == "simple_base64")
            return simple_base64;
        if (type == "sip008")
            return sip008;
        return nullptr;
    }

    std::shared_ptr<SubscriptionDecoder> simple_base64;
    std::shared_ptr<SubscriptionDecoder> sip008;
};
