#pragma once
#include <QObject>

#define QvMessageBusConnect(CLASSNAME) connect(::Qv2ray::common::QvMessageBusObject::instance, &::Qv2ray::common::QvMessageBusObject::QvSendMessage, this, &CLASSNAME::on_QvMessageReceived)

namespace Qv2ray::common
{
    class QvMessageBusObject : public QObject
    {
            Q_OBJECT
        public:
            explicit QvMessageBusObject();
            QvMessageBusObject *instance;
            enum QvMessage {
                ///
            };
            Q_ENUM(QvMessage);
            //
        signals:
            void QvSendMessage(QvMessage msg);
        private slots:
            void on_QvMessageReceived(QvMessage msg);
    };
}

using namespace Qv2ray::common;
