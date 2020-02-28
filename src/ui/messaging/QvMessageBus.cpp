#include <QMetaEnum>

#include "QvMessageBus.hpp"
#include "base/Qv2rayBase.hpp"

namespace Qv2ray::ui::messaging
{
    QvMessageBusObject::QvMessageBusObject()
    {
    }

    void QvMessageBusObject::EmitGlobalSignal(const QvMBMessage &msg)
    {
        QMetaEnum metaEnum = QMetaEnum::fromType<QvMBMessage>();
        LOG(MODULE_MESSAGING, "Emitting signal: " + QString(metaEnum.valueToKey(msg)));
        emit QvSendMessage(msg);
    }
}
