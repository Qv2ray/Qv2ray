#pragma once

#include <QMap>
#include <QObject>
#include <QStringList>

namespace Qv2ray::components::QvStyleManager
{
    struct QvStyle
    {
        enum StyleType
        {
            QVSTYLE_FACTORY,
            QVSTYLE_QSS
        } Type;
        QString Name;
        QString qssPath;
    };

    class QvStyleManager : QObject
    {
      public:
        QvStyleManager(QObject *parent = nullptr);
        inline QStringList AllStyles() const
        {
            return styles.keys();
        }
        bool ApplyStyle(const QString &);
        bool isDarkMode() const;

      private:
        void ReloadStyles();
        QMap<QString, QvStyle> styles;
    };

} // namespace Qv2ray::components::StyleManager

inline Qv2ray::components::QvStyleManager::QvStyleManager *StyleManager = nullptr;
