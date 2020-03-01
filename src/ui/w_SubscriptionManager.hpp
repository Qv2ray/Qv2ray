#pragma once

#include "base/Qv2rayBase.hpp"
#include "core/CoreSafeTypes.hpp"
#include "ui/messaging/QvMessageBus.hpp"
#include "ui_w_SubscriptionManager.h"

#include <QDialog>

class SubscribeEditor
    : public QDialog
    , private Ui::w_SubscribeEditor
{
    Q_OBJECT

  public:
    explicit SubscribeEditor(QWidget *parent = nullptr);
    ~SubscribeEditor();
    tuple<QString, CONFIGROOT> GetSelectedConfig();

  public slots:
    QvMessageBusSlotDecl;

  private slots:
    void on_addSubsButton_clicked();

    void on_updateButton_clicked();

    void on_removeSubsButton_clicked();

    void on_buttonBox_accepted();

    void on_subscriptionList_itemSelectionChanged();

    void on_subscriptionList_itemClicked(QTreeWidgetItem *item, int column);

    void on_subscriptionList_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

    void on_subNameTxt_textEdited(const QString &arg1);

    void on_subAddrTxt_textEdited(const QString &arg1);

    void on_updateIntervalSB_valueChanged(double arg1);

    void on_connectionsList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

  private:
    bool isUpdateInProgress = false;
    GroupId currentSubId = NullGroupId;
    ConnectionId currentConnectionId = NullConnectionId;
};
