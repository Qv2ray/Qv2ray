#pragma once

#include "CommonTypes.hpp"
#include "QvPluginInterface.hpp"

using namespace Qv2rayPlugin;

const inline QStringList SplitLines(const QString &_string)
{
    return _string.split(QRegularExpression("[\r\n]"), Qt::SkipEmptyParts);
}

class SimpleBase64Decoder : public SubscriptionDecoder
{
  public:
    explicit SimpleBase64Decoder() : SubscriptionDecoder(){};
    SubscriptionDecodeResult DecodeData(const QByteArray &data) const override;
};

class SIP008Decoder : public SubscriptionDecoder
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

    QList<SubscriptionInfoObject> SupportedSubscriptionTypes() const override
    {
        // "simple_base64" = magic value in Qv2ray main application
        return {
            SubscriptionInfoObject{ "sip008", "SIP008" },             //
            SubscriptionInfoObject{ "simple_base64", "Basic Base64" } //
        };
    }

    std::shared_ptr<SubscriptionDecoder> GetSubscriptionDecoder(const QString &type) const override
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
