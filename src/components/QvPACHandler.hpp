#ifndef QVPACHANDLER_H
#define QVPACHANDLER_H
#include "QvUtils.hpp"
#include "qhttpserver.h"
#include <QObject>
namespace Qv2ray
{
    namespace Components
    {
        class PACServer : public QObject
        {
                Q_OBJECT
            public:
                explicit PACServer();
                ~PACServer();
                void SetProxyString(const QString &proxyString);
                void StartListen();
                void StopServer();

                QString gfwFilePath;

            public slots:
                void onNewRequest(QHttpRequest *request, QHttpResponse *response);

            private:
                bool isStarted;
                QHttpServer *pacServer;
                QString pacContent;
                QString proxyString;
        };
    }
}

using namespace Qv2ray::Components;
#endif // QVPACHANDLER_H
