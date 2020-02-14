#pragma once

#include "base/Qv2rayBase.hpp"

class QvConnectionListModel;

namespace Qv2ray::core::handler
{
    class ConnectionHandler : public QObject
    {
            //    Q_OBJECT
            //public:
            //    explicit ConnectionHandler();
            //    ~ConnectionHandler();
            //    void ReloadConnections();
            //    // Get operations.
            //    QvConnectionListModel GetConnections();
            //
            //public slots:
            //    optional<QString> DeleteConnection(const ConnectionId &id);
            //    optional<QString> RenameConnection(const ConnectionId &id, const QString &newName);
            //    optional<QString> DuplicateConnection(const ConnectionId &id, ConnectionId *newId);
            //    optional<QString> MoveConnection(const ConnectionId &id, const GroupId &newGroupId);
            //    // Group Operations
            //    optional<QString> DeleteGroup(const GroupId &id);
            //    optional<QString> RenameGroup(const GroupId &id, const QString &newName);
    };
}

using namespace Qv2ray::core::handler;
