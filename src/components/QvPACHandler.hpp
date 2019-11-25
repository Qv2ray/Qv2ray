#ifndef QVPACHANDLER_H
#define QVPACHANDLER_H
#include "QvUtils.hpp"
#include "qhttpserver.h"
#include <QObject>
namespace Qv2ray
{
    namespace Components
    {
        class PACHandler : public QObject
        {
                Q_OBJECT
            public:
                explicit PACHandler();
                ~PACHandler();
                void SetLocalEndpoint(QString address, ushort port);
                void SetProxyString(QString proxyString);
                void StartListen();
                void StopServer();

                QString gfwFilePath;

            public slots:
                void onNewRequest(QHttpRequest *request, QHttpResponse *response);

            private:
                QHttpServer *pacServer;
                QString address;
                ushort port;
                QString pacContent;
                QString proxyString;
        };
    }
}

using namespace Qv2ray::Components;
#endif // QVPACHANDLER_H
