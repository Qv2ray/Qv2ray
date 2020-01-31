#pragma once
#include "qhttpserver.h"
#include <QObject>
namespace Qv2ray::components::pac
{
    QString ConvertGFWToPAC(const QString &rawContent, const QString &customProxyString);
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

using namespace Qv2ray::components;
using namespace Qv2ray::components::pac;
