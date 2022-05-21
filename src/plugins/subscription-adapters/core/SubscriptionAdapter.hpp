#pragma once

#include "QvPlugin/PluginInterface.hpp"

using namespace Qv2rayPlugin;
using namespace Qv2rayPlugin::Common::EditorCreator;

class SIP008Decoder : public SubscriptionProvider
{
  public:
    SubscriptionResult DecodeSubscription(const QByteArray &data) const override;
};

class SimpleBase64Decoder : public SubscriptionProvider
{
  public:
    SubscriptionResult DecodeSubscription(const QByteArray &data) const override;
};

class OOCProvider : public SubscriptionProvider
{
  public:
    SubscriptionResult FetchDecodeSubscription(const SubscriptionProviderOptions &) const override;
};

class BuiltinSubscriptionAdapterInterface final : public IPluginSubscriptionInterface
{

  public:
    explicit BuiltinSubscriptionAdapterInterface() = default;

    const static inline Qv2rayPlugin::Common::EditorCreator::EditorInfoList oocv1_options{
#ifdef OOCv1_DETAIL_CONFIGURATION
        EditorInfo::Create<ElementType::Integer>("version", "OOC version"),
        EditorInfo::Create<ElementType::String>("baseUrl", "Base URL"),
        EditorInfo::Create<ElementType::String>("secret", "Secret"),
        EditorInfo::Create<ElementType::String>("userId", "User ID"),
        EditorInfo::Create<ElementType::String>("certSha256", "Certification SHA256"),
#else
        EditorInfo::Create<ElementType::String>("token", "Token/URL"),
#endif
    };

    QList<SubscriptionProviderInfo> GetInfo() const override
    {
        return {
            SubscriptionProviderInfo::CreateDecoder<SIP008Decoder>(SubscriptionProviderId{ "sip008" }, "SIP008"),
            SubscriptionProviderInfo::CreateDecoder<SimpleBase64Decoder>(SubscriptionProviderId{ "simple_base64" }, "Base64 Links"),
            SubscriptionProviderInfo::CreateFetcherDecoder<OOCProvider>(SubscriptionProviderId{ "ooc" }, "Open Online Config", oocv1_options),
        };
    }
};
