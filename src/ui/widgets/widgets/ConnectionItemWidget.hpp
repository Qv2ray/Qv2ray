#pragma once

#include "base/models/QvConfigIdentifier.hpp"
#include "ui_ConnectionItemWidget.h"

#include <QWidget>

class ConnectionItemWidget
    : public QWidget
    , private Ui::ConnectionWidget
{
    Q_OBJECT
  public:
    explicit ConnectionItemWidget(const ConnectionGroupPair &id, QWidget *parent = nullptr);
    explicit ConnectionItemWidget(const GroupId &groupId, QWidget *parent = nullptr);
    //
    void BeginConnection();
    ~ConnectionItemWidget();
    //
    void BeginRename();
    void CancelRename();
    bool NameMatched(const QString &arg) const;
    inline const ConnectionGroupPair Identifier() const
    {
        return { this->connectionId, this->groupId };
    }
    inline bool IsRenaming() const
    {
        return stackedWidget->currentIndex() == 1;
    }
    inline bool IsConnection() const
    {
        return connectionId != NullConnectionId;
    }
  signals:
    void RequestWidgetFocus(const ConnectionItemWidget *me);
  private slots:
    void OnConnected(const ConnectionGroupPair &id);
    void OnDisConnected(const ConnectionGroupPair &id);
    void OnConnectionStatsArrived(const ConnectionGroupPair &id, const QMap<StatisticsType, QvStatsSpeedData> &data);
    void OnLatencyTestStart(const ConnectionId &id);
    void OnConnectionModified(const ConnectionId &id);
    void OnLatencyTestFinished(const ConnectionId &id, const int average);
    void RecalculateConnectionsCount();
    void OnConnectionItemRenamed(const ConnectionId &id, const QString &, const QString &newName);
    void OnGroupItemRenamed(const GroupId &id, const QString &, const QString &newName);
    void on_doRenameBtn_clicked();

  private:
    explicit ConnectionItemWidget(QWidget *parent = nullptr);
    QString originalItemName;
    ConnectionId connectionId;
    GroupId groupId;
};
