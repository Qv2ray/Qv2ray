#pragma once
#include <QString>
#include <QObject>
//
namespace Qv2ray::components::proxy
{
    bool ClearSystemProxy();
    bool SetSystemProxy(const QString &address, int port, bool usePAC);
}

using namespace Qv2ray::components;
using namespace Qv2ray::components::proxy;
