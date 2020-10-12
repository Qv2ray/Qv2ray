#include "QvMessageBus.hpp"

#include "base/Qv2rayBase.hpp"

#include <QMetaEnum>

#define QV_MODULE_NAME "MessageBus"

namespace Qv2ray::ui::messaging
{
    QvMessageBusObject::QvMessageBusObject()
    {
    }

    void QvMessageBusObject::EmitGlobalSignal(const QvMBMessage &msg)
    {
        QMetaEnum metaEnum = QMetaEnum::fromType<QvMBMessage>();
        LOG("Emitting signal: ", QString(metaEnum.valueToKey(msg)));
        emit QvSendMessage(msg);
    }
} // namespace Qv2ray::ui::messaging
