#include <QMetaEnum>

#include "QvMessageBus.hpp"
#include "base/Qv2rayBase.hpp"

namespace Qv2ray::ui::messaging
{
    QvMessageBusObject::QvMessageBusObject()
    {
        QvMessageBusConnect(QvMessageBusObject);
    }

    void QvMessageBusObject::on_QvMessageReceived(QvMessage msg)
    {
        QMetaEnum metaEnum = QMetaEnum::fromType<QvMessage>();
        DEBUG(MODULE_MESSAGING, "Signal recieved: " + QString(metaEnum.valueToKey(msg)));
    }

    void QvMessageBusObject::EmitGlobalSignal(QvMessage msg)
    {
        QMetaEnum metaEnum = QMetaEnum::fromType<QvMessage>();
        LOG(MODULE_MESSAGING, "Emitting signal: " + QString(metaEnum.valueToKey(msg)));
        emit QvSendMessage(msg);
    }
}
