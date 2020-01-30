#pragma once
#include <QString>
#include <QObject>
//
namespace Qv2ray::components
{
    bool ClearSystemProxy();
    bool SetSystemProxy(const QString &address, int port, bool usePAC);
}

using namespace Qv2ray::components;
