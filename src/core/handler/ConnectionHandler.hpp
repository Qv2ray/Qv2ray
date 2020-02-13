#pragma once

#include "base/Qv2rayBase.hpp"

class ConnectionGroupIdentifier;

namespace Qv2ray::core::handler
{
    class ConnectionHandler : public QObject
    {
            Q_OBJECT
            explicit ConnectionHandler();
            ~ConnectionHandler();
            void ReloadConnections();

        public slots:
            optional<QString> DeleteConnection(const ConnectionIdentifier &identifier);
            //
            optional<QString> RenameConnection(const ConnectionIdentifier &identifier, const QString &newName);
            optional<QString> MoveConnection(const ConnectionIdentifier &identifier, const ConnectionGroupIdentifier &newGroup);
            optional<QString> DuplicateConnection(const ConnectionIdentifier &identifier, ConnectionIdentifier *newIdentifier);
    };
}
