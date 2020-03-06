#ifndef SHADOWSOCKSRINSTANCE_H
#define SHADOWSOCKSRINSTANCE_H
#include "httpproxy.hpp"
#include <QThread>
#include "base/Qv2rayBase.hpp"
#include "core/CoreSafeTypes.hpp"
class SSRThread;
namespace Qv2ray::core::kernel {
class V2rayKernelInstance;
class ShadowsocksrInstance
{
public:
    ShadowsocksrInstance();
    bool restartShadowsocksRThread(V2rayKernelInstance*,const CONFIGROOT& root,const ConnectionId& id);
    void stop();
private:
    std::unique_ptr<HttpProxy> httpProxy;
    std::unique_ptr<SSRThread> ssrThread;
};
}

#endif // SHADOWSOCKSRINSTANCE_H
