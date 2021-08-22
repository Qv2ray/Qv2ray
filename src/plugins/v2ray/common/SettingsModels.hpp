#pragma once
#include "QvPlugin/Utils/BindableProps.hpp"
#include "QvPlugin/Utils/JsonConversion.hpp"

#ifndef QV2RAY_DEFAULT_VCORE_PATH
#define QV2RAY_DEFAULT_VCORE_PATH ""
#endif

#ifndef QV2RAY_DEFAULT_VASSETS_PATH
#define QV2RAY_DEFAULT_VASSETS_PATH ""
#endif

struct BrowserForwarderConfig
{
    Bindable<QString> listenAddr;
    Bindable<int> listenPort{ 18888 };
    QJS_JSON(P(listenAddr, listenPort))
};

struct ObservatoryConfig
{
    QStringList subjectSelector;
    QJS_JSON(F(subjectSelector))
};

struct V2RayCorePluginSettings
{
    enum V2RayLogLevel
    {
        None,
        Error,
        Warning,
        Info,
        Debug
    };

    Bindable<V2RayLogLevel> LogLevel{ Warning };

    Bindable<QString> CorePath{ QString::fromUtf8(QV2RAY_DEFAULT_VCORE_PATH) };
    Bindable<QString> AssetsPath{ QString::fromUtf8(QV2RAY_DEFAULT_VASSETS_PATH) };

    Bindable<int> OutboundMark{ 255 };

    Bindable<bool> APIEnabled{ true };
    Bindable<int> APIPort{ 15480 };

    BrowserForwarderConfig BrowserForwarderSettings;
    ObservatoryConfig ObservatorySettings;

    QJS_JSON(P(LogLevel, CorePath, AssetsPath, APIEnabled, APIPort, OutboundMark), F(BrowserForwarderSettings, ObservatorySettings))
};
