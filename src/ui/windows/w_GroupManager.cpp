#include "w_GroupManager.hpp"

#include "Qv2rayBase/Common/ProfileHelpers.hpp"
#include "Qv2rayBase/Common/Utils.hpp"
#include "Qv2rayBase/Plugin/PluginAPIHost.hpp"
#include "Qv2rayBase/Plugin/PluginManagerCore.hpp"
#include "Qv2rayBase/Profile/ProfileManager.hpp"
#include "ui/widgets/ConfigurableEditorWidget.hpp"
#include "ui/widgets/editors/DnsSettingsWidget.hpp"
#include "ui/widgets/editors/RouteSettingsMatrix.hpp"

#include <QFileDialog>

QString RemoveInvalidFileName(QString fileName)
{
    const static QString pattern = uR"("/\?%&^*;:|><)"_qs;
    std::replace_if(
        fileName.begin(), fileName.end(), [](QChar c) { return pattern.contains(c); }, '_');
    return fileName;
};

#define SELECTED_ROWS_INDEX                                                                                                                                              \
    [&]()                                                                                                                                                                \
    {                                                                                                                                                                    \
        const auto &__selection = this->connectionsTable->selectedItems();                                                                                               \
        QSet<int> rows;                                                                                                                                                  \
        for (const auto &selection : __selection)                                                                                                                        \
            rows.insert(this->connectionsTable->row(selection));                                                                                                         \
        return rows;                                                                                                                                                     \
    }()

#define GET_SELECTED_CONNECTION_IDS(connectionIdList)                                                                                                                    \
    [&]()                                                                                                                                                                \
    {                                                                                                                                                                    \
        QList<ConnectionId> _list;                                                                                                                                       \
        for (const auto &i : connectionIdList)                                                                                                                           \
            _list.push_back(ConnectionId(this->connectionsTable->item(i, 0)->data(Qt::UserRole).toString()));                                                            \
        return _list;                                                                                                                                                    \
    }()

GroupManager::GroupManager(QWidget *parent) : QvDialog("GroupManager", parent)
{
    setupUi(this);
    QvMessageBusConnect();

    for (const auto &[pluginInfo, info] : QvPluginAPIHost->Subscription_GetProviderInfoList())
    {
        subscriptionTypeCB->addItem(pluginInfo->metadata().Name + ": " + info.displayName, info.id.toString());
    }

    dnsSettingsWidget = new DnsSettingsWidget(this);
    routeSettingsWidget = new RouteSettingsMatrixWidget(this);

    dnsSettingsGB->setLayout(new QGridLayout(dnsSettingsGB));
    dnsSettingsGB->layout()->addWidget(dnsSettingsWidget);

    routeSettingsGB->setLayout(new QGridLayout(routeSettingsGB));
    routeSettingsGB->layout()->addWidget(routeSettingsWidget);

    connectionListRCMenu->addSection(tr("Connection Management"));
    connectionListRCMenu->addAction(deleteConnectionAction);
    connectionListRCMenu->addSeparator();
    connectionListRCMenu->addMenu(connectionListRCMenu_CopyToMenu);
    connectionListRCMenu->addMenu(connectionListRCMenu_MoveToMenu);
    connectionListRCMenu->addMenu(connectionListRCMenu_LinkToMenu);

    connect(deleteConnectionAction, &QAction::triggered, this, &GroupManager::onRCMDeleteConnectionTriggered);
    connect(QvProfileManager, &Qv2rayBase::Profile::ProfileManager::OnConnectionLinkedWithGroup, [this] { reloadConnectionsList(currentGroupId); });
    connect(QvProfileManager, &Qv2rayBase::Profile::ProfileManager::OnGroupCreated, this, &GroupManager::reloadGroupRCMActions);
    connect(QvProfileManager, &Qv2rayBase::Profile::ProfileManager::OnGroupDeleted, this, &GroupManager::reloadGroupRCMActions);
    connect(QvProfileManager, &Qv2rayBase::Profile::ProfileManager::OnGroupRenamed, this, &GroupManager::reloadGroupRCMActions);

    GroupManager::updateColorScheme();

    for (const auto &group : QvProfileManager->GetGroups())
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
        QvProfileManager->RemoveFromGroup(ConnectionId(item), currentGroupId);
    reloadConnectionsList(currentGroupId);
}

void GroupManager::reloadGroupRCMActions()
{
    connectionListRCMenu_CopyToMenu->clear();
    connectionListRCMenu_MoveToMenu->clear();
    connectionListRCMenu_LinkToMenu->clear();
    for (const auto &group : QvProfileManager->GetGroups())
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

void GroupManager::SaveCurrentGroup()
{
    const auto groupObject = QvProfileManager->GetGroupObject(currentGroupId);
    auto routing = QvProfileManager->GetRouting(groupObject.route_id);

    const auto &[dns, fakedns] = dnsSettingsWidget->GetDNSObject();
    routing.overrideDNS = dnsSettingsGB->isChecked();
    routing.dns = dns.toJson();

    QJsonArray pools;
    for (const auto &pool : fakedns)
    {
        pools.append(pool.toJson());
    }
    QJsonObject fdns;
    fdns.insert("pools", pools);
    routing.fakedns = fdns;

    const auto routematrix = routeSettingsWidget->GetRouteConfig();
    routing.extraOptions.insert(RouteMatrixConfig::EXTRA_OPTIONS_ID, routematrix.toJson());

    QvProfileManager->UpdateRouting(groupObject.route_id, routing);

    {
        auto subscription = groupObject.subscription_config;

        subscription.providerId = SubscriptionProviderId{ subscriptionTypeCB->currentData().toString() };
        subscription.address = subAddrTxt->text().trimmed();
        subscription.updateInterval = updateIntervalSB->value();

        if (subscriptionProviderOptionsEditor)
            subscription.providerSettings = SubscriptionProviderOptions{ QJsonObject::fromVariantMap(subscriptionProviderOptionsEditor->GetContent().toMap()) };

        subscription.excludeKeywords = ExcludeKeywords->toPlainText().remove('\r').split('\n');
        subscription.includeKeywords = IncludeKeywords->toPlainText().remove('\r').split('\n');
        subscription.excludeRelation = (SubscriptionConfigObject::FilterRelation) ExcludeRelation->currentIndex();
        subscription.includeRelation = (SubscriptionConfigObject::FilterRelation) IncludeRelation->currentIndex();

        QvProfileManager->SetSubscriptionData(currentGroupId, subscription);
    }
}

void GroupManager::setupSubscriptionProviderSettingsWidget()
{

    // Do not use the provider id from groupObject, that's not updated.
    const auto currentProviderId = SubscriptionProviderId{ subscriptionTypeCB->currentData().toString() };

    const auto &[plugin, info] = QvPluginAPIHost->Subscription_GetProviderInfo(currentProviderId);

    subscriptionOptionsEditorWidget->setVisible(info.mode == Qv2rayPlugin::SubscribingMode::Subscribe_FetcherAndDecoder);
    subAddrTxt->setVisible(info.mode == Qv2rayPlugin::SubscribingMode::Subscribe_Decoder);
    subAddrLabel->setVisible(info.mode == Qv2rayPlugin::SubscribingMode::Subscribe_Decoder);

    const auto _group = QvProfileManager->GetGroupObject(currentGroupId);
    switch (info.mode)
    {
        case Qv2rayPlugin::SubscribingMode::Subscribe_Decoder:
        {
            subAddrTxt->setText(_group.subscription_config.address);
            break;
        }
        case Qv2rayPlugin::SubscribingMode::Subscribe_FetcherAndDecoder:
        {
            if (subscriptionProviderOptionsEditor)
                delete subscriptionProviderOptionsEditor;
            subscriptionProviderOptionsEditor = new ConfigurableEditor(info.settings);
            subscriptionOptionsEditorLayout->setContentsMargins(0, 0, 0, 0);
            subscriptionOptionsEditorLayout->addWidget(subscriptionProviderOptionsEditor);
            subscriptionProviderOptionsEditor->SetContent(_group.subscription_config.providerSettings.toVariantMap());
            break;
        }
        default: break;
    }
}

void GroupManager::reloadConnectionsList(const GroupId &group)
{
    if (group.isNull())
        return;
    connectionsTable->clearContents();
    connectionsTable->model()->removeRows(0, connectionsTable->rowCount());
    const auto &connections = QvProfileManager->GetConnections(group);
    for (auto i = 0; i < connections.count(); i++)
    {
        const auto &conn = connections.at(i);
        connectionsTable->insertRow(i);
        //
        auto displayNameItem = new QTableWidgetItem(GetDisplayName(conn));
        displayNameItem->setData(Qt::UserRole, conn.toString());
        auto typeItem = new QTableWidgetItem(GetConnectionProtocolDescription(conn));

        QStringList groupsNamesString;
        for (const auto &group : QvProfileManager->GetGroups(conn))
            groupsNamesString.append(GetDisplayName(group));
        auto groupsItem = new QTableWidgetItem(groupsNamesString.join(';'));
        connectionsTable->setItem(i, 0, displayNameItem);
        connectionsTable->setItem(i, 1, typeItem);
        const auto profile = QvProfileManager->GetConnection(conn);
        if (!profile.outbounds.isEmpty())
        {
            const auto [type, host, port] = GetOutboundInfo(profile.outbounds.first());
            auto hostPortItem = new QTableWidgetItem(host + ":" + port);
            connectionsTable->setItem(i, 2, hostPortItem);
        }
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
        QvProfileManager->CreateConnection(QvProfileManager->GetConnection(connectionId), GetDisplayName(connectionId), groupId);
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
        QvProfileManager->LinkWithGroup(ConnectionId(connId), groupId);
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
        QvProfileManager->MoveToGroup(ConnectionId(connId), currentGroupId, groupId);
    }
    reloadConnectionsList(currentGroupId);
}

void GroupManager::updateColorScheme()
{
    addGroupButton->setIcon(QIcon(STYLE_RESX("add")));
    removeGroupButton->setIcon(QIcon(STYLE_RESX("ashbin")));
}

QvMessageBusSlotImpl(GroupManager)
{
    switch (msg)
    {

        MBUpdateColorSchemeDefaultImpl
    }
}

GroupManager::~GroupManager(){};
void GroupManager::on_addGroupButton_clicked()
{
    const auto name = tr("New Group") + " - " + GenerateRandomString(5);
    const auto id = QvProfileManager->CreateGroup(name);
    auto item = new QListWidgetItem(name);
    item->setData(Qt::UserRole, id.toString());
    groupList->addItem(item);
    groupList->setCurrentRow(groupList->count() - 1);
}

void GroupManager::on_updateButton_clicked()
{
    if (QvBaselib->Ask(tr("Update Subscription"), tr("Would you like to update the subscription?")) == Qv2rayBase::MessageOpt::Yes)
    {
        SaveCurrentGroup();
        this->setEnabled(false);
        qApp->processEvents();
        QvProfileManager->UpdateSubscription(currentGroupId, false);
        this->setEnabled(true);
        on_groupList_itemClicked(groupList->currentItem());
    }
}

void GroupManager::on_removeGroupButton_clicked()
{
    if (QvBaselib->Ask(tr("Remove a Group"), tr("All connections will be moved to default group, do you want to continue?")) == Qv2rayBase::MessageOpt::Yes)
    {
        QvProfileManager->DeleteGroup(currentGroupId, true);
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
    if (!currentGroupId.isNull())
        SaveCurrentGroup();
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
    const auto _group = QvProfileManager->GetGroupObject(currentGroupId);
    groupIsSubscriptionGroup->setChecked(_group.subscription_config.isSubscription);
    lastUpdatedLabel->setText(TimeToString(_group.updated));
    createdAtLabel->setText(TimeToString(_group.created));
    updateIntervalSB->setValue(_group.subscription_config.updateInterval);
    {
        const auto providerID = _group.subscription_config.providerId;
        const auto index = subscriptionTypeCB->findData(providerID.toString());
        if (_group.subscription_config.isSubscription && index < 0)
            QvBaselib->Warn(tr("Unknown Subscription Type"), tr("Unknown subscription type \"%1\", a plugin may be missing.").arg(providerID.toString()));
        else
        {
            subscriptionTypeCB->setCurrentIndex(index);
            setupSubscriptionProviderSettingsWidget();
        }
    }
    //
    // Load DNS / Route config
    const auto routeId = QvProfileManager->GetGroupRoutingId(currentGroupId);
    {
        const auto routingObject = QvProfileManager->GetRouting(routeId);

        QList<V2RayFakeDNSObject> pools;
        for (const auto &pool : routingObject.fakedns["pools"].toArray())
        {
            pools.append(V2RayFakeDNSObject::fromJson(pool.toObject()));
        }
        dnsSettingsWidget->SetDNSObject(V2RayDNSObject::fromJson(routingObject.dns), pools);

        dnsSettingsGB->setChecked(routingObject.overrideDNS);
        //
        RouteMatrixConfig c;
        c.loadJson(routingObject.extraOptions.value(RouteMatrixConfig::EXTRA_OPTIONS_ID));
        routeSettingsWidget->SetRoute(c);
        routeSettingsGB->setChecked(routingObject.overrideRules);
    }
    //
    // Import filters
    {
        IncludeRelation->setCurrentIndex(_group.subscription_config.includeRelation);
        IncludeKeywords->clear();
        for (const auto &key : _group.subscription_config.includeKeywords)
        {
            auto str = key.trimmed();
            if (!str.isEmpty())
            {
                IncludeKeywords->appendPlainText(str);
            }
        }
        ExcludeRelation->setCurrentIndex(_group.subscription_config.excludeRelation);
        ExcludeKeywords->clear();
        for (const auto &key : _group.subscription_config.excludeKeywords)
        {
            auto str = key.trimmed();
            if (!str.isEmpty())
            {
                ExcludeKeywords->appendPlainText(str);
            }
        }
    }
    reloadConnectionsList(currentGroupId);
}

void GroupManager::on_groupList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *priv)
{
    if (priv)
    {
        SaveCurrentGroup();
    }
    if (current)
    {
        on_groupList_itemClicked(current);
    }
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
    auto subscription = QvProfileManager->GetGroupObject(currentGroupId).subscription_config;
    subscription.isSubscription = checked;
    QvProfileManager->SetSubscriptionData(currentGroupId, subscription);
}

void GroupManager::on_groupNameTxt_textEdited(const QString &arg1)
{
    groupList->selectedItems().first()->setText(arg1);
    QvProfileManager->RenameGroup(currentGroupId, arg1.trimmed());
}

void GroupManager::on_deleteSelectedConnBtn_clicked()
{
    onRCMDeleteConnectionTriggered();
}

#undef GET_SELECTED_CONNECTION_IDS

void GroupManager::on_connectionsTable_customContextMenuRequested(const QPoint &pos)
{
    Q_UNUSED(pos)
    connectionListRCMenu->popup(QCursor::pos());
}

void GroupManager::on_subscriptionTypeCB_currentIndexChanged(int)
{
    setupSubscriptionProviderSettingsWidget();
}
