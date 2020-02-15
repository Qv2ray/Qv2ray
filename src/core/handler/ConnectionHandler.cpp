#include "ConnectionHandler.hpp"

namespace Qv2ray::core::handlers
{
    ConnectionHandler::ConnectionHandler()
    {
        auto _connections = GlobalConfig.connections;
        auto _subscriptions = GlobalConfig.subscriptions;
        auto _groups = GlobalConfig.groups;
        //
        saveTimerId = startTimer(60000);
    }
    ConnectionHandler::~ConnectionHandler()
    {
    }
}
