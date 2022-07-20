#pragma once
#include <QObject>

#define QvMessageBusConnect() connect(UIMessageBus, &std::decay_t<decltype(*UIMessageBus)>::QvSendMessage, this, &std::decay_t<decltype(*this)>::on_QvMessageReceived)

#define QvMessageBusSlotSig Qv2ray::components::MessageBus::QvMBMessage msg
#define QvMessageBusSlotIdentifier on_QvMessageReceived

#define QvMessageBusSlotDecl void QvMessageBusSlotIdentifier(QvMessageBusSlotSig)
#define QvMessageBusSlotImpl(CLASSNAME) void CLASSNAME::QvMessageBusSlotIdentifier(QvMessageBusSlotSig)

#define MBUpdateColorSchemeDefaultImpl                                                                                                                                   \
    case Qv2ray::components::MessageBus::UPDATE_COLORSCHEME:                                                                                                             \
    {                                                                                                                                                                    \
        this->updateColorScheme();                                                                                                                                       \
        break;                                                                                                                                                           \
    }

namespace Qv2ray::components::MessageBus
{
    Q_NAMESPACE
    enum QvMBMessage
    {
        UPDATE_COLORSCHEME
    };
    Q_ENUM_NS(QvMBMessage)

    class QvMessageBusObject : public QObject
    {
        Q_OBJECT
      public:
        explicit QvMessageBusObject(){};
        void EmitGlobalSignal(const QvMBMessage &msg)
        {
            emit QvSendMessage(msg);
        }
      signals:
        void QvSendMessage(const QvMBMessage &msg);
    };

} // namespace Qv2ray::components::MessageBus

inline Qv2ray::components::MessageBus::QvMessageBusObject *UIMessageBus;
