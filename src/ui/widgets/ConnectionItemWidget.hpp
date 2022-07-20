#pragma once

#include "QvPlugin/Common/CommonTypes.hpp"
#include "QvPlugin/Handlers/LatencyTestHandler.hpp"
#include "ui_ConnectionItemWidget.h"

#include <QWidget>

class ConnectionItemWidget
    : public QWidget
    , private Ui::ConnectionWidget
{
    Q_OBJECT
  public:
    explicit ConnectionItemWidget(const ProfileId &id, QWidget *parent = nullptr);
    explicit ConnectionItemWidget(const GroupId &groupId, QWidget *parent = nullptr);
    ~ConnectionItemWidget() = default;

    void BeginConnection() const;
    void BeginRename();
    void CancelRename();

    inline const ProfileId Profile() const
    {
        return { this->connectionId, this->groupId };
    }
    inline bool IsRenaming() const
    {
        return stackedWidget->currentIndex() == 1;
    }
    inline bool IsConnection() const
    {
        return !connectionId.isNull();
    }

  private slots:
    void OnConnected(const ProfileId &id);
    void OnDisConnected(const ProfileId &id);
    void OnConnectionStatsArrived(const ProfileId &id, const StatisticsObject &data);
    void OnLatencyTestStart(const ConnectionId &id);
    void OnConnectionModified(const ConnectionId &id);
    void OnLatencyTestFinished(const ConnectionId &id, const Qv2rayPlugin::Latency::LatencyTestResponse &average);
    void RecalculateConnections();
    void OnConnectionItemRenamed(const ConnectionId &id, const QString &, const QString &newName);
    void OnGroupItemRenamed(const GroupId &id, const QString &, const QString &newName);
    void on_doRenameBtn_clicked();

  private:
    explicit ConnectionItemWidget(QWidget *parent = nullptr);
    QString originalItemName;
    ConnectionId connectionId;
    GroupId groupId;
};
