#pragma once

#include "ui/common/QvMessageBus.hpp"
#include "ui/widgets/Qv2rayWidgetApplication.hpp"

#include <QDialog>
#include <QGraphicsEffect>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QJsonObject>
#include <QPainter>
#include <QShowEvent>
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

constexpr auto QV2RAY_ICON_EXTENSION = ".svg";

#define QV2RAY_COLORSCHEME_ROOT_X(isDark) (QString(":/assets/icons/") + ((isDark) ? "ui_dark/" : "ui_light/"))
#define QV2RAY_TRASY_ICON_STYLE_X(isGlyph) (QString("") + ((isGlyph) ? "glyph-" : ""))
#define QV2RAY_COLORSCHEME_ROOT QV2RAY_COLORSCHEME_ROOT_X(GlobalConfig.uiConfig.useDarkTheme)
#define QV2RAY_ICON_RESOURCE(file) (QV2RAY_COLORSCHEME_ROOT + file + QV2RAY_ICON_EXTENSION)
#define QICON_R(file) QPixmap(QV2RAY_ICON_RESOURCE(file))
#define Q_TRAYICON(name) (QPixmap(QV2RAY_COLORSCHEME_ROOT_X(GlobalConfig.uiConfig.useDarkTrayIcon) +\
    (QV2RAY_TRASY_ICON_STYLE_X(GlobalConfig.uiConfig.useGlyphTrayIcon)) + name + ".png"))

class QvStateObject
{
    using options_save_func_t = std::function<QJsonValue()>;
    using options_restore_func_t = std::function<void(QJsonValue)>;
    using options_storage_type = std::map<QString, std::pair<options_save_func_t, options_restore_func_t>>;

  public:
    explicit QvStateObject(const QString &name) : windowName(name){};
    void SaveState()
    {
        QvWidgetApplication->UIStates[windowName] = saveStateImpl();
    }

    void RestoreState()
    {
        restoreStateImpl(QvWidgetApplication->UIStates[windowName].toObject());
    }

  protected:
    void addStateOptions(QString name, std::pair<options_save_func_t, options_restore_func_t> funcs)
    {
        state_options_list[name] = funcs;
    }

  private:
    QJsonObject saveStateImpl()
    {
        QJsonObject o;
        for (const auto &[name, pair] : state_options_list)
            o[name] = pair.first();
        return o;
    }
    void restoreStateImpl(const QJsonObject &o)
    {
        for (const auto &[name, pair] : state_options_list)
            if (o.contains(name))
                pair.second(o[name]);
    }
    options_storage_type state_options_list;
    const QString windowName;
};

class QvDialog
    : public QDialog
    , public QvStateObject
{
    Q_OBJECT
  public:
    explicit QvDialog(const QString &name, QWidget *parent) : QDialog(parent), QvStateObject(name)
    {
        connect(this, &QvDialog::finished, [this] { SaveState(); });
    }
    virtual ~QvDialog(){};
    virtual void processCommands(QString command, QStringList commands, QMap<QString, QString> args) = 0;

  protected:
    virtual QvMessageBusSlotDecl = 0;
    virtual void updateColorScheme() = 0;
    void showEvent(QShowEvent *event) override
    {
        QWidget::showEvent(event);
        RestoreState();
    }
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
