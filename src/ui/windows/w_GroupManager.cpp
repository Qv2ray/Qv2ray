#include "w_GroupManager.hpp"

#include "common/QvHelpers.hpp"
#include "core/connection/Generation.hpp"
#include "core/handler/ConfigHandler.hpp"
#include "core/settings/SettingsBackend.hpp"

#include <QFileDialog>
#include <QListWidgetItem>

#define SELECTED_ROWS_INDEX                                                                                                                     \
    ([&]() {                                                                                                                                    \
        const auto &__selection = connectionsTable->selectedItems();                                                                            \
        QSet<int> rows;                                                                                                                         \
        for (const auto &selection : __selection)                                                                                               \
        {                                                                                                                                       \
            rows.insert(connectionsTable->row(selection));                                                                                      \
        }                                                                                                                                       \
        return rows;                                                                                                                            \
    }())

#define GET_SELECTED_CONNECTION_IDS(connectionIdList)                                                                                           \
    ([&]() {                                                                                                                                    \
        QList<ConnectionId> _list;                                                                                                              \
        for (const auto &i : connectionIdList)                                                                                                  \
        {                                                                                                                                       \
            _list.push_back(ConnectionId(connectionsTable->item(i, 0)->data(Qt::UserRole).toString()));                                         \
        }                                                                                                                                       \
        return _list;                                                                                                                           \
    }())
GroupManager::GroupManager(QWidget *parent) : QDialog(parent)
{
    setupUi(this);
    QvMessageBusConnect(GroupManager);
    UpdateColorScheme();
    connectionListRCMenu->addSection(tr("Connection Management"));
    connectionListRCMenu->addAction(exportConnectionAction);
    connectionListRCMenu->addAction(deleteConnectionAction);
    connectionListRCMenu->addSeparator();
    connectionListRCMenu->addMenu(connectionListRCMenu_CopyToMenu);
    connectionListRCMenu->addMenu(connectionListRCMenu_MoveToMenu);
    connectionListRCMenu->addMenu(connectionListRCMenu_LinkToMenu);
    //
    connect(exportConnectionAction, &QAction::triggered, this, &GroupManager::onRCMExportConnectionTriggered);
    connect(deleteConnectionAction, &QAction::triggered, this, &GroupManager::onRCMDeleteConnectionTriggered);
    //
    connect(ConnectionManager, &QvConfigHandler::OnConnectionLinkedWithGroup, //
            [&]() {                                                           //
                this->reloadConnectionsList(currentGroupId);                  //
            });
    //
    connect(ConnectionManager, &QvConfigHandler::OnGroupCreated, this, &GroupManager::reloadGroupRCMActions);
    connect(ConnectionManager, &QvConfigHandler::OnGroupDeleted, this, &GroupManager::reloadGroupRCMActions);
    connect(ConnectionManager, &QvConfigHandler::OnGroupRenamed, this, &GroupManager::reloadGroupRCMActions);
    //
    for (const auto &group : ConnectionManager->AllGroups())
    {
        auto item = new QListWidgetItem(GetDisplayName(group));
        item->setData(Qt::UserRole, group.toString());
        groupList->addItem(item);
    }
    if (groupList->count() > 0)
    {
        groupList->setCurrentItem(groupList->item(0));
        on_groupList_itemClicked(groupList->item(0));
    }
    else
    {
        groupInfoGroupBox->setEnabled(false);
        tabWidget->setEnabled(false);
    }
    reloadGroupRCMActions();
}

void GroupManager::onRCMDeleteConnectionTriggered()
{
    const auto list = GET_SELECTED_CONNECTION_IDS(SELECTED_ROWS_INDEX);
    for (const auto &item : list)
    {
        ConnectionManager->RemoveConnectionFromGroup(ConnectionId(item), currentGroupId, true);
    }
    reloadConnectionsList(currentGroupId);
}

void GroupManager::onRCMExportConnectionTriggered()
{
    const auto &list = GET_SELECTED_CONNECTION_IDS(SELECTED_ROWS_INDEX);
    QFileDialog d;
    switch (list.count())
    {
        case 0: return;
        case 1:
        {
            const auto id = ConnectionId(list.first());
            auto filePath = d.getSaveFileName(this, GetDisplayName(id));
            if (filePath.isEmpty())
                return;
            auto root = GenerateRuntimeConfig(ConnectionManager->GetConnectionRoot(id));
            //
            // Apply export filter
            ExportConnectionFilter(root);
            //
            if (filePath.endsWith(".json"))
            {
                filePath += ".json";
            }
            //
            QFile file(filePath);
            StringToFile(JsonToString(root), file);
            QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(file).absoluteDir().absolutePath()));
            break;
        }
        default:
        {
            const auto path = d.getExistingDirectory();
            if (path.isEmpty())
                return;
            for (const auto &connId : list)
            {
                ConnectionId id(connId);
                auto root = GenerateRuntimeConfig(ConnectionManager->GetConnectionRoot(id));
                //
                // Apply export filter
                ExportConnectionFilter(root);
                //
                const auto fileName = RemoveInvalidFileName(GetDisplayName(id)) + ".json";
                QFile file(path + "/" + fileName);
                StringToFile(JsonToString(root), file);
            }
            QDesktopServices::openUrl(QUrl::fromLocalFile(path));
            break;
        }
    }
}

void GroupManager::reloadGroupRCMActions()
{
    connectionListRCMenu_CopyToMenu->clear();
    connectionListRCMenu_MoveToMenu->clear();
    for (const auto &group : ConnectionManager->AllGroups())
    {
        auto cpAction = new QAction(GetDisplayName(group), connectionListRCMenu_CopyToMenu);
        auto mvAction = new QAction(GetDisplayName(group), connectionListRCMenu_MoveToMenu);
        auto lnAction = new QAction(GetDisplayName(group), connectionListRCMenu_LinkToMenu);
        //
        cpAction->setData(group.toString());
        mvAction->setData(group.toString());
        lnAction->setData(group.toString());
        //
        connectionListRCMenu_CopyToMenu->addAction(cpAction);
        connectionListRCMenu_MoveToMenu->addAction(mvAction);
        connectionListRCMenu_LinkToMenu->addAction(lnAction);
        //
        connect(cpAction, &QAction::triggered, this, &GroupManager::onRCMActionTriggered_Copy);
        connect(mvAction, &QAction::triggered, this, &GroupManager::onRCMActionTriggered_Move);
        connect(lnAction, &QAction::triggered, this, &GroupManager::onRCMActionTriggered_Link);
    }
}

void GroupManager::reloadConnectionsList(const GroupId &group)
{
    connectionsTable->clearContents();
    connectionsTable->model()->removeRows(0, connectionsTable->rowCount());
    const auto &connections = ConnectionManager->Connections(group);
    for (auto i = 0; i < connections.count(); i++)
    {
        const auto &conn = connections.at(i);
        connectionsTable->insertRow(i);
        auto displayNameItem = new QTableWidgetItem(GetDisplayName(conn));
        displayNameItem->setData(Qt::UserRole, conn.toString());
        auto typeItem = new QTableWidgetItem(GetConnectionProtocolString(conn));
        const auto [type, host, port] = GetConnectionInfo(conn);
        auto hostPortItem = new QTableWidgetItem(host + ":" + QSTRN(port));
        //
        QStringList groupsNamesString;
        for (const auto &group : ConnectionManager->GetGroupId(conn))
        {
            groupsNamesString.append(GetDisplayName(group));
        }
        auto groupsItem = new QTableWidgetItem(groupsNamesString.join(";"));
        connectionsTable->setItem(i, 0, displayNameItem);
        connectionsTable->setItem(i, 1, typeItem);
        connectionsTable->setItem(i, 2, hostPortItem);
        connectionsTable->setItem(i, 3, groupsItem);
        //
        connectionsTable->resizeColumnsToContents();
    }
}

void GroupManager::onRCMActionTriggered_Copy()
{
    const auto _sender = qobject_cast<QAction *>(sender());
    const GroupId groupId{ _sender->data().toString() };
    //
    const auto list = GET_SELECTED_CONNECTION_IDS(SELECTED_ROWS_INDEX);
    for (const auto &connId : list)
    {
        const auto &connectionId = ConnectionId(connId);
        ConnectionManager->CreateConnection(ConnectionManager->GetConnectionRoot(connectionId), GetDisplayName(connectionId), groupId, true);
    }
    reloadConnectionsList(currentGroupId);
}

void GroupManager::onRCMActionTriggered_Link()
{
    const auto _sender = qobject_cast<QAction *>(sender());
    const GroupId groupId{ _sender->data().toString() };
    //
    const auto list = GET_SELECTED_CONNECTION_IDS(SELECTED_ROWS_INDEX);
    for (const auto &connId : list)
    {
        ConnectionManager->LinkConnectionWithGroup(ConnectionId(connId), groupId, false);
    }
    reloadConnectionsList(currentGroupId);
}

void GroupManager::onRCMActionTriggered_Move()
{
    const auto _sender = qobject_cast<QAction *>(sender());
    const GroupId groupId{ _sender->data().toString() };
    //
    const auto list = GET_SELECTED_CONNECTION_IDS(SELECTED_ROWS_INDEX);
    for (const auto &connId : list)
    {
        ConnectionManager->MoveConnectionFromToGroup(ConnectionId(connId), currentGroupId, groupId, false);
    }
    reloadConnectionsList(currentGroupId);
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

std::tuple<QString, CONFIGROOT> GroupManager::GetSelectedConfig()
{
    return { GetDisplayName(currentConnectionId), ConnectionManager->GetConnectionRoot(currentConnectionId) };
}

GroupManager::~GroupManager()
{
}

void GroupManager::on_addGroupButton_clicked()
{
    auto const key = tr("New Group") + " - " + GenerateRandomString(5);
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
        ConnectionManager->DeleteGroup(currentGroupId);
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
            tabWidget->setEnabled(false);
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
    const auto &groupMetaObject = ConnectionManager->GetGroupMetaObject(currentGroupId);
    groupIsSubscriptionGroup->setChecked(groupMetaObject.isSubscription);
    subAddrTxt->setText(groupMetaObject.subscriptionOption.address);
    lastUpdatedLabel->setText(timeToString(groupMetaObject.lastUpdatedDate));
    createdAtLabel->setText(timeToString(groupMetaObject.creationDate));
    updateIntervalSB->setValue(groupMetaObject.subscriptionOption.updateInterval);
    //
    reloadConnectionsList(currentGroupId);
}

void GroupManager::on_groupList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *)
{
    on_groupList_itemClicked(current);
}

void GroupManager::on_subAddrTxt_textEdited(const QString &arg1)
{
    auto newUpdateInterval = updateIntervalSB->value();
    ConnectionManager->SetSubscriptionData(currentGroupId, true, arg1, newUpdateInterval);
}

void GroupManager::on_updateIntervalSB_valueChanged(double arg1)
{
    auto newAddress = subAddrTxt->text().trimmed();
    ConnectionManager->SetSubscriptionData(currentGroupId, true, newAddress, arg1);
}

void GroupManager::on_connectionsList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    Q_UNUSED(previous)
    if (current != nullptr)
    {
        currentConnectionId = ConnectionId(current->data(Qt::UserRole).toString());
    }
}

void GroupManager::on_groupIsSubscriptionGroup_clicked(bool checked)
{
    ConnectionManager->SetSubscriptionData(currentGroupId, checked);
}

void GroupManager::on_groupNameTxt_textEdited(const QString &arg1)
{
    groupList->selectedItems().first()->setText(arg1);
    ConnectionManager->RenameGroup(currentGroupId, arg1.trimmed());
}

void GroupManager::on_deleteSelectedConnBtn_clicked()
{
    onRCMDeleteConnectionTriggered();
}

void GroupManager::on_exportSelectedConnBtn_clicked()
{
    if (connectionsTable->selectedItems().isEmpty())
    {
        connectionsTable->selectAll();
    }
    onRCMExportConnectionTriggered();
}

void GroupManager::ExportConnectionFilter(CONFIGROOT &root)
{
    root.remove("api");
    QJsonArray inbounds = root["inbounds"].toArray();
    for (int i = root["inbounds"].toArray().count() - 1; i >= 0; i--)
    {
        auto obj = root["inbounds"].toArray().at(i).toObject();
        if (obj["tag"] == API_TAG_INBOUND)
        {
            inbounds.removeAt(i);
        }
    }
    root["inbounds"] = inbounds;
}

#undef GET_SELECTED_CONNECTION_IDS

void GroupManager::on_connectionsTable_customContextMenuRequested(const QPoint &pos)
{
    Q_UNUSED(pos)
    connectionListRCMenu->popup(QCursor::pos());
}
