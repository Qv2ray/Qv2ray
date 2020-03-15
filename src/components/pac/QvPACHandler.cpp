#include "QvPACHandler.hpp"

#include "3rdparty/cpp-httplib/httplib.h"
#include "common/QvHelpers.hpp"
#include "core/CoreUtils.hpp"

namespace Qv2ray::components::pac
{
    PACServer::PACServer(QObject *parent) : QThread(parent)
    {
        server = new httplib::Server();
        connect(this, &QThread::finished, this, &QThread::deleteLater);
    }
    PACServer::~PACServer()
    {
        wait();
        DEBUG(MODULE_PROXY, "~PACServer")
        delete server;
    }
    void PACServer::stopServer()
    {
        if (server->is_running())
            server->stop();
        quit();
        LOG(MODULE_UI, "Stopping PAC server")
    }
    void PACServer::run()
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
        server->Get("/pac", pacRequestHandler);
        auto result = server->listen(address.toStdString().c_str(), static_cast<ushort>(port));
        if (result)
        {
            DEBUG(MODULE_PROXY, "PAC handler stopped.")
        }
        else
        {
            LOG(MODULE_PROXY, "Failed to listen on port " + QSTRN(port) + ", possible permission denied.")
            QvMessageBoxWarn(nullptr, tr("PAC Handler"), tr("Failed to listen PAC request on this port, please verify the permissions"));
        }
    }

    void PACServer::pacRequestHandler(const httplib::Request &req, httplib::Response &rsp)
    {
        rsp.set_header("Server", "Qv2ray/" QV2RAY_VERSION_STRING " PAC_Handler");
        if (req.method == "GET")
        {
            if (req.path == "/pac")
            {
                DEBUG(MODULE_PROXY, "Serving PAC file request.")
                //
                rsp.status = 200;
                rsp.set_content(pacContent.toStdString(), "application/javascript; charset=utf-8");
                DEBUG(MODULE_PROXY, "Serving a pac file...")
            }
            else
            {
                rsp.status = 404;
                rsp.set_content("NOT FOUND", "text/plain; charset=utf-8");
            }
        }
        else
        {
            rsp.status = 405;
            rsp.set_content("PAC ONLY SUPPORT GET", "text/plain; charset=utf-8");
        }
    }
} // namespace Qv2ray::components::pac
