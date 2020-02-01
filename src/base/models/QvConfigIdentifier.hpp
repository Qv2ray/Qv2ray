#pragma once
#include <QString>
#include <QtCore>
#include "3rdparty/x2struct/x2struct.hpp"
namespace Qv2ray::base
{
    struct ConnectionIdentifier {
        QString subscriptionName;
        QString connectionName;
        ConnectionIdentifier() { };
        bool isEmpty()
        {
            return connectionName.isEmpty();
        }
        ConnectionIdentifier(QString connectionName)
        {
            this->connectionName = connectionName;
        }
        ConnectionIdentifier(QString connectionName, QString subscriptionName)
        {
            this->connectionName = connectionName;
            this->subscriptionName = subscriptionName;
        }
        const QString IdentifierString() const
        {
            return connectionName + (subscriptionName.isEmpty() ? "" : " (" + subscriptionName + ")");
        }
        friend bool operator==(ConnectionIdentifier &left, ConnectionIdentifier &right)
        {
            return left.subscriptionName == right.subscriptionName && left.connectionName == right.connectionName;
        }
        friend bool operator!=(ConnectionIdentifier &left, ConnectionIdentifier &right)
        {
            return !(left == right);
        }
        friend bool operator==(ConnectionIdentifier &left, QString &right)
        {
            return left.IdentifierString() == right;
        }
        friend bool operator!=(ConnectionIdentifier &left, QString &right)
        {
            return !(left.IdentifierString() == right);
        }
        // To make QMap happy
        friend bool operator<(const ConnectionIdentifier left, const ConnectionIdentifier right)
        {
            return left.IdentifierString() < right.IdentifierString();
        }
        friend bool operator>(const ConnectionIdentifier left, const ConnectionIdentifier right)
        {
            return left.IdentifierString() > right.IdentifierString();
        }
        XTOSTRUCT(O(subscriptionName, connectionName))
    };
}
Q_DECLARE_METATYPE(Qv2ray::base::ConnectionIdentifier);
