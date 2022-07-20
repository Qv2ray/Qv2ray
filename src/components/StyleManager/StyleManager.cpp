#include "StyleManager.hpp"

#include "MessageBus/MessageBus.hpp"
#include "Qv2rayApplication.hpp"
#include "Qv2rayBase/Common/Utils.hpp"

#include <QApplication>
#include <QColor>
#include <QFileInfo>
#include <QPalette>
#include <QStyle>
#include <QStyleFactory>
#include <iostream>

constexpr auto QV2RAY_BUILT_IN_DARK_MODE_NAME = "Built-in Darkmode";
constexpr auto QV2RAY_SYSTEM_STYLE = "System";

namespace Qv2ray::components::QvStyleManager
{
    QvStyleManager::QvStyleManager(QObject *parent) : QObject(parent)
    {
        ReloadStyles();
    }

    void QvStyleManager::ReloadStyles()
    {
        styles.clear();
        styles.insert(QV2RAY_BUILT_IN_DARK_MODE_NAME, {});
        styles.insert(QV2RAY_SYSTEM_STYLE, {});
        for (const auto &key : QStyleFactory::keys())
        {
            qInfo() << "Found factory style:" << key;
            QvStyle style;
            style.Name = key;
            style.Type = QvStyle::QVSTYLE_FACTORY;
            styles.insert(key, style);
        }

        for (const auto &styleDir : QvBaselib->GetAssetsPaths(u"uistyles"_qs))
        {
            for (const auto &fileInfo : QDir(styleDir).entryInfoList(QDir::Files))
            {
                if (!fileInfo.isFile())
                    break;
                if (fileInfo.suffix() == u"css"_qs || fileInfo.suffix() == u"qss"_qs || fileInfo.suffix() == u"qvstyle"_qs)
                {
                    qInfo() << "Found QSS style at:" << fileInfo;
                    QvStyle style;
                    style.Name = fileInfo.baseName();
                    style.qssPath = fileInfo.absoluteFilePath();
                    style.Type = QvStyle::QVSTYLE_QSS;
                    styles.insert(style.Name, style);
                }
            }
        }
    }

    bool QvStyleManager::ApplyStyle(const QString &style)
    {
        if (!styles.contains(style))
            return false;
        qApp->setStyle(u"fusion"_qs);
        if (style == QV2RAY_SYSTEM_STYLE)
        {
            qApp->setStyle(Qv2ray::Models::Qv2rayAppearanceConfig::_system_theme);
            return true;
        }

        if (style == QV2RAY_BUILT_IN_DARK_MODE_NAME)
        {
            qInfo() << "Applying built-in darkmode theme.";
            // From https://forum.qt.io/topic/101391/windows-10-dark-theme/4
            static const QColor darkColor(45, 45, 45);
            static const QColor disabledColor(70, 70, 70);
            static const QColor defaultTextColor(210, 210, 210);
            //
            QPalette darkPalette;
            darkPalette.setColor(QPalette::Window, darkColor);
            darkPalette.setColor(QPalette::Button, darkColor);
            darkPalette.setColor(QPalette::AlternateBase, darkColor);
            //
            darkPalette.setColor(QPalette::Text, defaultTextColor);
            darkPalette.setColor(QPalette::ButtonText, defaultTextColor);
            darkPalette.setColor(QPalette::WindowText, defaultTextColor);
            darkPalette.setColor(QPalette::ToolTipBase, defaultTextColor);
            darkPalette.setColor(QPalette::ToolTipText, defaultTextColor);
            //
            darkPalette.setColor(QPalette::Disabled, QPalette::Text, disabledColor);
            darkPalette.setColor(QPalette::Disabled, QPalette::WindowText, disabledColor);
            darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, disabledColor);
            darkPalette.setColor(QPalette::Disabled, QPalette::HighlightedText, disabledColor);
            //
            darkPalette.setColor(QPalette::Base, QColor(18, 18, 18));
            darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
            darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
            //
            darkPalette.setColor(QPalette::BrightText, Qt::red);
            darkPalette.setColor(QPalette::HighlightedText, Qt::black);
            qApp->setPalette(darkPalette);
            qApp->setStyleSheet(u"QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }"_qs);
            return true;
        }

        const auto s = styles.value(style);
        switch (s.Type)
        {
            case QvStyle::QVSTYLE_QSS:
            {
                qInfo() << "Applying UI QSS style:" << s.qssPath;
                const auto content = ReadFile(s.qssPath);
                qApp->setStyleSheet(content);
                break;
            }
            case QvStyle::QVSTYLE_FACTORY:
            {
                qInfo() << "Applying UI style:" << s.Name;
                const auto _style = QStyleFactory::create(s.Name);
                qApp->setPalette(_style->standardPalette());
                qApp->setStyle(_style);
                qApp->setStyleSheet(u""_qs);
                break;
            }
            default:
            {
                return false;
            }
        }

        UIMessageBus->EmitGlobalSignal(MessageBus::UPDATE_COLORSCHEME);
        qApp->processEvents();
        return true;
    }

    bool QvStyleManager::isDarkMode() const
    {
        return qApp->palette().color(QPalette::Window).lightness() < 110;
    }
} // namespace Qv2ray::components::QvStyleManager
