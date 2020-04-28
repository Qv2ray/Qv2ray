#pragma once

#include "base/Qv2rayBase.hpp"
#include "core/CoreSafeTypes.hpp"
#include "ui/messaging/QvMessageBus.hpp"
#include "ui_w_GroupManager.h"

#include <QDialog>

class SubscriptionEditor
    : public QDialog
    , private Ui::w_GroupManager
{
    Q_OBJECT

  public:
    explicit SubscriptionEditor(QWidget *parent = nullptr);
    ~SubscriptionEditor();
    tuple<QString, CONFIGROOT> GetSelectedConfig();

  private:
    QvMessageBusSlotDecl;

  private slots:
    void on_addSubsButton_clicked();

    void on_updateButton_clicked();

    void on_removeSubsButton_clicked();

    void on_buttonBox_accepted();

    void on_groupList_itemSelectionChanged();

    void on_groupList_itemClicked(QTreeWidgetItem *item, int column);

    void on_groupList_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

    void on_subNameTxt_textEdited(const QString &arg1);

    void on_subAddrTxt_textEdited(const QString &arg1);

    void on_updateIntervalSB_valueChanged(double arg1);

    void on_connectionsList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

  private:
    void UpdateColorScheme();
    bool isUpdateInProgress = false;
    GroupId currentSubId = NullGroupId;
    ConnectionId currentConnectionId = NullConnectionId;
};
