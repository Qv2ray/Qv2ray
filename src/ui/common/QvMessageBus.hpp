#pragma once
#include <QObject>

#define QvMessageBusConnect(CLASSNAME) connect(&UIMessageBus, &QvMessageBusObject::QvSendMessage, this, &CLASSNAME::on_QvMessageReceived)

#define QvMessageBusSlotSig const QvMBMessage &msg
#define QvMessageBusSlotIdentifier on_QvMessageReceived

#define QvMessageBusSlotDecl void QvMessageBusSlotIdentifier(QvMessageBusSlotSig)
#define QvMessageBusSlotImpl(CLASSNAME) void CLASSNAME::QvMessageBusSlotIdentifier(QvMessageBusSlotSig)

#define MBShowDefaultImpl                                                                                                                            \
    case SHOW_WINDOWS:                                                                                                                               \
        this->setWindowOpacity(1);                                                                                                                   \
        break;

#define MBHideDefaultImpl                                                                                                                            \
    case HIDE_WINDOWS:                                                                                                                               \
        this->setWindowOpacity(0);                                                                                                                   \
        break;

#define MBRetranslateDefaultImpl                                                                                                                     \
    case RETRANSLATE:                                                                                                                                \
        this->retranslateUi(this);                                                                                                                   \
        break;

#define MBUpdateColorSchemeDefaultImpl                                                                                                               \
    case UPDATE_COLORSCHEME: this->updateColorScheme(); break;

namespace Qv2ray::ui::messaging
{
    Q_NAMESPACE
    enum QvMBMessage
    {
        /// Show all windows.
        SHOW_WINDOWS,
        /// Hide all windows.
        HIDE_WINDOWS,
        /// Retranslate User Interface.
        RETRANSLATE,
        /// Change Color Scheme
        UPDATE_COLORSCHEME
    };
    Q_ENUM_NS(QvMBMessage)
    //
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
        // private slots:
        //    void on_QvMessageReceived(QvMessage msg);
    };

    inline QvMessageBusObject UIMessageBus = QvMessageBusObject();
} // namespace Qv2ray::ui::messaging

using namespace Qv2ray::ui::messaging;
