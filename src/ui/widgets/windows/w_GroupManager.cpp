#include "w_GroupManager.hpp"

#include "core/connection/Generation.hpp"
#include "core/handler/ConfigHandler.hpp"
#include "core/handler/RouteHandler.hpp"
#include "core/settings/SettingsBackend.hpp"
#include "ui/widgets/widgets/DnsSettingsWidget.hpp"
#include "ui/widgets/widgets/RouteSettingsMatrix.hpp"
#include "utils/QvHelpers.hpp"

#include <QDesktopServices>
#include <QFileDialog>
#include <QListWidgetItem>

#define SELECTED_ROWS_INDEX                                                                                                                          \
    ([&]() {                                                                                                                                         \
        const auto &__selection = connectionsTable->selectedItems();                                                                                 \
        QSet<int> rows;                                                                                                                              \
        for (const auto &selection : __selection)                                                                                                    \
        {                                                                                                                                            \
            rows.insert(connectionsTable->row(selection));                                                                                           \
        }                                                                                                                                            \
        return rows;                                                                                                                                 \
    }())

#define GET_SELECTED_CONNECTION_IDS(connectionIdList)                                                                                                \
    ([&]() {                                                                                                                                         \
        QList<ConnectionId> _list;                                                                                                                   \
        for (const auto &i : connectionIdList)                                                                                                       \
        {                                                                                                                                            \
            _list.push_back(ConnectionId(connectionsTable->item(i, 0)->data(Qt::UserRole).toString()));                                              \
        }                                                                                                                                            \
        return _list;                                                                                                                                \
    }())

GroupManager::GroupManager(QWidget *parent) : QvDialog("GroupManager", parent)
{
    addStateOptions("width", { [&] { return width(); }, [&](QJsonValue val) { resize(val.toInt(), size().height()); } });
    addStateOptions("height", { [&] { return height(); }, [&](QJsonValue val) { resize(size().width(), val.toInt()); } });
    addStateOptions("x", { [&] { return x(); }, [&](QJsonValue val) { move(val.toInt(), y()); } });
    addStateOptions("y", { [&] { return y(); }, [&](QJsonValue val) { move(x(), val.toInt()); } });

    setupUi(this);
    QvMessageBusConnect(GroupManager);

    for (const auto &plugin : PluginHost->UsablePlugins())
    {
        const auto pluginInfo = PluginHost->GetPlugin(plugin);
        if (!pluginInfo->hasComponent(COMPONENT_SUBSCRIPTION_ADAPTER))
            continue;
        const auto subscriptionAdapterInterface = pluginInfo->pluginInterface->GetSubscriptionAdapter();
        const auto types = subscriptionAdapterInterface->SupportedSubscriptionTypes();
        for (const auto &type : types)
        {
            subscriptionTypeCB->addItem(pluginInfo->metadata.Name + ": " + type.displayName, type.protocol);
        }
    }

    //
    dnsSettingsWidget = new DnsSettingsWidget(this);
    routeSettingsWidget = new RouteSettingsMatrixWidget(GlobalConfig.kernelConfig.AssetsPath(), this);
    //
    dnsSettingsGB->setLayout(new QGridLayout(dnsSettingsGB));
    dnsSettingsGB->layout()->addWidget(dnsSettingsWidget);
    //
    routeSettingsGB->setLayout(new QGridLayout(routeSettingsGB));
    routeSettingsGB->layout()->addWidget(routeSettingsWidget);
    //
    updateColorScheme();
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
    connect(ConnectionManager, &QvConfigHandler::OnConnectionLinkedWithGroup, [this] { reloadConnectionsList(currentGroupId); });
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
        ConnectionManager->RemoveConnectionFromGroup(ConnectionId(item), currentGroupId);
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
            auto root = RouteManager->GenerateFinalConfig({ id, currentGroupId }, false);
            //
            // Apply export filter
            exportConnectionFilter(root);
            //
            if (filePath.endsWith(".json"))
            {
                filePath += ".json";
            }
            //
            StringToFile(JsonToString(root), filePath);
            QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(filePath).absoluteDir().absolutePath()));
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
                auto root = RouteManager->GenerateFinalConfig({ id, currentGroupId });
                //
                // Apply export filter
                exportConnectionFilter(root);
                //
                const auto fileName = RemoveInvalidFileName(GetDisplayName(id)) + ".json";
                StringToFile(JsonToString(root), path + "/" + fileName);
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
    connectionListRCMenu_LinkToMenu->clear();
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
    if (group == NullGroupId)
        return;
    connectionsTable->clearContents();
    connectionsTable->model()->removeRows(0, connectionsTable->rowCount());
    const auto &connections = ConnectionManager->GetConnections(group);
    for (auto i = 0; i < connections.count(); i++)
    {
        const auto &conn = connections.at(i);
        connectionsTable->insertRow(i);
        //
        auto displayNameItem = new QTableWidgetItem(GetDisplayName(conn));
        displayNameItem->setData(Qt::UserRole, conn.toString());
        auto typeItem = new QTableWidgetItem(GetConnectionProtocolString(conn));
        //
        const auto [type, host, port] = GetConnectionInfo(conn);
        auto hostPortItem = new QTableWidgetItem(host + ":" + QSTRN(port));
        //
        QStringList groupsNamesString;
        for (const auto &group : ConnectionManager->GetConnectionContainedIn(conn))
        {
            groupsNamesString.append(GetDisplayName(group));
        }
        auto groupsItem = new QTableWidgetItem(groupsNamesString.join(";"));
        connectionsTable->setItem(i, 0, displayNameItem);
        connectionsTable->setItem(i, 1, typeItem);
        connectionsTable->setItem(i, 2, hostPortItem);
        connectionsTable->setItem(i, 3, groupsItem);
    }
    connectionsTable->resizeColumnsToContents();
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
        ConnectionManager->LinkConnectionWithGroup(ConnectionId(connId), groupId);
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
        ConnectionManager->MoveConnectionFromToGroup(ConnectionId(connId), currentGroupId, groupId);
    }
    reloadConnectionsList(currentGroupId);
}

void GroupManager::updateColorScheme()
{
    addGroupButton->setIcon(QIcon(QV2RAY_COLORSCHEME_FILE("add")));
    removeGroupButton->setIcon(QIcon(QV2RAY_COLORSCHEME_FILE("ashbin")));
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

GroupManager::~GroupManager(){};
void GroupManager::on_addGroupButton_clicked()
{
    auto const key = tr("New Group") + " - " + GenerateRandomString(5);
    auto id = ConnectionManager->CreateGroup(key, false);
    //
    auto item = new QListWidgetItem(key);
    item->setData(Qt::UserRole, id.toString());
    groupList->addItem(item);
    groupList->setCurrentRow(groupList->count() - 1);
}

void GroupManager::on_updateButton_clicked()
{
    const auto address = subAddrTxt->text().trimmed();
    if (address.isEmpty())
    {
        QvMessageBoxWarn(this, tr("Update Subscription"), tr("The subscription link is empty."));
        return;
    }
    if (!QUrl(address).isValid())
    {
        QvMessageBoxWarn(this, tr("Update Subscription"), tr("The subscription link is invalid."));
        return;
    }
    if (QvMessageBoxAsk(this, tr("Update Subscription"), tr("Would you like to update the subscription?")) == Yes)
    {
        this->setEnabled(false);
        qApp->processEvents();
        ConnectionManager->UpdateSubscription(currentGroupId);
        this->setEnabled(true);
        on_groupList_itemClicked(groupList->currentItem());
    }
}

void GroupManager::on_removeGroupButton_clicked()
{
    if (QvMessageBoxAsk(this, tr("Remove a Group"), tr("All connections will be moved to default group, do you want to continue?")) == Yes)
    {
        ConnectionManager->DeleteGroup(currentGroupId);
        auto item = groupList->currentItem();
        int index = groupList->row(item);
        groupList->removeItemWidget(item);
        delete item;
        if (groupList->count() > 0)
        {
            index = std::max(index, 0);
            index = std::min(index, groupList->count() - 1);
            groupList->setCurrentItem(groupList->item(index));
            on_groupList_itemClicked(groupList->item(index));
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
    if (currentGroupId != NullGroupId)
    {
        const auto routeId = ConnectionManager->GetGroupRoutingId(currentGroupId);
        const auto &[dns, fakedns] = dnsSettingsWidget->GetDNSObject();
        RouteManager->SetDNSSettings(routeId, dnsSettingsGB->isChecked(), dns, fakedns);
        RouteManager->SetAdvancedRouteSettings(routeId, routeSettingsGB->isChecked(), routeSettingsWidget->GetRouteConfig());
    }
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
    groupNameTxt->setText(GetDisplayName(currentGroupId));
    //
    const auto _group = ConnectionManager->GetGroupMetaObject(currentGroupId);
    groupIsSubscriptionGroup->setChecked(_group.isSubscription);
    subAddrTxt->setText(_group.subscriptionOption.address);
    lastUpdatedLabel->setText(timeToString(_group.lastUpdatedDate));
    createdAtLabel->setText(timeToString(_group.creationDate));
    updateIntervalSB->setValue(_group.subscriptionOption.updateInterval);
    {
        const auto &type = _group.subscriptionOption.type;
        const auto index = subscriptionTypeCB->findData(type);
        if (index < 0)
            QvMessageBoxWarn(this, tr("Unknown Subscription Type"), tr("Unknown subscription type \"%1\", a plugin may be missing.").arg(type));
        else
            subscriptionTypeCB->setCurrentIndex(index);
    }
    //
    // Import filters
    {
        IncludeRelation->setCurrentIndex(_group.subscriptionOption.IncludeRelation);
        IncludeKeywords->clear();
        for (const auto &key : _group.subscriptionOption.IncludeKeywords)
        {
            auto str = key.trimmed();
            if (!str.isEmpty())
            {
                IncludeKeywords->appendPlainText(str);
            }
        }
        ExcludeRelation->setCurrentIndex(_group.subscriptionOption.ExcludeRelation);
        ExcludeKeywords->clear();
        for (const auto &key : _group.subscriptionOption.ExcludeKeywords)
        {
            auto str = key.trimmed();
            if (!str.isEmpty())
            {
                ExcludeKeywords->appendPlainText(str);
            }
        }
    }
    //
    // Load DNS / Route config
    const auto routeId = ConnectionManager->GetGroupRoutingId(currentGroupId);
    {
        const auto &[overrideDns, dns, fakedns] = RouteManager->GetDNSSettings(routeId);
        dnsSettingsWidget->SetDNSObject(dns, fakedns);
        dnsSettingsGB->setChecked(overrideDns);
        //
        const auto &[overrideRoute, route] = RouteManager->GetAdvancedRoutingSettings(routeId);
        routeSettingsWidget->SetRouteConfig(route);
        routeSettingsGB->setChecked(overrideRoute);
    }
    reloadConnectionsList(currentGroupId);
}

void GroupManager::on_IncludeRelation_currentTextChanged(const QString &)
{
    ConnectionManager->SetSubscriptionIncludeRelation(currentGroupId, (SubscriptionFilterRelation) IncludeRelation->currentIndex());
}

void GroupManager::on_ExcludeRelation_currentTextChanged(const QString &)
{
    ConnectionManager->SetSubscriptionExcludeRelation(currentGroupId, (SubscriptionFilterRelation) ExcludeRelation->currentIndex());
}

void GroupManager::on_IncludeKeywords_textChanged()
{
    QStringList keywords = IncludeKeywords->toPlainText().replace("\r", "").split("\n");
    ConnectionManager->SetSubscriptionIncludeKeywords(currentGroupId, keywords);
}

void GroupManager::on_ExcludeKeywords_textChanged()
{
    QStringList keywords = ExcludeKeywords->toPlainText().replace("\r", "").split("\n");
    ConnectionManager->SetSubscriptionExcludeKeywords(currentGroupId, keywords);
}

void GroupManager::on_groupList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *priv)
{
    if (priv)
    {
        const auto group = ConnectionManager->GetGroupMetaObject(currentGroupId);
        const auto &[dns, fakedns] = dnsSettingsWidget->GetDNSObject();
        RouteManager->SetDNSSettings(group.routeConfigId, dnsSettingsGB->isChecked(), dns, fakedns);
        RouteManager->SetAdvancedRouteSettings(group.routeConfigId, routeSettingsGB->isChecked(), routeSettingsWidget->GetRouteConfig());
    }
    if (current)
    {
        on_groupList_itemClicked(current);
    }
}

void GroupManager::on_subAddrTxt_textEdited(const QString &arg1)
{
    ConnectionManager->SetSubscriptionData(currentGroupId, std::nullopt, arg1);
}

void GroupManager::on_updateIntervalSB_valueChanged(double arg1)
{
    ConnectionManager->SetSubscriptionData(currentGroupId, std::nullopt, std::nullopt, arg1);
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

void GroupManager::exportConnectionFilter(CONFIGROOT &root)
{
    root.remove("api");
    root.remove("stats");
    auto inbounds = root["inbounds"].toArray();
    for (auto iter = inbounds.begin(); iter != inbounds.end();)
    {
        auto obj = iter->toObject();
        if (obj["tag"] == API_TAG_INBOUND)
            iter = inbounds.erase(iter);
        iter++;
    }
    root["inbounds"] = inbounds;
}

#undef GET_SELECTED_CONNECTION_IDS

void GroupManager::on_connectionsTable_customContextMenuRequested(const QPoint &pos)
{
    Q_UNUSED(pos)
    connectionListRCMenu->popup(QCursor::pos());
}

void GroupManager::on_subscriptionTypeCB_currentIndexChanged(int)
{
    ConnectionManager->SetSubscriptionType(currentGroupId, subscriptionTypeCB->currentData().toString());
}
