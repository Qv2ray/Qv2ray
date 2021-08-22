#include "InternalPlugin.hpp"

#include "InternalProfilePreprocessor.hpp"

const Qv2rayPlugin::QvPluginMetadata Qv2rayInternalPlugin::GetMetadata() const
{
    return Qv2rayPlugin::QvPluginMetadata{
        u"Qv2ray Internal Plugin"_qs,                                                            //
        u"Moody"_qs,                                                                             //
        PluginId{ u"qvinternal"_qs },                                                            //
        u"Apply your globally configured inbounds and routing rules for simple connections."_qs, //
        u""_qs,                                                                                  //
        { Qv2rayPlugin::COMPONENT_PROFILE_PREPROCESSOR }                                         //
    };
}

bool Qv2rayInternalPlugin::InitializePlugin()
{
    m_ProfilePreprocessor = std::make_shared<InternalProfilePreprocessor>();
    return true;
}

void Qv2rayInternalPlugin::SettingsUpdated()
{
    // No settings related.
}
