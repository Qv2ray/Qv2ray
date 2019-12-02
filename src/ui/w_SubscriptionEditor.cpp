#include "w_SubscriptionEditor.hpp"
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
    subscriptions = QMap<string, string>(conf.subscribes);
    LoadSubscriptionList(subscriptions);
}

void SubscribeEditor::LoadSubscriptionList(QMap<string, string> list)
{
    subscriptionList->clear();

    for (auto i = 0; i < list.count(); i++) {
        subscriptionList->addItem(QSTRING(list.keys()[i]));
    }
}

SubscribeEditor::~SubscribeEditor()
{
}

void SubscribeEditor::on_addSubsButton_clicked()
{
    auto key = QString::number(QTime::currentTime().msecsSinceStartOfDay());
    subscriptionList->addItem(key);
    subscriptions[key.toStdString()] = "http://example.com/myfile";
    SaveConfig();
}

void SubscribeEditor::on_updateButton_clicked()
{
    if (isUpdateInProgress) {
        QvMessageBox(this, tr("Update subscription in progress"), tr("Please try update later"));
        return;
    }

    isUpdateInProgress = true;
    //int index ;
    //auto name = ->item(index, 0)->text();
    //auto url = subscriptionList->item(index, 1)->text();
    //auto data = helper.syncget(url);
    //ProcessSubscriptionEntry(data, name);
}

void SubscribeEditor::ProcessSubscriptionEntry(QByteArray result, QString subsciptionName)
{
    auto content = DecodeSubscriptionString(result).trimmed();

    if (!content.isEmpty()) {
        auto vmessList = SplitLines(content);

        for (auto vmess : vmessList) {
            QString errMessage;
            QString _alias;
            auto config = ConvertConfigFromVMessString(vmess.trimmed(), &_alias, &errMessage);
        }

        isUpdateInProgress = false;
    }
}

void SubscribeEditor::on_removeSubsButton_clicked()
{
    if (subscriptionList->currentRow() < 0)
        return;

    subscriptions.remove(subscriptionList->currentItem()->text().toStdString());
    subscriptionList->takeItem(subscriptionList->currentRow());
    SaveConfig();
}

void SubscribeEditor::on_subscriptionList_currentRowChanged(int currentRow)
{
    if (currentRow < 0 && subscriptionList->count() > 1) {
        subscriptionList->setCurrentRow(0);
    }

    currentSubName = subscriptionList->currentItem()->text();
    //
    subNameTxt->setText(currentSubName);
    subAddrTxt->setPlainText(QSTRING(subscriptions[currentSubName.toStdString()]));
    //
    connectionsList->clear();
    auto _list = GetSubscriptionConnection(currentSubName.toStdString());

    for (auto i = 0; i < _list.count(); i++) {
        connectionsList->addItem(_list.keys()[i]);
    }
}

void SubscribeEditor::on_applyChangesBtn_clicked()
{
    auto newName = subNameTxt->text();
    auto newAddress = subAddrTxt->toPlainText();

    if (currentSubName != newName) {
        // Rename needed.
        LOG(MODULE_SUBSCRIPTION, "Renaming a subscription, from " + currentSubName.toStdString() + " to: " + newName.toStdString())

        if (subscriptionList->findItems(newName, Qt::MatchExactly).count() > 0) {
            QvMessageBox(this, tr("Renaming a subscription"), tr("New name of this subscription has been used already, please suggest a new name"));
            return;
        }

        bool result = RenameSubscription(currentSubName, newName);

        if (!result) {
            QvMessageBox(this, tr("Renaming a subscription"), tr("Failed to rename a subscription, this is an unknown error."));
            return;
        }

        subscriptions[newName.toStdString()] = subscriptions[currentSubName.toStdString()];
        subscriptions.remove(currentSubName.toStdString());
        LoadSubscriptionList(subscriptions);
        QvMessageBox(this, tr("Renaming a subscription"), tr("Successfully renamed a subscription"));
        currentSubName = newName;
    }

    if (subscriptions[currentSubName.toStdString()] != newAddress.toStdString()) {
        LOG(MODULE_SUBSCRIPTION, "Setting new address, from " + subscriptions[currentSubName.toStdString()] + " to: " + newAddress.toStdString())
        subscriptions[currentSubName.toStdString()] = newAddress.toStdString();

        if (QvMessageBoxAsk(this, tr("Setting new subscription address"), tr("You have changed the address of a subscription") + NEWLINE +
                            tr("Would you like to update this subscription?")) == QMessageBox::Yes) {
            // TODO UPDATE
        }
    }

    SaveConfig();
}

void SubscribeEditor::SaveConfig()
{
    auto conf = GetGlobalConfig();
    conf.subscribes = subscriptions.toStdMap();
    SetGlobalConfig(conf);
}
