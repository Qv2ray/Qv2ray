#pragma once

#include <QApplication>
#include <QGraphicsEffect>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QTextCursor>
#include <QTextDocument>
#include <QtGui>

namespace Qv2ray::ui
{
    inline QPixmap ApplyEffectToImage(QPixmap src, QGraphicsEffect *effect, int extent)
    {
        constexpr int extent2 = 0;
        if (src.isNull())
            return QPixmap(); // No need to do anything else!
        if (!effect)
            return src; // No need to do anything else!
        QGraphicsScene scene;
        auto p = scene.addPixmap(src);
        p->setGraphicsEffect(effect);
        //
        QImage res(src.size() + QSize(extent2, extent2), QImage::Format_ARGB32);
        res.fill(Qt::transparent);
        QPainter ptr(&res);
        //
        scene.render(&ptr, QRectF(), QRectF(-extent, -extent, src.width() + extent2, src.height() + extent * 2));
        //
        scene.removeItem(p);
        return QPixmap::fromImage(res);
    }
    inline QPixmap BlurImage(const QPixmap &pixmap, const double rad)
    {
        QGraphicsBlurEffect pBlur;
        pBlur.setBlurRadius(rad);
        return ApplyEffectToImage(pixmap, &pBlur, 0);
    }

    inline QPixmap ColorizeImage(const QPixmap &pixmap, const QColor &color, const qreal factor)
    {
        QGraphicsColorizeEffect pColor;
        pColor.setColor(color);
        pColor.setStrength(factor);
        return ApplyEffectToImage(pixmap, &pColor, 0);
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
