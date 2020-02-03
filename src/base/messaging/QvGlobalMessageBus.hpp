#pragma once
#include <QObject>

#define QvMessageBusConnect(CLASSNAME) connect(&::Qv2ray::messageBus, &::Qv2ray::base::QvMessageBusObject::QvSendMessage, this, &CLASSNAME::on_QvMessageReceived)

#define QvMessageBusSlotHeader void on_QvMessageReceived(::Qv2ray::base::QvMessage msg);
#define QvMessageBusSlotImpl(CLASSNAME) void CLASSNAME::on_QvMessageReceived(::Qv2ray::base::QvMessage msg)

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
    this->retranslateUi(this);\
    break;

#define QvMessageBusSlotImplDefault(CLASSNAME) \
    QvMessageBusSlotImpl(CLASSNAME)\
    {\
        switch (msg) {\
                QvMessageBusShowDefault\
                QvMessageBusHideDefault\
                QvMessageBusRetranslateDefault\
        }\
    }

namespace Qv2ray::base
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
}

using namespace Qv2ray::base;
