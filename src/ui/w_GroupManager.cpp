#include "w_GroupManager.hpp"

#include "common/QvHelpers.hpp"
#include "core/handler/ConfigHandler.hpp"
#include "core/settings/SettingsBackend.hpp"

GroupManager::GroupManager(QWidget *parent) : QDialog(parent)
{
    setupUi(this);
    QvMessageBusConnect(GroupManager);
    UpdateColorScheme();
    connectionListRCMenu->addMenu(connectionListRCMenu_CopyToMenu);
    connectionListRCMenu->addMenu(connectionListRCMenu_MoveToMenu);
    for (auto group : ConnectionManager->AllGroups())
    {
        auto item = new QListWidgetItem(GetDisplayName(group));
        item->setData(Qt::UserRole, group.toString());
        groupList->addItem(item);
    }
    if (groupList->count() > 0)
    {
        groupList->setCurrentItem(groupList->item(0));
    }
}

void GroupManager::UpdateColorScheme()
{
    addGroupButton->setIcon(QICON_R("add.png"));
    removeGroupButton->setIcon(QICON_R("delete.png"));
}

QvMessageBusSlotImpl(GroupManager)
{
    switch (msg)
    {
        MBShowDefaultImpl;
        MBHideDefaultImpl;
        MBRetranslateDefaultImpl;
        MBUpdateColorSchemeDefaultImpl
    }
}

tuple<QString, CONFIGROOT> GroupManager::GetSelectedConfig()
{
    return { GetDisplayName(currentConnectionId), ConnectionManager->GetConnectionRoot(currentConnectionId) };
}

GroupManager::~GroupManager()
{
}

void GroupManager::on_addGroupButton_clicked()
{
    auto const key = QSTRN(QTime::currentTime().msecsSinceStartOfDay());
    auto id = ConnectionManager->CreateGroup(key, true);
    //
    auto item = new QListWidgetItem(key);
    item->setData(Qt::UserRole, id.toString());
    groupList->addItem(item);
}

void GroupManager::on_updateButton_clicked()
{
    if (QvMessageBoxAsk(this, tr("Reload Subscription"), tr("Would you like to reload the subscription?")) == QMessageBox::Yes)
    {
        this->setEnabled(false);
        ConnectionManager->UpdateSubscription(currentSubId); //
        this->setEnabled(true);
        on_groupList_itemClicked(groupList->currentItem());
    }
}

void GroupManager::on_removeGroupButton_clicked()
{
    if (QvMessageBoxAsk(this, tr("Deleting a subscription"), tr("All connections will be moved to default group, do you want to continue?")) ==
        QMessageBox::Yes)
    {
        ConnectionManager->DeleteGroup(currentSubId); //
        auto item = groupList->currentItem();
        groupList->removeItemWidget(item);
        delete item;
        if (groupList->count() > 0)
        {
            groupList->setCurrentItem(groupList->item(0));
            on_groupList_itemClicked(groupList->item(0));
        }
        else
        {
            groupInfoGroupBox->setEnabled(false);
        }
    }
}

void GroupManager::on_buttonBox_accepted()
{
    // Nothing?
}

void GroupManager::on_groupList_itemSelectionChanged()
{
    groupInfoGroupBox->setEnabled(groupList->selectedItems().count() > 0);
}

void GroupManager::on_groupList_itemClicked(QListWidgetItem *item)
{

    if (item == nullptr)
    {
        return;
    }

    groupInfoGroupBox->setEnabled(true);
    currentSubId = GroupId(item->data(Qt::UserRole).toString());
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

void GroupManager::on_groupList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    Q_UNUSED(previous)
    on_groupList_itemClicked(current);
}

void GroupManager::on_subNameTxt_textEdited(const QString &arg1)
{
    groupList->selectedItems().first()->setText(arg1);
    ConnectionManager->RenameGroup(currentSubId, arg1.trimmed());
}

void GroupManager::on_subAddrTxt_textEdited(const QString &arg1)
{
    auto newUpdateInterval = updateIntervalSB->value();
    ConnectionManager->SetSubscriptionData(currentSubId, arg1, newUpdateInterval);
}

void GroupManager::on_updateIntervalSB_valueChanged(double arg1)
{
    auto newAddress = subAddrTxt->text().trimmed();
    ConnectionManager->SetSubscriptionData(currentSubId, newAddress, arg1);
}

void GroupManager::on_connectionsList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    Q_UNUSED(previous)
    if (current != nullptr)
    {
        currentConnectionId = ConnectionManager->GetConnectionIdByDisplayName(current->text(), currentSubId);
    }
}

void GroupManager::on_connectionsList_customContextMenuRequested(const QPoint &pos)
{
    Q_UNUSED(pos)
    connectionListRCMenu->popup(QCursor::pos());
}
