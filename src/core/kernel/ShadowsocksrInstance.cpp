#include "ShadowsocksrInstance.hpp"
#include "common/QvHelpers.hpp"
#include "KernelInteractions.hpp"
#include "core/connection/ConnectionIO.hpp"
#include "core/CoreUtils.hpp"
#include <memory>

namespace Qv2ray::core::kernel{
ShadowsocksrInstance::ShadowsocksrInstance() = default;

bool ShadowsocksrInstance::restartShadowsocksRThread(V2rayKernelInstance *kernel, const CONFIGROOT &root,const ConnectionId& id)
{
    int socks_local_port=0;
    int http_local_port=0;
    QString tag;
    QString listen_address;
    for(const auto& item:root["inbounds"].toArray())
    {
        auto protocol=item.toObject()["protocol"].toString(QObject::tr("N/A"));
        if(protocol=="socks")
        {
            tag = item.toObject()["tag"].toString("");
            socks_local_port = item.toObject()["port"].toInt(0);
            listen_address =item.toObject()["listen"].toString("");
        } else if(protocol=="http"){
            http_local_port= item.toObject()["port"].toInt(0);
        }
    }
    if(socks_local_port==0)
    {
        //socks must enabled
        return false;
    }
    OUTBOUND outbound = OUTBOUND(root["outbounds"].toArray().first().toObject());
    auto ssrServer = StructFromJsonString<ShadowSocksRServerObject>(JsonToString(outbound["settings"].toObject()["servers"].toArray().first().toObject()));
    auto remotePort=ssrServer.port;
    auto remote_host=ssrServer.address.toStdString();
    auto method=ssrServer.method.toStdString();
    auto password=ssrServer.password.toStdString();
    auto obfs=ssrServer.obfs.toStdString();
    auto obfs_param=ssrServer.obfs_param.toStdString();
    auto protocol=ssrServer.protocol.toStdString();
    auto protocol_param=ssrServer.protocol_param.toStdString();
    ssrThread=std::make_unique<SSRThread>(
                socks_local_port,
                remotePort,
                listen_address.toStdString(),
                remote_host,
                method,
                password,
                obfs,
                obfs_param,
                protocol,
                protocol_param,
                tag);
    ssrThread->connect(ssrThread.get(),&SSRThread::onSSRThreadLog,kernel,[kernel,id](const QString& output){
        emit kernel->OnProcessOutputReadyRead(id,output);
    });
    ssrThread->connect(ssrThread.get(), &SSRThread::OnDataReady, kernel, &V2rayKernelInstance::onAPIDataReady);
    ssrThread->start();
    if(http_local_port!=0)
    {
        httpProxy=make_unique<HttpProxy>();
        httpProxy->httpListen(QHostAddress{listen_address},http_local_port,socks_local_port);
    }
    return true;
}

void ShadowsocksrInstance::stop()
{
    if(ssrThread!=nullptr&&ssrThread->isRunning())
    {
        ssrThread=make_unique<SSRThread>();
    }
    httpProxy=nullptr;
}

}
