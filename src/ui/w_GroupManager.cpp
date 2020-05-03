#include "w_GroupManager.hpp"

#include "common/QvHelpers.hpp"
#include "core/connection/Generation.hpp"
#include "core/handler/ConfigHandler.hpp"
#include "core/settings/SettingsBackend.hpp"

#include <QFileDialog>
#include <QListWidgetItem>
#define GET_DATA(type, typeConv)                                                                                                                \
    [&](const QList<QListWidgetItem *> list) {                                                                                                  \
        QList<type> _list;                                                                                                                      \
        for (const auto &item : list)                                                                                                           \
        {                                                                                                                                       \
            _list.push_back(item->data(Qt::UserRole).to##typeConv());                                                                           \
        }                                                                                                                                       \
        return _list;                                                                                                                           \
    }
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
    //
    connect(exportConnectionAction, &QAction::triggered, this, &GroupManager::onRCMExportConnectionTriggered);
    connect(deleteConnectionAction, &QAction::triggered, this, &GroupManager::onRCMDeleteConnectionTriggered);
    //
    connect(ConnectionManager, &QvConfigHandler::OnConnectionGroupChanged, //
            [&](const ConnectionId &, const GroupId &, const GroupId &) {  //
                this->loadConnectionList(currentGroupId);                  //
            });
    //
    connect(ConnectionManager, &QvConfigHandler::OnConnectionCreated, [&](const ConnectionId &id) { //
        const auto groupId = GetConnectionGroupId(id);                                              //
        if (groupId == currentGroupId)                                                              //
            this->loadConnectionList(groupId);                                                      //
    });                                                                                             //
    connect(ConnectionManager, &QvConfigHandler::OnConnectionDeleted, [&](const ConnectionId &, const GroupId &group) {
        if (group == currentGroupId)         //
            this->loadConnectionList(group); //
    });                                      //
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

void GroupManager::onRCMDeleteConnectionTriggered()
{
    const auto list = GET_DATA(QString, String)(connectionsList->selectedItems());
    for (const auto &item : list)
    {
        ConnectionManager->DeleteConnection(ConnectionId(item));
    }
}

void GroupManager::onRCMExportConnectionTriggered()
{
    const auto list = GET_DATA(QString, String)(connectionsList->selectedItems());
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
        connectionsList->addItem(item);
    }
}

void GroupManager::onRCMActionTriggered_Copy()
{
    const auto _sender = qobject_cast<QAction *>(sender());
    const GroupId groupId{ _sender->data().toString() };
    //
    const auto list = GET_DATA(QString, String)(connectionsList->selectedItems());
    for (const auto &connId : list)
    {
        const auto &connectionId = ConnectionId(connId);
        ConnectionManager->CreateConnection(GetDisplayName(connectionId), groupId, ConnectionManager->GetConnectionRoot(connectionId), true);
    }
}

void GroupManager::onRCMActionTriggered_Move()
{
    const auto _sender = qobject_cast<QAction *>(sender());
    const GroupId groupId{ _sender->data().toString() };
    //
    const auto list = GET_DATA(QString, String)(connectionsList->selectedItems());
    for (const auto &connId : list)
    {
        ConnectionManager->MoveConnectionGroup(ConnectionId(connId), groupId);
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

std::tuple<QString, CONFIGROOT> GroupManager::GetSelectedConfig()
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
    subAddrTxt->setText(groupMetaObject.address);
    lastUpdatedLabel->setText(timeToString(groupMetaObject.lastUpdated));
    createdAtLabel->setText(timeToString(groupMetaObject.importDate));
    updateIntervalSB->setValue(groupMetaObject.updateInterval);
    //
    connectionsList->clear();
    loadConnectionList(currentGroupId);
}

void GroupManager::on_groupList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    Q_UNUSED(previous)
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

void GroupManager::on_connectionsList_customContextMenuRequested(const QPoint &pos)
{
    Q_UNUSED(pos)
    connectionListRCMenu->popup(QCursor::pos());
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
    if (connectionsList->selectedItems().isEmpty())
    {
        connectionsList->selectAll();
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

#undef GET_DATA
