#pragma once
#include <QObject>

namespace Qv2ray::components::plugins
{
    class QvPluginHost : public QObject
    {
        Q_OBJECT
      public:
        explicit QvPluginHost(QObject *parent = nullptr);
        bool LoadPlugin();
    };

    inline QvPluginHost pluginHost;
} // namespace Qv2ray::components::plugins
