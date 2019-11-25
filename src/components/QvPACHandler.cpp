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
            pacServer = new QHttpServer();
            connect(pacServer, &QHttpServer::newRequest, this, &PACHandler::onNewRequest);
            //
            auto conf = GetGlobalConfig();
            auto address = QSTRING(conf.inboundConfig.listenip);
            auto port = conf.inboundConfig.pacConfig.port;
            //
            QString gfwContent = StringFromFile(new QFile(QV2RAY_RULES_GFWLIST_PATH));
            pacContent = ConvertGFWToPAC(gfwContent, proxyString);
            //
            auto result = pacServer->listen(QHostAddress(address), static_cast<ushort>(port));

            if (result) {
                isStarted = true;
                LOG(MODULE_PROXY, "Started PAC listener")
            } else {
                LOG(MODULE_PROXY, "Failed to listen on port " + to_string(port) + ", please verify the permission.")
                QvMessageBox(nullptr, tr("PAC Handler"), tr("Failed to listen PAC request on this port, please verify the permissions"));
            }
        }
        void PACHandler::StopServer()
        {
            if (isStarted) {
                pacServer->close();
                delete pacServer;
                isStarted = false;
            }
        }
        void PACHandler::onNewRequest(QHttpRequest *req, QHttpResponse *rsp)
        {
            rsp->setHeader("Server", "Qv2ray/" QV2RAY_VERSION_STRING " PAC_Handler");

            if (req->method() == QHttpRequest::HTTP_GET) {
                //
                if (req->path() == "/pac") {
                    rsp->setHeader("Content-Type", "application/javascript; charset=utf-8");
                    rsp->writeHead(QHttpResponse::StatusCode::STATUS_OK);
                    rsp->end(pacContent.toUtf8());
                    DEBUG(MODULE_PROXY, "Serving a pac file...")
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
