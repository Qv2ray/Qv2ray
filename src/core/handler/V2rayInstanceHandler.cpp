#include "ConnectionHandler.hpp"
#include "core/connection/Generation.hpp"

optional<QString> QvConnectionHandler::_CHTryStartConnection_p(const ConnectionId &id, const CONFIGROOT &root)
{
    auto &connectionMeta = connections[id];
    auto fullConfig = GenerateRuntimeConfig(root);
    return kernelInstance->StartConnection(fullConfig);
    //if (startFlag) {
    //    bool usePAC = GlobalConfig.inboundConfig.pacConfig.enablePAC;
    //    bool pacUseSocks = GlobalConfig.inboundConfig.pacConfig.useSocksProxy;
    //    bool httpEnabled = GlobalConfig.inboundConfig.useHTTP;
    //    bool socksEnabled = GlobalConfig.inboundConfig.useSocks;
    //
    //    if (usePAC) {
    //        bool canStartPAC = true;
    //        QString pacProxyString;  // Something like this --> SOCKS5 127.0.0.1:1080; SOCKS 127.0.0.1:1080; DIRECT; http://proxy:8080
    //        auto pacIP = GlobalConfig.inboundConfig.pacConfig.localIP;
    //
    //        if (pacIP.isEmpty()) {
    //            LOG(MODULE_PROXY, "PAC Local IP is empty, default to 127.0.0.1")
    //            pacIP = "127.0.0.1";
    //        }
    //
    //        if (pacUseSocks) {
    //            if (socksEnabled) {
    //                pacProxyString = "SOCKS5 " + pacIP + ":" + QSTRN(GlobalConfig.inboundConfig.socks_port);
    //            } else {
    //                LOG(MODULE_UI, "PAC is using SOCKS, but it is not enabled")
    //                QvMessageBoxWarn(this, tr("Configuring PAC"), tr("Could not start PAC server as it is configured to use SOCKS, but it is not enabled"));
    //                canStartPAC = false;
    //            }
    //        } else {
    //            if (httpEnabled) {
    //                pacProxyString = "PROXY " + pacIP + ":" + QSTRN(GlobalConfig.inboundConfig.http_port);
    //            } else {
    //                LOG(MODULE_UI, "PAC is using HTTP, but it is not enabled")
    //                QvMessageBoxWarn(this, tr("Configuring PAC"), tr("Could not start PAC server as it is configured to use HTTP, but it is not enabled"));
    //                canStartPAC = false;
    //            }
    //        }
    //
    //        if (canStartPAC) {
    //            pacServer.SetProxyString(pacProxyString);
    //            pacServer.StartListen();
    //        } else {
    //            LOG(MODULE_PROXY, "Not starting PAC due to previous error.")
    //        }
    //    }
    //
    //    if (GlobalConfig.inboundConfig.setSystemProxy) {
    //        MWSetSystemProxy();
    //    }
    //}
    return "startFlag";
}
