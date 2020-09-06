#pragma once

#include <QApplication>
#include <QDialog>
#include <QGraphicsEffect>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QTextCursor>
#include <QTextDocument>
#include <QtGui>

#define QV2RAY_USE_NEW_ICONS

constexpr auto QV2RAY_ICON_EXTENSION = ".svg";

#define QV2RAY_COLORSCHEME_ROOT_X(isDark) (QString(":/assets/icons/") + ((isDark) ? "ui_dark/" : "ui_light/"))
#define QV2RAY_COLORSCHEME_ROOT QV2RAY_COLORSCHEME_ROOT_X(GlobalConfig.uiConfig.useDarkTheme)
#define QV2RAY_ICON_RESOURCE(file) (QV2RAY_COLORSCHEME_ROOT + file + QV2RAY_ICON_EXTENSION)
#define QICON_R(file) QIcon(QV2RAY_ICON_RESOURCE(file))
#define Q_TRAYICON(name) (QIcon(QV2RAY_COLORSCHEME_ROOT_X(GlobalConfig.uiConfig.useDarkTrayIcon) + name + ".png"))

class QvDialog : public QDialog
{
    Q_OBJECT
  public:
    explicit QvDialog(QWidget *parent) : QDialog(parent){};
    virtual void processCommands(QString command, QStringList commands, QMap<QString, QString> args) = 0;
    virtual void updateColorScheme() = 0;
};

namespace Qv2ray::ui
{
    inline QPixmap ApplyEffectToImage(QPixmap src, QGraphicsEffect *effect, int extent = 0)
    {
        if (src.isNull() || !effect)
            return src;
        QGraphicsScene scene;
        QGraphicsPixmapItem item;
        item.setPixmap(src);
        item.setGraphicsEffect(effect);
        scene.addItem(&item);
        QImage res(src.size() + QSize(extent * 2, extent * 2), QImage::Format_ARGB32);
        res.fill(Qt::transparent);
        QPainter ptr(&res);
        scene.render(&ptr, QRectF(), QRectF(-extent, -extent, src.width() + extent * 2, src.height() + extent * 2));
        return QPixmap::fromImage(res);
    }

    inline QPixmap BlurImage(const QPixmap &pixmap, const double rad)
    {
        auto pBlur = new QGraphicsBlurEffect();
        pBlur->setBlurRadius(rad);
        return ApplyEffectToImage(pixmap, pBlur, 0);
    }

    inline QPixmap ColorizeImage(const QPixmap &pixmap, const QColor &color, const qreal factor)
    {
        auto pColor = new QGraphicsColorizeEffect();
        pColor->setColor(color);
        pColor->setStrength(factor);
        return ApplyEffectToImage(pixmap, pColor, 0);
    }
    inline void FastAppendTextDocument(const QString &message, QTextDocument *doc)
    {
        QTextCursor cursor(doc);
        cursor.movePosition(QTextCursor::End);
        cursor.beginEditBlock();
        cursor.insertBlock();
        cursor.insertText(message);
        cursor.endEditBlock();
    }
} // namespace Qv2ray::ui

using namespace Qv2ray::ui;
