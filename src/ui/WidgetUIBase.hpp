#pragma once

#include "MessageBus/MessageBus.hpp"
#include "Qv2rayApplication.hpp"
#include "StyleManager/StyleManager.hpp"

#include <QDialog>
#include <QGraphicsEffect>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QPainter>
#include <QTextCursor>
#include <QTextDocument>

// GUI TOOLS
inline void RED(QWidget *obj)
{
    auto _temp = obj->palette();
    _temp.setColor(QPalette::Text, Qt::red);
    obj->setPalette(_temp);
}

inline void BLACK(QWidget *obj)
{
    obj->setPalette(qApp->palette());
}

#define _COLOR_RESX(isDark) ((isDark) ? u":/ui_dark/"_qs : u":/ui_light/"_qs)
#define STYLE_RESX(file) (_COLOR_RESX(StyleManager->isDarkMode()) + file + ".svg")
#define Q_TRAYICON(name) (QPixmap(_COLOR_RESX(GlobalConfig->appearanceConfig->DarkModeTrayIcon) + "glyph-" + name + ".png"))

class QvDialog : public QDialog
{
    Q_OBJECT
  public:
    explicit QvDialog(const QString &, QWidget *parent) : QDialog(parent){};
    virtual ~QvDialog() = default;
    virtual void processCommands(QString command, QStringList commands, QMap<QString, QString> args) = 0;

  protected:
    virtual QvMessageBusSlotDecl = 0;
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
        // Clean up
        item.setGraphicsEffect(nullptr);
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
