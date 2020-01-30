#pragma once
#include <QString>
#include <QtCore>
#include "3rdparty/x2struct/x2struct.hpp"

struct QvConfigIdentifier {
    QString subscriptionName;
    QString connectionName;
    QvConfigIdentifier() { };
    bool isEmpty()
    {
        return connectionName.isEmpty();
    }
    QvConfigIdentifier(QString connectionName)
    {
        this->connectionName = connectionName;
    }
    QvConfigIdentifier(QString connectionName, QString subscriptionName)
    {
        this->connectionName = connectionName;
        this->subscriptionName = subscriptionName;
    }
    const QString IdentifierString() const
    {
        return connectionName + (subscriptionName.isEmpty() ? "" : " (" + subscriptionName + ")");
    }
    friend bool operator==(QvConfigIdentifier &left, QvConfigIdentifier &right)
    {
        return left.subscriptionName == right.subscriptionName && left.connectionName == right.connectionName;
    }
    friend bool operator!=(QvConfigIdentifier &left, QvConfigIdentifier &right)
    {
        return !(left == right);
    }
    friend bool operator==(QvConfigIdentifier &left, QString &right)
    {
        return left.IdentifierString() == right;
    }
    friend bool operator!=(QvConfigIdentifier &left, QString &right)
    {
        return !(left.IdentifierString() == right);
    }
    // To make QMap happy
    friend bool operator<(const QvConfigIdentifier left, const QvConfigIdentifier right)
    {
        return left.IdentifierString() < right.IdentifierString();
    }
    friend bool operator>(const QvConfigIdentifier left, const QvConfigIdentifier right)
    {
        return left.IdentifierString() > right.IdentifierString();
    }
    XTOSTRUCT(O(subscriptionName, connectionName))
};

Q_DECLARE_METATYPE(QvConfigIdentifier);
