#pragma once

#include <QWidget>
#include "ui_ConnectionWidget.h"
#include "core/handler/ConnectionHandler.hpp"

enum ITEM_TYPE {
    GROUP_HEADER_ITEM,
    SUBS_HEADER_ITEM,
    NODE_ITEM
};

class ConnectionWidget : public QWidget, private Ui::ConnectionWidget
{
        Q_OBJECT
    public:
        //
        // ======================================= Initialisation for connection nodes.
        explicit ConnectionWidget(const ConnectionIdentifier &identifier, QWidget *parent = nullptr);
        //
        // ======================================= Initialisation for root nodes.
        explicit ConnectionWidget(const GroupId &id, QWidget *parent = nullptr);
        explicit ConnectionWidget(const SubscriptionId &id, QWidget *parent = nullptr);
        //
        void BeginConnection();
        ~ConnectionWidget();
        //
        inline bool NameMatched(const QString &arg)
        {
            auto searchString = arg.toLower();
            auto matchHeader = [&](const QString & arg1) {
                return itemType == SUBS_HEADER_ITEM
                       ? ConnectionHandler->GetSubscription(subscriptionId).displayName.toLower().contains(arg1)
                       : ConnectionHandler->GetGroup(groupId).displayName.toLower().contains(arg1);
            };

            if (itemType != NODE_ITEM) {
                return matchHeader(searchString);
            } else {
                return matchHeader(searchString) || ConnectionHandler->GetConnection(connectionIdentifier.connectionId).displayName.toLower().contains(searchString);
            }
        }
        inline const ConnectionIdentifier Identifier() const
        {
            return connectionIdentifier;
        }
        //
        inline bool IsConnection() const
        {
            return itemType == NODE_ITEM;
        }
    signals:
        void RequestWidgetFocus(const ConnectionWidget *me);
    private slots:
        void OnConnected(const ConnectionId &id);
    private:
        explicit ConnectionWidget(QWidget *parent = nullptr);
        void InitialiseForGroup(const QString &displayName, int connectionCount);
        QString rawDisplayName;
        //
        ITEM_TYPE itemType;
        //
        ConnectionIdentifier connectionIdentifier;
        GroupId groupId;
        SubscriptionId subscriptionId;

        Q_DISABLE_COPY_MOVE(ConnectionWidget)
};
