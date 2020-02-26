#include "w_SubscriptionManager.hpp"
#include "common/QvHelpers.hpp"
#include "core/config/ConfigBackend.hpp"
#include "core/handler/ConnectionHandler.hpp"

SubscribeEditor::SubscribeEditor(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);
    QvMessageBusConnect(SubscribeEditor);
    addSubsButton->setIcon(QICON_R("add.png"));
    removeSubsButton->setIcon(QICON_R("delete.png"));

    for (auto subs : ConnectionManager->Subscriptions()) {
        subscriptionList->addTopLevelItem(new QTreeWidgetItem(QStringList() << ConnectionManager->GetDisplayName(subs) << subs.toString()));
    }
}

QvMessageBusSlotImpl(SubscribeEditor)
{
    switch (msg) {
            QvMessageBusShowDefault
            QvMessageBusHideDefault
            QvMessageBusRetranslateDefault
    }
}

QPair<QString, CONFIGROOT> SubscribeEditor::GetSelectedConfig()
{
    return QPair<QString, CONFIGROOT> ();
}

SubscribeEditor::~SubscribeEditor()
{
}

void SubscribeEditor::on_addSubsButton_clicked()
{
    //    auto const key = QSTRN(QTime::currentTime().msecsSinceStartOfDay());
    //    subscriptionList->addItem(key);
    //    subscriptions[key].address = "https://example.com/myfile";
    //    QDir().mkpath(QV2RAY_SUBSCRIPTION_DIR + key);
    //    subscriptionList->setCurrentRow(subscriptions.count() - 1);
    //    SaveConfig();
}

void SubscribeEditor::on_updateButton_clicked()
{
    //auto newName = subNameTxt->text().trimmed();
    //auto newAddress = subAddrTxt->text().trimmed();
    //auto newUpdateInterval = updateIntervalSB->value();
    //if (currentSubId != newName) {
    //    // Rename needed.
    //    LOG(MODULE_SUBSCRIPTION, "Renaming a subscription, from " + currentSubId + " to: " + newName)
    //    bool canGo = true;
    //
    //    if (newName.isEmpty() || !IsValidFileName(newName)) {
    //        QvMessageBoxWarn(this, tr("Renaming a subscription"), tr("The subscription name is invalid, please try another."));
    //        canGo = false;
    //    }
    //
    //    if (subscriptionList->findItems(newName, Qt::MatchExactly).count() > 0) {
    //        QvMessageBoxWarn(this, tr("Renaming a subscription"), tr("New name of this subscription has been used already, please suggest another one"));
    //        canGo = false;
    //    }
    //
    //    if (!canGo) {
    //        subNameTxt->setText(currentSubId);
    //        return;
    //    }
    //
    //    ////bool result = RenameSubscription(currentSubName, newName);
    //    //
    //    //if (!result) {
    //    //    QvMessageBoxWarn(this, tr("Renaming a subscription"), tr("Failed to rename a subscription, this is an unknown error."));
    //    //    return;
    //    //}
    //    subscriptions[newName] = subscriptions[currentSubId];
    //    subscriptions.remove(currentSubId);
    //    subNameTxt->setText(newName);
    //    //
    //    QvMessageBoxInfo(this, "NOT SUPPORTED", "WIP");
    //    // Update auto-start config if possible
    //    //auto ASsetting = GlobalConfig.autoStartConfig.subscriptionName;
    //    //
    //    //if (ASsetting == currentSubName) {
    //    //    GlobalConfig.autoStartConfig.subscriptionName = newName;
    //    //}
    //    SaveGlobalConfig(GlobalConfig);
    //    // This will set the name to the new name.
    //    LoadSubscriptionList(subscriptions);
    //    // Keep subAddress && Interval changes
    //    // Update thing still down
    //    subAddrTxt->setText(newAddress);
    //    updateIntervalSB->setValue(newUpdateInterval);
    //    QvMessageBoxInfo(this, tr("Renaming a subscription"), tr("Successfully renamed a subscription"));
    //}
    //
    //subscriptions[currentSubId].updateInterval = newUpdateInterval;
    //
    //if (subscriptions[currentSubId].address != newAddress) {
    //    LOG(MODULE_SUBSCRIPTION, "Setting new address, from " + subscriptions[currentSubId].address + " to: " + newAddress)
    //    subscriptions[currentSubId].address = newAddress;
    //}
    //
    ////SaveConfig();
    //
    ////if (QvMessageBoxAsk(this, tr("Update Subscription"), tr("Would you like to reload this subscription from the Url?")) == QMessageBox::Yes) {
    ////    StartUpdateSubscription(currentSubId);
    ////}
}

void SubscribeEditor::StartUpdateSubscription(const QString &subscriptionName)
{
    this->setEnabled(false);
    // auto data = helper.syncget(subscriptions[subscriptionName].address, withProxyCB->isChecked());
    // auto content = DecodeSubscriptionString(data).trimmed();
    //if (!content.isEmpty()) {
    //    connectionsList->clear();
    //    auto vmessList = SplitLines(content);
    //    QDir(QV2RAY_SUBSCRIPTION_DIR + subscriptionName).removeRecursively();
    //    QDir().mkpath(QV2RAY_SUBSCRIPTION_DIR + subscriptionName);
    //
    //    for (auto vmess : vmessList) {
    //        QString errMessage;
    //        QString _alias;
    //        auto config = ConvertConfigFromString(vmess.trimmed(), &_alias, &errMessage);
    //
    //        if (!errMessage.isEmpty()) {
    //            LOG(MODULE_SUBSCRIPTION, "Processing a subscription with following error: " + errMessage)
    //        } else {
    //            //SaveSubscriptionConfig(config, subscriptionName, &_alias);
    //            connectionsList->addItem(_alias);
    //        }
    //    }
    //
    //    subscriptions[subscriptionName].lastUpdated = system_clock::to_time_t(system_clock::now());
    //    lastUpdatedLabel->setText(timeToString(subscriptions[subscriptionName].lastUpdated));
    //    isUpdateInProgress = false;
    //} else {
    //    LOG(MODULE_NETWORK, "We have received an empty string from the URL.")
    //    QvMessageBoxWarn(this, tr("Updating subscriptions"), tr("Failed to process the result from the upstream, please check your Url."));
    //}
    //
    this->setEnabled(true);
}

void SubscribeEditor::on_removeSubsButton_clicked()
{
    //if (subscriptionList->currentRow() < 0)
    //    return;
    //
    //auto name = subscriptionList->currentItem()->text();
    //subscriptionList->takeItem(subscriptionList->currentRow());
    //subscriptions.remove(name);
    //
    //if (!name.isEmpty()) {
    //    QDir(QV2RAY_SUBSCRIPTION_DIR + name).removeRecursively();
    //}
    //
    ////// If removed a whole subscription...
    ////if (GlobalConfig.autoStartConfig.subscriptionName == name) {
    ////    GlobalConfig.autoStartConfig = QvConnectionObject();
    ////    SaveGlobalConfig(GlobalConfig);
    ////}
    //groupBox_2->setEnabled(subscriptionList->count() > 0);
    //SaveConfig();
}

void SubscribeEditor::SaveConfig()
{
    //QMap<QString, SubscriptionObject_Config> newConf;
    //
    //for (auto _ : subscriptions.toStdMap()) {
    //    if (!_.second.address.isEmpty()) {
    //        newConf[_.first] = _.second;
    //    }
    //}
    //
    //GlobalConfig.subscriptions = newConf;
    //SaveGlobalConfig(GlobalConfig);
}

void SubscribeEditor::on_buttonBox_accepted()
{
    SaveConfig();
}

void SubscribeEditor::on_subscriptionList_itemSelectionChanged()
{
    groupBox_2->setEnabled(subscriptionList->selectedItems().count() > 0);
}

void SubscribeEditor::on_subscriptionList_itemClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column)

    if (item == nullptr) {
        return;
    }

    currentSubId = GroupId(item->text(1));
    //
    subNameTxt->setText(ConnectionManager->GetDisplayName(currentSubId));
    auto [addr, lastUpdated, updateInterval] = ConnectionManager->GetSubscriptionData(currentSubId);
    subAddrTxt->setText(addr);
    lastUpdatedLabel->setText(timeToString(lastUpdated));
    updateIntervalSB->setValue(updateInterval);
    //
    connectionsList->clear();

    for (auto conn : ConnectionManager->Connections(currentSubId)) {
        connectionsList->addItem(ConnectionManager->GetDisplayName(conn));
    }
}
