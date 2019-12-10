﻿#include "w_SubscriptionEditor.hpp"
#include "QvHTTPRequestHelper.hpp"
#include "QvUtils.hpp"
#include "QvCoreConfigOperations.hpp"

SubscribeEditor::SubscribeEditor(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);
    auto conf = GetGlobalConfig();
    addSubsButton->setIcon(QICON_R("add.png"));
    removeSubsButton->setIcon(QICON_R("delete.png"));

    for (auto _ : conf.subscribes) {
        subscriptions[QSTRING(_.first)] = QSTRING(_.second);
    }

    LoadSubscriptionList(subscriptions);
}

void SubscribeEditor::LoadSubscriptionList(QMap<QString, QString> list)
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
    auto const key = QString::number(QTime::currentTime().msecsSinceStartOfDay());
    subscriptionList->addItem(key);
    subscriptions[key] = "http://example.com/myfile";
    QDir().mkpath(QV2RAY_SUBSCRIPTION_DIR + key);
    subscriptionList->setCurrentRow(subscriptions.count() - 1);
    SaveConfig();
}

void SubscribeEditor::on_updateButton_clicked()
{
    auto newName = subNameTxt->text().trimmed();
    auto newAddress = subAddrTxt->text().trimmed();

    if (currentSubName != newName) {
        // Rename needed.
        LOG(MODULE_SUBSCRIPTION, "Renaming a subscription, from " + currentSubName.toStdString() + " to: " + newName.toStdString())
        bool canGo = true;

        if (newName.isEmpty() || !IsValidFileName(newName)) {
            QvMessageBox(this, tr("Renaming a subscription"), tr("The subscription name is invalid, please try another."));
            canGo = false;
        }

        if (subscriptionList->findItems(newName, Qt::MatchExactly).count() > 0) {
            QvMessageBox(this, tr("Renaming a subscription"), tr("New name of this subscription has been used already, please suggest another one"));
            canGo = false;
        }

        if (!canGo) {
            subNameTxt->setText(currentSubName);
            return;
        }

        bool result = RenameSubscription(currentSubName, newName);

        if (!result) {
            QvMessageBox(this, tr("Renaming a subscription"), tr("Failed to rename a subscription, this is an unknown error."));
            return;
        }

        subscriptions[newName] = subscriptions[currentSubName];
        subscriptions.remove(currentSubName);
        subNameTxt->setText(newName);
        //
        // Update auto-start config if possible
        auto conf = GetGlobalConfig();
        auto ASsetting = QSTRING(conf.autoStartConfig.subscriptionName);

        if (ASsetting == currentSubName) {
            conf.autoStartConfig.subscriptionName = newName.toStdString();
        }

        SetGlobalConfig(conf);
        // This will set the name to the new name.
        LoadSubscriptionList(subscriptions);
        QvMessageBox(this, tr("Renaming a subscription"), tr("Successfully renamed a subscription"));
    }

    if (subscriptions[currentSubName] != newAddress) {
        LOG(MODULE_SUBSCRIPTION, ("Setting new address, from " + subscriptions[currentSubName] + " to: " + newAddress).toStdString())
        subscriptions[currentSubName] = newAddress;
    }

    SaveConfig();

    if (QvMessageBoxAsk(this, tr("Update Subscription"), tr("Would you like to reload this subscription from the Url?")) == QMessageBox::Yes) {
        StartUpdateSubscription(currentSubName);
    }
}

void SubscribeEditor::StartUpdateSubscription(const QString &subscriptionName)
{
    this->setEnabled(false);
    auto data = helper.syncget(subscriptions[subscriptionName]);
    auto content = DecodeSubscriptionString(data).trimmed();

    if (!content.isEmpty()) {
        connectionsList->clear();
        auto vmessList = SplitLines(content);
        QDir(QV2RAY_SUBSCRIPTION_DIR + subscriptionName).removeRecursively();
        QDir().mkpath(QV2RAY_SUBSCRIPTION_DIR + subscriptionName);

        for (auto vmess : vmessList) {
            QString errMessage;
            QString _alias;
            auto config = ConvertConfigFromVMessString(vmess.trimmed(), &_alias, &errMessage);

            if (!errMessage.isEmpty()) {
                LOG(MODULE_SUBSCRIPTION, "Processing a subscription with following error: " + errMessage.toStdString())
            } else {
                connectionsList->addItem(_alias);
                SaveSubscriptionConfig(config, subscriptionName, _alias);
            }
        }

        isUpdateInProgress = false;
    } else {
        LOG(MODULE_NETWORK, "We have received an empty string from the URL.")
        QvMessageBox(this, tr("Updating subscriptions"), tr("Failed to process the result from the upstream, please check your Url"));
    }

    this->setEnabled(true);
}

void SubscribeEditor::on_removeSubsButton_clicked()
{
    if (subscriptionList->currentRow() < 0)
        return;

    auto name = subscriptionList->currentItem()->text();
    subscriptionList->takeItem(subscriptionList->currentRow());
    subscriptions.remove(name);

    if (!name.isEmpty()) {
        QDir(QV2RAY_SUBSCRIPTION_DIR + name).removeRecursively();
    }

    // If removed a whole subscription...
    auto conf = GetGlobalConfig();

    if (conf.autoStartConfig.subscriptionName == name.toStdString()) {
        conf.autoStartConfig.subscriptionName.clear();
        conf.autoStartConfig.connectionName.clear();
        SetGlobalConfig(conf);
    }

    groupBox_2->setEnabled(subscriptionList->count() > 0);
    SaveConfig();
}

void SubscribeEditor::on_subscriptionList_currentRowChanged(int currentRow)
{
    if (subscriptionList->count() == 0) {
        return;
    }

    if (currentRow < 0 && subscriptionList->count() > 0) {
        subscriptionList->setCurrentRow(0);
    }

    currentSubName = subscriptionList->currentItem()->text();
    LOG(MODULE_UI, "Subscription row changed, new name: " + currentSubName.toStdString())
    //
    subNameTxt->setText(currentSubName);
    subAddrTxt->setText(subscriptions[currentSubName]);
    //
    connectionsList->clear();
    auto _list = GetSubscriptionConnection(currentSubName.toStdString());

    for (auto i = 0; i < _list.count(); i++) {
        connectionsList->addItem(_list.keys()[i]);
    }
}

void SubscribeEditor::SaveConfig()
{
    auto conf = GetGlobalConfig();
    QMap<string, string> newConf;

    for (auto _ : subscriptions.toStdMap()) {
        if (!_.second.isEmpty()) {
            newConf[_.first.toStdString()] = _.second.toStdString();
        }
    }

    conf.subscribes = newConf.toStdMap();
    SetGlobalConfig(conf);
}

void SubscribeEditor::on_buttonBox_accepted()
{
    SaveConfig();
}

void SubscribeEditor::on_subscriptionList_itemSelectionChanged()
{
    groupBox_2->setEnabled(subscriptionList->selectedItems().count() > 0);
}
