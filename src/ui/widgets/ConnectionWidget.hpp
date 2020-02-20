#pragma once

#include <QWidget>
#include "ui_ConnectionWidget.h"
#include "core/handler/ConnectionHandler.hpp"

enum ITEM_TYPE {
    GROUP_HEADER_ITEM,
    NODE_ITEM
};

class ConnectionWidget : public QWidget, private Ui::ConnectionWidget
{
        Q_OBJECT
    public:
        explicit ConnectionWidget(const ConnectionId &connecionId, QWidget *parent = nullptr);
        explicit ConnectionWidget(const GroupId &groupId, QWidget *parent = nullptr);
        //
        void BeginConnection();
        ~ConnectionWidget();
        //
        inline bool NameMatched(const QString &arg)
        {
            auto searchString = arg.toLower();
            auto headerMatched = ConnectionHandler->GetGroup(groupId).displayName.toLower().contains(arg);

            if (itemType != NODE_ITEM) {
                return headerMatched;
            } else {
                return headerMatched || ConnectionHandler->GetConnection(connectionId).displayName.toLower().contains(searchString);
            }
        }
        inline const tuple<GroupId, ConnectionId> Identifier() const
        {
            return make_tuple(groupId, connectionId);
        }
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
        ITEM_TYPE itemType;
        ConnectionId connectionId;
        GroupId groupId;

        Q_DISABLE_COPY_MOVE(ConnectionWidget)
};
