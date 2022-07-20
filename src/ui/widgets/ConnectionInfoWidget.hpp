#pragma once

#include "MessageBus/MessageBus.hpp"
#include "QvPlugin/PluginInterface.hpp"
#include "ui_ConnectionInfoWidget.h"

#include <QWidget>

class TagsLineEdit;

class ConnectionInfoWidget
    : public QWidget
    , private Ui::ConnectionInfoWidget
{
    Q_OBJECT

  public:
    explicit ConnectionInfoWidget(QWidget *parent = nullptr);
    void ShowDetails(const ProfileId &_identifier);
    ~ConnectionInfoWidget();

  signals:
    void OnEditRequested(const ConnectionId &id);
    void OnJsonEditRequested(const ConnectionId &id);
    void OnTagSearchRequested(const QString &tag);

  protected:
    bool eventFilter(QObject *object, QEvent *event) override;

  private slots:
    void on_connectBtn_clicked();
    void on_editBtn_clicked();
    void on_editJsonBtn_clicked();
    void on_deleteBtn_clicked();
    void on_latencyBtn_clicked();

    void OnGroupRenamed(const GroupId &id, const QString &oldName, const QString &newName);
    void OnConnected(const ProfileId &id);
    void OnDisConnected(const ProfileId &id);
    void OnConnectionModified(const ConnectionId &id);
    void OnConnectionModified_Pair(const ProfileId &id);

  private:
    TagsLineEdit *tagsEditor;
    void updateColorScheme();
    QvMessageBusSlotDecl;
    ConnectionId connectionId;
    GroupId groupId;
    //
    bool isRealPixmapShown;
    QPixmap qrPixmap;
    QPixmap qrPixmapBlured;
};
