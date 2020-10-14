#include "StyleManager.hpp"

#include "base/Qv2rayBase.hpp"
#include "ui/widgets/common/WidgetUIBase.hpp"
#include "utils/QvHelpers.hpp"

#include <QApplication>
#include <QColor>
#include <QPalette>
#include <QStyle>
#include <QStyleFactory>

constexpr auto QV2RAY_BUILT_IN_DARK_MODE_NAME = "Built-in Darkmode";
#define QV_MODULE_NAME "StyleManager"

namespace Qv2ray::ui::styles
{
    QvStyleManager::QvStyleManager(QObject *parent) : QObject(parent)
    {
        ReloadStyles();
    }

    void QvStyleManager::ReloadStyles()
    {
        styles.clear();
        styles.insert(QV2RAY_BUILT_IN_DARK_MODE_NAME, {});
        for (const auto &key : QStyleFactory::keys())
        {
            LOG("Found factory style: " + key);
            QvStyle style;
            style.Name = key;
            style.Type = QvStyle::QVSTYLE_FACTORY;
            styles.insert(key, style);
        }

        for (const auto &styleDir : QvCoreApplication->GetAssetsPaths("uistyles"))
        {
            for (const auto &file : GetFileList(QDir(styleDir)))
            {
                QFileInfo fileInfo(styleDir + "/" + file);
                if (fileInfo.suffix() == "css" || fileInfo.suffix() == "qss" || fileInfo.suffix() == "qvstyle")
                {
                    LOG("Found QSS style at: \"" + fileInfo.absoluteFilePath() + "\"");
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
        qApp->setStyle("fusion");
        if (style == QV2RAY_BUILT_IN_DARK_MODE_NAME)
        {
            LOG("Applying built-in darkmode theme.");
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
            qApp->setStyleSheet("QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }");
            return true;
        }

        const auto &s = styles[style];
        switch (s.Type)
        {
            case QvStyle::QVSTYLE_QSS:
            {
                LOG("Applying UI QSS style: " + s.qssPath);
                const auto content = StringFromFile(s.qssPath);
                qApp->setStyleSheet(content);
                break;
            }
            case QvStyle::QVSTYLE_FACTORY:
            {
                LOG("Applying UI style: " + s.Name);
                const auto &_style = QStyleFactory::create(s.Name);
                qApp->setPalette(_style->standardPalette());
                qApp->setStyle(_style);
                qApp->setStyleSheet("");
                break;
            }
            default:
            {
                return false;
            }
        }
        qApp->processEvents();
        return true;
    }
} // namespace Qv2ray::ui::styles
