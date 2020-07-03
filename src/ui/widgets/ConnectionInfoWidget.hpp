#pragma once

#include "core/handler/ConfigHandler.hpp"
#include "ui/messaging/QvMessageBus.hpp"
#include "ui_ConnectionInfoWidget.h"

#include <QWidget>

class ConnectionInfoWidget
    : public QWidget
    , private Ui::ConnectionInfoWidget
{
    Q_OBJECT

  public:
    explicit ConnectionInfoWidget(QWidget *parent = nullptr);
    void ShowDetails(const tuple<GroupId, ConnectionId> &_identifier);
    ~ConnectionInfoWidget();

  signals:
    void OnEditRequested(const ConnectionId &id);
    void OnJsonEditRequested(const ConnectionId &id);

  protected:
    bool eventFilter(QObject *object, QEvent *event) override;

  private slots:
    void on_connectBtn_clicked();
    void on_editBtn_clicked();
    void on_editJsonBtn_clicked();
    void on_deleteBtn_clicked();

    void OnConnectionModified(const ConnectionId &id);
    void OnGroupRenamed(const GroupId &id, const QString &oldName, const QString &newName);
    void OnConnected(const ConnectionId &id);
    void OnDisConnected(const ConnectionId &id);
    void on_latencyBtn_clicked();

  private:
    void UpdateColorScheme();
    QvMessageBusSlotDecl;
    ConnectionId connectionId = NullConnectionId;
    GroupId groupId = NullGroupId;
    //
    bool isRealPixmapShown;
    QPixmap qrPixmap;
    QPixmap qrPixmapBlured;
};
