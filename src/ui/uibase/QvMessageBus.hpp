#pragma once
#include <QObject>

#define QvMsgBusConnect(BODY) connect(&::Qv2ray::messageBus, &::Qv2ray::base::QvMessageBusObject::QvSendMessage, BODY);
#define QvMsgBusLambda(LAMBDABODY) [this](::Qv2ray::base::QvMessage msg) { LAMBDABODY }
#define QvMsgBusSwitch(SWITCHBODY) switch(msg) { SWITCHBODY }

#define QvMsgBusSlot(Impl) QvMsgBusConnect(QvMsgBusLambda(QvMsgBusSwitch(Impl)))

#define QvMsgBusShowDefault \
    case MINIMIZE_WINDOWS:\
    if(this) { this->setWindowOpacity(0); }\
    break;
//if(this) { this->setWindowState((this->windowState() & ~Qt::WindowMinimized) | Qt::WindowActive); }

#define QvMsgBusHideDefault \
    case RESTORE_WINDOWS:\
    if(this) { this->setWindowOpacity(1); }\
    break;
//if(this) { this->setWindowState(this->windowState() | Qt::WindowMinimized); }

#define QvMsgBusRetranslateDefault \
    case RETRANSLATE:\
    if(this) {this->retranslateUi(this);}\
    break;

#define QvMsgBusImplDefault\
    QvMsgBusShowDefault\
    QvMsgBusHideDefault\
    QvMsgBusRetranslateDefault

namespace Qv2ray::base
{
    Q_NAMESPACE
    enum QvMessage {
        /// Hide all windows.
        MINIMIZE_WINDOWS,
        /// Show all windows.
        RESTORE_WINDOWS,
        /// Retranslate User Interface.
        RETRANSLATE,
        /// Change Color Scheme
        //CHANGE_COLORSCHEME
    };
    Q_ENUM_NS(QvMessage);

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
