#include "w_GroupManager.hpp"

#include "common/QvHelpers.hpp"
#include "core/handler/ConfigHandler.hpp"
#include "core/settings/SettingsBackend.hpp"

#include <QListWidgetItem>

GroupManager::GroupManager(QWidget *parent) : QDialog(parent)
{
    setupUi(this);
    QvMessageBusConnect(GroupManager);
    UpdateColorScheme();
    connectionListRCMenu->addMenu(connectionListRCMenu_CopyToMenu);
    connectionListRCMenu->addMenu(connectionListRCMenu_MoveToMenu);
    //
    connect(ConnectionManager, &QvConfigHandler::OnConnectionGroupChanged, //
            [&](const ConnectionId &, const GroupId &, const GroupId &) {  //
                this->loadConnectionList(currentGroupId);                  //
            });
    //
    connect(ConnectionManager, &QvConfigHandler::OnConnectionCreated, //
            [&](const ConnectionId &id, const QString &) {            //
                const auto groupId = GetConnectionGroupId(id);        //
                if (groupId == currentGroupId)                        //
                    this->loadConnectionList(groupId);                //
            });                                                       //
    //
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
    ReloadGroupAction();
}

void GroupManager::ReloadGroupAction()
{
    connectionListRCMenu_CopyToMenu->clear();
    connectionListRCMenu_MoveToMenu->clear();
    for (const auto &group : ConnectionManager->AllGroups())
    {
        auto cpAction = new QAction(GetDisplayName(group), connectionListRCMenu_CopyToMenu);
        auto mvAction = new QAction(GetDisplayName(group), connectionListRCMenu_MoveToMenu);
        //
        cpAction->setData(group.toString());
        mvAction->setData(group.toString());
        //
        connectionListRCMenu_CopyToMenu->addAction(cpAction);
        connectionListRCMenu_MoveToMenu->addAction(mvAction);
        //
        connect(cpAction, &QAction::triggered, this, &GroupManager::onRCMActionTriggered_Copy);
        connect(mvAction, &QAction::triggered, this, &GroupManager::onRCMActionTriggered_Move);
    }
}

void GroupManager::loadConnectionList(const GroupId &group)
{
    connectionsList->clear();
    for (auto conn : ConnectionManager->Connections(group))
    {
        auto item = new QListWidgetItem(GetDisplayName(conn), connectionsList);
        item->setData(Qt::UserRole, conn.toString());
        connectionsList->addItem(item); //
    }
}

void GroupManager::onRCMActionTriggered_Copy()
{
    const auto _sender = qobject_cast<QAction *>(sender());
    const GroupId groupId{ _sender->data().toString() };
    //
    const auto &connectionList = connectionsList->selectedItems();
    for (const auto &connItem : connectionList)
    {
        const auto &connectionId = ConnectionId(connItem->data(Qt::UserRole).toString());
        ConnectionManager->CreateConnection(GetDisplayName(connectionId), groupId, ConnectionManager->GetConnectionRoot(connectionId), true);
    }
}
void GroupManager::onRCMActionTriggered_Move()
{
    const auto _sender = qobject_cast<QAction *>(sender());
    const GroupId groupId{ _sender->data().toString() };
    //
    const auto &connectionList = connectionsList->selectedItems();
    for (const auto &connItem : connectionList)
    {
        if (!connItem)
        {
            LOG(MODULE_UI, "Invalid?")
            continue;
        }
        const auto &id = connItem->data(Qt::UserRole);
        ConnectionManager->MoveConnectionGroup(ConnectionId(id.toString()), groupId);
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
        ConnectionManager->UpdateSubscription(currentGroupId);
        this->setEnabled(true);
        on_groupList_itemClicked(groupList->currentItem());
    }
}

void GroupManager::on_removeGroupButton_clicked()
{
    if (QvMessageBoxAsk(this, tr("Deleting a subscription"), tr("All connections will be moved to default group, do you want to continue?")) ==
        QMessageBox::Yes)
    {
        ConnectionManager->DeleteGroup(currentGroupId); //
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
    currentGroupId = GroupId(item->data(Qt::UserRole).toString());
    //
    groupNameTxt->setText(GetDisplayName(currentGroupId));
    auto const [addr, lastUpdated, updateInterval] = ConnectionManager->GetSubscriptionData(currentGroupId);
    subAddrTxt->setText(addr);
    lastUpdatedLabel->setText(timeToString(lastUpdated));
    updateIntervalSB->setValue(updateInterval);
    //
    connectionsList->clear();
    loadConnectionList(currentGroupId);
}

void GroupManager::on_groupList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    Q_UNUSED(previous)
    on_groupList_itemClicked(current);
}

void GroupManager::on_subNameTxt_textEdited(const QString &arg1)
{
    groupList->selectedItems().first()->setText(arg1);
    ConnectionManager->RenameGroup(currentGroupId, arg1.trimmed());
}

void GroupManager::on_subAddrTxt_textEdited(const QString &arg1)
{
    auto newUpdateInterval = updateIntervalSB->value();
    ConnectionManager->SetSubscriptionData(currentGroupId, arg1, newUpdateInterval);
}

void GroupManager::on_updateIntervalSB_valueChanged(double arg1)
{
    auto newAddress = subAddrTxt->text().trimmed();
    ConnectionManager->SetSubscriptionData(currentGroupId, newAddress, arg1);
}

void GroupManager::on_connectionsList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    Q_UNUSED(previous)
    if (current != nullptr)
    {
        currentConnectionId = ConnectionId(current->data(Qt::UserRole).toString());
    }
}

void GroupManager::on_connectionsList_customContextMenuRequested(const QPoint &pos)
{
    Q_UNUSED(pos)
    connectionListRCMenu->popup(QCursor::pos());
}
