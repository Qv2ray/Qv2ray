#include <QMetaEnum>

#include "QvGlobalMessageBus.hpp"
#include "base/Qv2rayBase.hpp"

namespace Qv2ray::common
{
    QvMessageBusObject::QvMessageBusObject()
    {
        instance = this;
        QvMessageBusConnect(QvMessageBusObject);
    }

    void QvMessageBusObject::on_QvMessageReceived(QvMessage msg)
    {
        QMetaEnum metaEnum = QMetaEnum::fromType<QvMessage>();
        LOG(MESSAGING, metaEnum.valueToKey(msg));
    }
}
