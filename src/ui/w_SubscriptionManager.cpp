#include "w_SubscriptionManager.hpp"

#include "common/QvHelpers.hpp"
#include "core/config/ConfigBackend.hpp"
#include "core/handler/ConnectionHandler.hpp"

SubscribeEditor::SubscribeEditor(QWidget *parent) : QDialog(parent)
{
    setupUi(this);
    QvMessageBusConnect(SubscribeEditor);
    addSubsButton->setIcon(QICON_R("add.png"));
    removeSubsButton->setIcon(QICON_R("delete.png"));

    for (auto subs : ConnectionManager->Subscriptions())
    {
        subscriptionList->addTopLevelItem(new QTreeWidgetItem(QStringList()                              //
                                                              << ConnectionManager->GetDisplayName(subs) //
                                                              << subs.toString()));                      //
    }
}

QvMessageBusSlotImpl(SubscribeEditor)
{
    switch (msg)
    {
        MBShowDefaultImpl MBHideDefaultImpl MBRetranslateDefaultImpl
    }
}

QPair<QString, CONFIGROOT> SubscribeEditor::GetSelectedConfig()
{
    return QPair<QString, CONFIGROOT>(ConnectionManager->GetDisplayName(currentConnectionId),
                                      ConnectionManager->GetConnectionRoot(currentConnectionId));
}

SubscribeEditor::~SubscribeEditor()
{
}

void SubscribeEditor::on_addSubsButton_clicked()
{
    auto const key = QSTRN(QTime::currentTime().msecsSinceStartOfDay());
    auto id = ConnectionManager->CreateGroup(key, true);
    //
    subscriptionList->addTopLevelItem(new QTreeWidgetItem(QStringList()       //
                                                          << key              //
                                                          << id.toString())); //
}

void SubscribeEditor::on_updateButton_clicked()
{
    if (QvMessageBoxAsk(this, tr("Reload Subscription"), tr("Would you like to reload the subscription?")) == QMessageBox::Yes)
    {
        this->setEnabled(false);
        ConnectionManager->UpdateSubscription(currentSubId, withProxyCB->isChecked()); //
        this->setEnabled(true);
        on_subscriptionList_itemClicked(subscriptionList->currentItem(), 0);
    }
}

void SubscribeEditor::on_removeSubsButton_clicked()
{
    if (QvMessageBoxAsk(this, tr("Deleting a subscription"), tr("All connections will be moved to default group, do you want to continue?")) ==
        QMessageBox::Yes)
    {
        ConnectionManager->DeleteGroup(currentSubId); //
        auto item = subscriptionList->currentItem();
        subscriptionList->removeItemWidget(item, 0);
        delete item;
        if (subscriptionList->topLevelItemCount() > 0)
        {
            subscriptionList->setCurrentItem(subscriptionList->topLevelItem(0));
            on_subscriptionList_itemClicked(subscriptionList->topLevelItem(0), 0);
        }
        else
        {
            groupBox_2->setEnabled(false);
        }
    }
}

void SubscribeEditor::on_buttonBox_accepted()
{
    // Nothing?
}

void SubscribeEditor::on_subscriptionList_itemSelectionChanged()
{
    groupBox_2->setEnabled(subscriptionList->selectedItems().count() > 0);
}

void SubscribeEditor::on_subscriptionList_itemClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column)

    if (item == nullptr)
    {
        return;
    }

    //
    groupBox_2->setEnabled(true);
    currentSubId = GroupId(item->text(1));
    //
    subNameTxt->setText(ConnectionManager->GetDisplayName(currentSubId));
    auto [addr, lastUpdated, updateInterval] = ConnectionManager->GetSubscriptionData(currentSubId);
    subAddrTxt->setText(addr);
    lastUpdatedLabel->setText(timeToString(lastUpdated));
    updateIntervalSB->setValue(updateInterval);
    //
    connectionsList->clear();

    for (auto conn : ConnectionManager->Connections(currentSubId))
    {
        connectionsList->addItem(ConnectionManager->GetDisplayName(conn)); //
    }
}

void SubscribeEditor::on_subscriptionList_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    on_subscriptionList_itemClicked(current, 0);
    Q_UNUSED(previous)
}

void SubscribeEditor::on_subNameTxt_textEdited(const QString &arg1)
{
    subscriptionList->selectedItems().first()->setText(0, arg1);
    ConnectionManager->RenameGroup(currentSubId, arg1.trimmed());
}

void SubscribeEditor::on_subAddrTxt_textEdited(const QString &arg1)
{
    auto newUpdateInterval = updateIntervalSB->value();
    ConnectionManager->SetSubscriptionData(currentSubId, arg1, newUpdateInterval);
}

void SubscribeEditor::on_updateIntervalSB_valueChanged(double arg1)
{
    auto newAddress = subAddrTxt->text().trimmed();
    ConnectionManager->SetSubscriptionData(currentSubId, newAddress, arg1);
}

void SubscribeEditor::on_connectionsList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    Q_UNUSED(previous)
    currentConnectionId = ConnectionManager->GetConnectionIdByDisplayName(current->text(), currentSubId);
}
