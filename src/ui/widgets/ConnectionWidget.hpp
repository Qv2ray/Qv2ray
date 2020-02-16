#pragma once

#include <QWidget>
#include "ui_ConnectionWidget.h"
#include "core/handler/ConnectionHandler.hpp"

enum CONNECTION_ITEM_TYPE {
    CONNECTION,
    GROUP,
    SUBSCRIPTION
};

class ConnectionWidget : public QWidget, private Ui::ConnectionWidget
{
        Q_OBJECT
    public:
        CONNECTION_ITEM_TYPE type;
        explicit ConnectionWidget(const ConnectionId &id, QWidget *parent = nullptr): ConnectionWidget(parent)
        {
            type = CONNECTION;
            connectionId = id;
            //
            auto connection = connectionHandler->GetConnection(id);
            connNameLabel->setText(connection.displayName);
            latencyLabel->setText(tr("Latency: ") + QSTRN(connection.latency) + " " + tr("ms"));
        }
        explicit ConnectionWidget(const GroupId &id, QWidget *parent = nullptr) : ConnectionWidget(parent)
        {
            type = GROUP;
            groupId = id;
            InitialiseForGroups(connectionHandler->GetGroup(id).displayName, connectionHandler->Connections(id).count());
        }
        explicit ConnectionWidget(const SubscriptionId &id, QWidget *parent = nullptr) : ConnectionWidget(parent)
        {
            type = SUBSCRIPTION;
            subscriptionId = id;
            InitialiseForGroups(connectionHandler->GetSubscription(id).displayName, connectionHandler->Connections(id).count());
        }
        ~ConnectionWidget();
        Q_DISABLE_COPY_MOVE(ConnectionWidget)

    private:
        ConnectionWidget(QWidget *parent = nullptr) : QWidget(parent), connectionId("null"), groupId("null"), subscriptionId("null")
        {
            setupUi(this);
        }
        void InitialiseForGroups(const QString &displayName, int connectionCount)
        {
            connNameLabel->setText(tr("Group: ") + displayName);
            latencyLabel->setText(QSTRN(connectionCount) + " " + (connectionCount < 2 ? tr("connection") : tr("connections")));
            //
            layout()->removeWidget(connTypeLabel);
            delete connTypeLabel;
            layout()->removeWidget(dataLabel);
            delete dataLabel;
        }
        //
        ConnectionId connectionId;
        GroupId groupId;
        SubscriptionId subscriptionId;
};
