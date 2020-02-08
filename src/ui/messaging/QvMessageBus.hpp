#pragma once
#include <QObject>

#define QvMessageBusConnect(CLASSNAME) connect(&messageBus, &QvMessageBusObject::QvSendMessage, this, &CLASSNAME::on_QvMessageReceived)

#define QvMessageBusSlotHeader void on_QvMessageReceived(QvMessage msg);
#define QvMessageBusSlotImpl(CLASSNAME) void CLASSNAME::on_QvMessageReceived(QvMessage msg)

#define QvMessageBusShowDefault \
    case SHOW_WINDOWS:\
    this->setWindowOpacity(1);\
    break;

#define QvMessageBusHideDefault \
    case HIDE_WINDOWS:\
    this->setWindowOpacity(0);\
    break;

#define QvMessageBusRetranslateDefault \
    case RETRANSLATE:\
    {\
        this->retranslateUi(this);\
    }\
    break;

namespace Qv2ray::ui::messaging
{
    Q_NAMESPACE
    enum QvMessage {
        /// Show all windows.
        SHOW_WINDOWS,
        /// Hide all windows.
        HIDE_WINDOWS,
        /// Retranslate User Interface.
        RETRANSLATE,
        /// Change Color Scheme
        //CHANGE_COLORSCHEME
    };
    Q_ENUM_NS(QvMessage);
    //
    class QvMessageBusObject : public QObject
    {
            Q_OBJECT
        public:
            explicit QvMessageBusObject();

            //
            void EmitGlobalSignal(QvMessage msg);
        signals:
            void QvSendMessage(QvMessage msg);
        private slots:
            void on_QvMessageReceived(QvMessage msg);
    };

    // Danger, new is used here. Possible memory leak (hope not so much leak)
    inline QvMessageBusObject messageBus = QvMessageBusObject();
}

using namespace Qv2ray::ui::messaging;
