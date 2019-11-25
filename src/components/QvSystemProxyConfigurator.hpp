#ifndef QVSYSTEMPROXYCONFIGURATOR_H
#define QVSYSTEMPROXYCONFIGURATOR_H
#include "QvUtils.hpp"
#include <QObject>
//
namespace Qv2ray
{
    namespace Components
    {
        bool ClearSystemProxy();
        bool SetSystemProxy(QString address, int port, bool usePAC);
    }
}

using namespace Qv2ray::Components;

#endif // QVSYSTEMPROXYCONFIGURATOR_H
