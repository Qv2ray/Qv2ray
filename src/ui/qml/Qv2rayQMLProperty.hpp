#pragma once

#include "core/handler/ConfigHandler.hpp"
#include "utils/QvHelpers.hpp"

#include <QObject>
#include <QVariant>

#define Q_PROPERTY_DECL(type, name, val)                                                                                                             \
  public:                                                                                                                                            \
    typedef type __prop_##name##_type;                                                                                                               \
                                                                                                                                                     \
    type &name()                                                                                                                                     \
    {                                                                                                                                                \
        return __##name;                                                                                                                             \
    }                                                                                                                                                \
    void set_##name(const type &_in_val)                                                                                                             \
    {                                                                                                                                                \
        setProperty(#name, QVariant::fromValue(_in_val));                                                                                            \
    }                                                                                                                                                \
                                                                                                                                                     \
  private:                                                                                                                                           \
    type __##name = val

#define PROPERTY_CHANGED_SIGNAL_NAME(name) on_##name##_changed
#define PROPERTY_CHANGED_SIGNAL(name)                                                                                                                \
    void __property__internal__##name##_chk(const __prop_##name##_type d);                                                                           \
    void PROPERTY_CHANGED_SIGNAL_NAME(name)();

#define PROPERTY_ARGS(type, name) type name MEMBER __##name
#define PROPERTY_NOTIFY(type, name) PROPERTY_ARGS(type, name) NOTIFY PROPERTY_CHANGED_SIGNAL_NAME(name)

class Qv2rayQMLProperty : public QObject
{
    Q_OBJECT
    Q_PROPERTY_DECL(ConnectionGroupPair, currentConnection, {});
    Q_PROPERTY_DECL(QString, assetsPrefix, "qrc:/assets/icons/ui_dark");

    Q_PROPERTY(PROPERTY_ARGS(ConnectionGroupPair, currentConnection))
    Q_PROPERTY(PROPERTY_ARGS(QString, assetsPrefix))

  public:
    explicit Qv2rayQMLProperty(QObject *parent = nullptr);
  public slots:
    void onButtonClicked(const QString &text)
    {
        QvMessageBoxWarn(nullptr, "Hi!", text);
    }
};
