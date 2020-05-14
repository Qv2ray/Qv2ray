#include "QvPortDetector.hpp"

#include <QTcpServer>

namespace Qv2ray::components::port
{
    bool detectPortTCP(quint16 port)
    {
        QTcpServer server;
        return server.listen(QHostAddress::LocalHost, port);
    }

} // namespace Qv2ray::components::port
