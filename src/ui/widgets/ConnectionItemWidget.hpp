#pragma once

#include "core/handler/ConfigHandler.hpp"
#include "ui_ConnectionItemWidget.h"

#include <QWidget>

enum ITEM_TYPE
{
    GROUP_HEADER_ITEM,
    NODE_ITEM
};

class ConnectionItemWidget
    : public QWidget
    , private Ui::ConnectionWidget
{
    Q_OBJECT
  public:
    explicit ConnectionItemWidget(const ConnectionId &connecionId, QWidget *parent = nullptr);
    explicit ConnectionItemWidget(const GroupId &groupId, QWidget *parent = nullptr);
    //
    void BeginConnection();
    ~ConnectionItemWidget();
    //
    void BeginRename();
    inline bool NameMatched(const QString &arg)
    {
        auto searchString = arg.toLower();
        auto headerMatched = GetDisplayName(groupId).toLower().contains(arg);

        if (itemType != NODE_ITEM)
        {
            return headerMatched;
        }
        else
        {
            return headerMatched || GetDisplayName(connectionId).toLower().contains(searchString);
        }
    }
    inline const tuple<GroupId, ConnectionId> Identifier() const
    {
        return { groupId, connectionId };
    }
    inline bool IsConnection() const
    {
        return itemType == NODE_ITEM;
    }
    void keyPressEvent(QKeyEvent *e) override;
  signals:
    void RequestWidgetFocus(const ConnectionItemWidget *me);
  private slots:
    void OnConnectionStatsArrived(const ConnectionId &id, const quint64 upS, const quint64 downS, const quint64 upD, const quint64 downD);
    void OnConnected(const ConnectionId &id);
    void OnDisConnected(const ConnectionId &id);
    void OnLatencyTestStart(const ConnectionId &id);
    void OnLatencyTestFinished(const ConnectionId &id, const uint average);
    inline void RecalculateConnectionsCount()
    {
        auto connectionCount = ConnectionManager->Connections(groupId).count();
        latencyLabel->setText(QSTRN(connectionCount) + " " + (connectionCount < 2 ? tr("connection") : tr("connections")));
    }
    void OnConnectionItemRenamed(const ConnectionId &id, const QString &, const QString &newName);
    void OnGroupItemRenamed(const GroupId &id, const QString &, const QString &newName);
    void on_doRenameBtn_clicked();

  private:
    explicit ConnectionItemWidget(QWidget *parent = nullptr);
    QString originalItemName;
    ITEM_TYPE itemType;
    ConnectionId connectionId;
    GroupId groupId;

    Q_DISABLE_COPY_MOVE(ConnectionItemWidget)
};
