#pragma once

#include <QWidget>
#include "ui_ConnectionWidget.h"
#include "core/handler/ConnectionHandler.hpp"

enum CONNECTION_ITEM_TYPE {
    INVALID,
    GROUP,
    SUBSCRIPTION
};

class ConnectionWidget : public QWidget, private Ui::ConnectionWidget
{
        Q_OBJECT
    public:
        //
        // ======================================= Initialisation for connection nodes.
        //
        explicit ConnectionWidget(const GroupId &group, const ConnectionId &id, QWidget *parent = nullptr): ConnectionWidget(parent)
        {
            type = GROUP;
            connectionId = id;
            groupId = group;
            InitialiseForConnection(id);
        }
        explicit ConnectionWidget(const SubscriptionId &group, const ConnectionId &id, QWidget *parent = nullptr): ConnectionWidget(parent)
        {
            type = SUBSCRIPTION;
            connectionId = id;
            subscriptionId = group;
            InitialiseForConnection(id);
        }
        //
        // ======================================= Initialisation for root nodes.
        //
        explicit ConnectionWidget(const GroupId &id, QWidget *parent = nullptr) : ConnectionWidget(parent)
        {
            type = INVALID;
            groupId = id;
            InitialiseForGroup(ConnectionHandler->GetGroup(id).displayName, ConnectionHandler->Connections(id).count());
        }
        explicit ConnectionWidget(const SubscriptionId &id, QWidget *parent = nullptr) : ConnectionWidget(parent)
        {
            type = INVALID;
            subscriptionId = id;
            InitialiseForGroup(ConnectionHandler->GetSubscription(id).displayName, ConnectionHandler->Connections(id).count());
        }
        inline bool IsConnection() const
        {
            return isConnectionItem;
        }
        void BeginConnection()
        {
            if (isConnectionItem) {
                if (type == GROUP) {
                    ConnectionHandler->StartConnection(groupId, connectionId);
                } else {
                    ConnectionHandler->StartConnection(subscriptionId, connectionId);
                }
            } else {
                LOG(MODULE_UI, "Trying to start a non-connection entry, this call is illegal.")
            }
        }
        ~ConnectionWidget()
        {
            //
        }

    private:
        ConnectionWidget(QWidget *parent = nullptr) : QWidget(parent), connectionId("null"), groupId("null"), subscriptionId("null")
        {
            setupUi(this);
        }
        void InitialiseForConnection(const ConnectionId &id)
        {
            isConnectionItem = true;
            auto connection = ConnectionHandler->GetConnection(id);
            rawDisplayName = connection.displayName;
            connNameLabel->setText(rawDisplayName);
            latencyLabel->setText(tr("Latency: ") + QSTRN(connection.latency) + " " + tr("ms"));
        }
        void InitialiseForGroup(const QString &displayName, int connectionCount)
        {
            rawDisplayName = displayName;
            connNameLabel->setText(rawDisplayName);
            latencyLabel->setText(QSTRN(connectionCount) + " " + (connectionCount < 2 ? tr("connection") : tr("connections")));
            //
            layout()->removeWidget(connTypeLabel);
            delete connTypeLabel;
            layout()->removeWidget(dataLabel);
            delete dataLabel;
        }
        CONNECTION_ITEM_TYPE type;
        bool isConnectionItem = false;
        QString rawDisplayName;
        //
        ConnectionId connectionId;
        GroupId groupId;
        SubscriptionId subscriptionId;
        
        Q_DISABLE_COPY_MOVE(ConnectionWidget)
};
