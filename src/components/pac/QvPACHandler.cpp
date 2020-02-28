#include "QvPACHandler.hpp"

#include "common/QvHelpers.hpp"
#include "core/CoreUtils.hpp"
#include "qhttprequest.h"
#include "qhttpresponse.h"

namespace Qv2ray::components::pac
{

    PACServer::PACServer() : QObject(), pacServer(this)
    {
        connect(&pacServer, &QHttpServer::newRequest, this, &PACServer::onNewRequest);
    }
    PACServer::~PACServer()
    {
        if (isStarted) { pacServer.close(); }
    }
    void PACServer::SetProxyString(const QString &proxyString)
    {
        DEBUG(MODULE_PROXY, "Setting new PAC proxy string: " + proxyString)
        this->proxyString = proxyString;
    }
    void PACServer::StartListen()
    {
        LOG(MODULE_PROXY, "Starting PAC listener")
        //
        auto address = GlobalConfig.inboundConfig.listenip;
        auto port = GlobalConfig.inboundConfig.pacConfig.port;
        //
        DEBUG(MODULE_PROXY, "PAC Listening local endpoint: " + address + ":" + QSTRN(port))
        //
        QString gfwContent = StringFromFile(QV2RAY_RULES_GFWLIST_PATH);
        pacContent = ConvertGFWToPAC(gfwContent, proxyString);
        //
        auto result = pacServer.listen(QHostAddress(address), static_cast<ushort>(port));

        if (result)
        {
            isStarted = true;
            DEBUG(MODULE_PROXY, "Started PAC handler")
        }
        else
        {
            LOG(MODULE_PROXY, "Failed to listen on port " + QSTRN(port) + ", possible permission denied.")
            QvMessageBoxWarn(nullptr, tr("PAC Handler"), tr("Failed to listen PAC request on this port, please verify the permissions"));
        }
    }

    void PACServer::StopServer()
    {
        if (isStarted)
        {
            pacServer.close();
            DEBUG(MODULE_PROXY, "PAC Handler stopped.")
            isStarted = false;
        }
    }

    void PACServer::onNewRequest(QHttpRequest *req, QHttpResponse *rsp)
    {
        rsp->setHeader("Server", "Qv2ray/" QV2RAY_VERSION_STRING " PAC_Handler");

        if (req->method() == QHttpRequest::HTTP_GET)
        {
            //
            if (req->path() == "/pac")
            {
                DEBUG(MODULE_PROXY, "Serving PAC file request.")
                //
                rsp->setHeader("Content-Type", "application/javascript; charset=utf-8");
                rsp->writeHead(QHttpResponse::StatusCode::STATUS_OK);
                rsp->end(pacContent.toUtf8());
                DEBUG(MODULE_PROXY, "Serving a pac file...")
            }
            else
            {
                rsp->writeHead(QHttpResponse::StatusCode::STATUS_NOT_FOUND);
                rsp->end("NOT FOUND");
            }
        }
        else
        {
            rsp->writeHead(QHttpResponse::StatusCode::STATUS_METHOD_NOT_ALLOWED);
            rsp->end("PAC ONLY SUPPORT GET");
        }
    }
} // namespace Qv2ray::components::pac
