#include "w_GroupManager.hpp"

#include "common/QvHelpers.hpp"
#include "core/handler/ConfigHandler.hpp"
#include "core/settings/SettingsBackend.hpp"

SubscriptionEditor::SubscriptionEditor(QWidget *parent) : QDialog(parent)
{
    setupUi(this);
    QvMessageBusConnect(SubscriptionEditor);
    UpdateColorScheme();
    for (auto subs : ConnectionManager->Subscriptions())
    {
        groupList->addTopLevelItem(new QTreeWidgetItem(QStringList{ GetDisplayName(subs), subs.toString() }));
    }
    if (groupList->topLevelItemCount() > 0)
    {
        groupList->setCurrentItem(groupList->topLevelItem(0));
    }
}

void SubscriptionEditor::UpdateColorScheme()
{
    addSubsButton->setIcon(QICON_R("add.png"));
    removeSubsButton->setIcon(QICON_R("delete.png"));
}

QvMessageBusSlotImpl(SubscriptionEditor)
{
    switch (msg)
    {
        MBShowDefaultImpl MBHideDefaultImpl MBRetranslateDefaultImpl MBUpdateColorSchemeDefaultImpl
    }
}

tuple<QString, CONFIGROOT> SubscriptionEditor::GetSelectedConfig()
{
    return { GetDisplayName(currentConnectionId), ConnectionManager->GetConnectionRoot(currentConnectionId) };
}

SubscriptionEditor::~SubscriptionEditor()
{
}

void SubscriptionEditor::on_addSubsButton_clicked()
{
    auto const key = QSTRN(QTime::currentTime().msecsSinceStartOfDay());
    auto id = ConnectionManager->CreateGroup(key, true);
    //
    groupList->addTopLevelItem(new QTreeWidgetItem(QStringList{ key, id.toString() }));
}

void SubscriptionEditor::on_updateButton_clicked()
{
    if (QvMessageBoxAsk(this, tr("Reload Subscription"), tr("Would you like to reload the subscription?")) == QMessageBox::Yes)
    {
        this->setEnabled(false);
        ConnectionManager->UpdateSubscription(currentSubId); //
        this->setEnabled(true);
        on_groupList_itemClicked(groupList->currentItem(), 0);
    }
}

void SubscriptionEditor::on_removeSubsButton_clicked()
{
    if (QvMessageBoxAsk(this, tr("Deleting a subscription"), tr("All connections will be moved to default group, do you want to continue?")) ==
        QMessageBox::Yes)
    {
        ConnectionManager->DeleteGroup(currentSubId); //
        auto item = groupList->currentItem();
        groupList->removeItemWidget(item, 0);
        delete item;
        if (groupList->topLevelItemCount() > 0)
        {
            groupList->setCurrentItem(groupList->topLevelItem(0));
            on_groupList_itemClicked(groupList->topLevelItem(0), 0);
        }
        else
        {
            groupBox_2->setEnabled(false);
        }
    }
}

void SubscriptionEditor::on_buttonBox_accepted()
{
    // Nothing?
}

void SubscriptionEditor::on_groupList_itemSelectionChanged()
{
    groupBox_2->setEnabled(groupList->selectedItems().count() > 0);
}

void SubscriptionEditor::on_groupList_itemClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column)

    if (item == nullptr)
    {
        return;
    }

    //
    groupBox_2->setEnabled(true);
    currentSubId = GroupId(item->text(1));
    //
    groupNameTxt->setText(GetDisplayName(currentSubId));
    auto const [addr, lastUpdated, updateInterval] = ConnectionManager->GetSubscriptionData(currentSubId);
    subAddrTxt->setText(addr);
    lastUpdatedLabel->setText(timeToString(lastUpdated));
    updateIntervalSB->setValue(updateInterval);
    //
    connectionsList->clear();

    for (auto conn : ConnectionManager->Connections(currentSubId))
    {
        connectionsList->addItem(GetDisplayName(conn)); //
    }
}

void SubscriptionEditor::on_groupList_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    Q_UNUSED(previous)
    on_groupList_itemClicked(current, 0);
}

void SubscriptionEditor::on_subNameTxt_textEdited(const QString &arg1)
{
    groupList->selectedItems().first()->setText(0, arg1);
    ConnectionManager->RenameGroup(currentSubId, arg1.trimmed());
}

void SubscriptionEditor::on_subAddrTxt_textEdited(const QString &arg1)
{
    auto newUpdateInterval = updateIntervalSB->value();
    ConnectionManager->SetSubscriptionData(currentSubId, arg1, newUpdateInterval);
}

void SubscriptionEditor::on_updateIntervalSB_valueChanged(double arg1)
{
    auto newAddress = subAddrTxt->text().trimmed();
    ConnectionManager->SetSubscriptionData(currentSubId, newAddress, arg1);
}

void SubscriptionEditor::on_connectionsList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    Q_UNUSED(previous)
    if (current != nullptr)
    {
        currentConnectionId = ConnectionManager->GetConnectionIdByDisplayName(current->text(), currentSubId);
    }
}
