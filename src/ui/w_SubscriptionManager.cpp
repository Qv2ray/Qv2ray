#include "w_SubscriptionManager.hpp"
#include "common/QvHelpers.hpp"
#include "core/CoreUtils.hpp"

#include "core/connection/ConnectionIO.hpp"
#include "core/connection/Serialization.hpp"

SubscribeEditor::SubscribeEditor(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);
    QvMessageBusConnect(SubscribeEditor);
    addSubsButton->setIcon(QICON_R("add.png"));
    removeSubsButton->setIcon(QICON_R("delete.png"));

    for (auto i = GlobalConfig.subscriptions.begin(); i != GlobalConfig.subscriptions.end(); i++) {
        subscriptions[i.key()] = i.value();
    }

    LoadSubscriptionList(subscriptions);
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
    return currentSelectedConfig;
}

void SubscribeEditor::LoadSubscriptionList(QMap<QString, SubscriptionObject> list)
{
    subscriptionList->clear();

    for (auto i = 0; i < list.count(); i++) {
        subscriptionList->addItem(list.keys()[i]);
    }

    if (subscriptionList->count() > 0) {
        subscriptionList->setCurrentRow(0);
    }
}

SubscribeEditor::~SubscribeEditor()
{
}

void SubscribeEditor::on_addSubsButton_clicked()
{
    auto const key = QSTRN(QTime::currentTime().msecsSinceStartOfDay());
    subscriptionList->addItem(key);
    subscriptions[key].address = "http://example.com/myfile";
    QDir().mkpath(QV2RAY_SUBSCRIPTION_DIR + key);
    subscriptionList->setCurrentRow(subscriptions.count() - 1);
    SaveConfig();
}

void SubscribeEditor::on_updateButton_clicked()
{
    auto newName = subNameTxt->text().trimmed();
    auto newAddress = subAddrTxt->text().trimmed();
    auto newUpdateInterval = updateIntervalSB->value();

    if (currentSubName != newName) {
        // Rename needed.
        LOG(MODULE_SUBSCRIPTION, "Renaming a subscription, from " + currentSubName + " to: " + newName)
        bool canGo = true;

        if (newName.isEmpty() || !IsValidFileName(newName)) {
            QvMessageBoxWarn(this, tr("Renaming a subscription"), tr("The subscription name is invalid, please try another."));
            canGo = false;
        }

        if (subscriptionList->findItems(newName, Qt::MatchExactly).count() > 0) {
            QvMessageBoxWarn(this, tr("Renaming a subscription"), tr("New name of this subscription has been used already, please suggest another one"));
            canGo = false;
        }

        if (!canGo) {
            subNameTxt->setText(currentSubName);
            return;
        }

        ////bool result = RenameSubscription(currentSubName, newName);
        //
        //if (!result) {
        //    QvMessageBoxWarn(this, tr("Renaming a subscription"), tr("Failed to rename a subscription, this is an unknown error."));
        //    return;
        //}
        subscriptions[newName] = subscriptions[currentSubName];
        subscriptions.remove(currentSubName);
        subNameTxt->setText(newName);
        //
        QvMessageBoxInfo(this, "NOT SUPPORTED", "WIP");
        // Update auto-start config if possible
        //auto ASsetting = GlobalConfig.autoStartConfig.subscriptionName;
        //
        //if (ASsetting == currentSubName) {
        //    GlobalConfig.autoStartConfig.subscriptionName = newName;
        //}
        SaveGlobalConfig(GlobalConfig);
        // This will set the name to the new name.
        LoadSubscriptionList(subscriptions);
        QvMessageBoxInfo(this, tr("Renaming a subscription"), tr("Successfully renamed a subscription"));
    }

    subscriptions[currentSubName].updateInterval = newUpdateInterval;

    if (subscriptions[currentSubName].address != newAddress) {
        LOG(MODULE_SUBSCRIPTION, "Setting new address, from " + subscriptions[currentSubName].address + " to: " + newAddress)
        subscriptions[currentSubName].address = newAddress;
    }

    SaveConfig();

    if (QvMessageBoxAsk(this, tr("Update Subscription"), tr("Would you like to reload this subscription from the Url?")) == QMessageBox::Yes) {
        StartUpdateSubscription(currentSubName);
    }
}

void SubscribeEditor::StartUpdateSubscription(const QString &subscriptionName)
{
    //this->setEnabled(false);
    //auto data = helper.syncget(subscriptions[subscriptionName].address, withProxyCB->isChecked());
    //auto content = DecodeSubscriptionString(data).trimmed();
    //
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
    //this->setEnabled(true);
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

void SubscribeEditor::on_subscriptionList_currentRowChanged(int currentRow)
{
    //if (subscriptionList->count() == 0) {
    //    return;
    //}
    //
    //if (currentRow < 0 && subscriptionList->count() > 0) {
    //    subscriptionList->setCurrentRow(0);
    //}
    //
    //currentSubName = subscriptionList->currentItem()->text();
    //LOG(MODULE_UI, "Subscription row changed, new name: " + currentSubName)
    ////
    //subNameTxt->setText(currentSubName);
    //subAddrTxt->setText(subscriptions[currentSubName].address);
    //updateIntervalSB->setValue(subscriptions[currentSubName].updateInterval);
    //lastUpdatedLabel->setText(timeToString(subscriptions[currentSubName].lastUpdated));
    ////
    //connectionsList->clear();
    //auto _list = GetSubscriptionConnection(currentSubName);
    //
    //for (auto i = 0; i < _list.count(); i++) {
    //    connectionsList->addItem(_list.keys()[i]);
    //}
}

void SubscribeEditor::SaveConfig()
{
    QMap<QString, SubscriptionObject> newConf;

    for (auto _ : subscriptions.toStdMap()) {
        if (!_.second.address.isEmpty()) {
            newConf[_.first] = _.second;
        }
    }

    GlobalConfig.subscriptions = newConf;
    SaveGlobalConfig(GlobalConfig);
}

void SubscribeEditor::on_buttonBox_accepted()
{
    SaveConfig();
}

void SubscribeEditor::on_subscriptionList_itemSelectionChanged()
{
    groupBox_2->setEnabled(subscriptionList->selectedItems().count() > 0);
}

void SubscribeEditor::on_updateIntervalSB_valueChanged(double arg1)
{
    subscriptions[currentSubName].updateInterval = arg1;
}

void SubscribeEditor::on_connectionsList_itemClicked(QListWidgetItem *item)
{
    if (item != nullptr) {
        auto name = item->text();
        currentSelectedConfig.first = name;
        //currentSelectedConfig.second = GetSubscriptionConnection(currentSubName)[name];
    }
}
