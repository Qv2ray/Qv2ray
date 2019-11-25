#include "QvPACHandler.hpp"
#include "qhttprequest.h"
#include "qhttpresponse.h"

namespace Qv2ray
{
    namespace Components
    {
        PACHandler::PACHandler() : QObject()
        {
            pacServer = new QHttpServer();
            connect(pacServer, &QHttpServer::newRequest, this, &PACHandler::onNewRequest);
        }
        void PACHandler::SetLocalEndpoint(QString address, ushort port)
        {
            this->address = address;
            this->port = port;
        }
        PACHandler::~PACHandler()
        {
            pacServer->close();
            delete pacServer;
        }
        void PACHandler::SetProxyString(QString proxyString)
        {
            this->proxyString = proxyString;
        }
        void PACHandler::StartListen()
        {
            QString gfwContent = StringFromFile(new QFile(gfwFilePath));
            pacContent = ConvertGFWToPAC(gfwContent, proxyString);
            //
            pacServer->listen(QHostAddress(address), port);
        }
        void PACHandler::StopServer()
        {
            pacServer->close();
        }
        void PACHandler::onNewRequest(QHttpRequest *req, QHttpResponse *rsp)
        {
            rsp->setHeader("Server", "Qv2ray/" QV2RAY_VERSION_STRING " PAC Handler");

            if (req->method() == QHttpRequest::HTTP_GET) {
                //
                if (req->path() == "/pac.txt") {
                    rsp->setHeader("Content-Type", "application/javascript; charset=utf-8");
                    rsp->writeHead(QHttpResponse::StatusCode::STATUS_OK);
                    rsp->end(pacContent.toUtf8());
                } else {
                    rsp->writeHead(QHttpResponse::StatusCode::STATUS_NOT_FOUND);
                    rsp->end("NOT FOUND");
                }
            } else {
                rsp->writeHead(QHttpResponse::StatusCode::STATUS_METHOD_NOT_ALLOWED);
                rsp->end("PAC ONLY SUPPORT GET");
            }
        }
    }
}
