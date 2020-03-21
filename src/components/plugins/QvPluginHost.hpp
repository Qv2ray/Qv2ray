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
} // namespace Qv2ray::components::plugins
