#pragma once

#include <QDialog>
#include "base/Qv2rayBase.hpp"
#include "ui_w_SubscriptionManager.h"
#include "ui/messaging/QvMessageBus.hpp"
#include "core/CoreSafeTypes.hpp"

class SubscribeEditor : public QDialog, private Ui::w_SubscribeEditor
{
        Q_OBJECT

    public:
        explicit SubscribeEditor(QWidget *parent = nullptr);
        ~SubscribeEditor();
        QPair<QString, CONFIGROOT> GetSelectedConfig();

    public slots:
        QvMessageBusSlotDecl

    private slots:
        void on_addSubsButton_clicked();

        void on_updateButton_clicked();

        void on_removeSubsButton_clicked();

        void on_buttonBox_accepted();

        void on_subscriptionList_itemSelectionChanged();

        void on_subscriptionList_itemClicked(QTreeWidgetItem *item, int column);

    private:
        void StartUpdateSubscription(const QString &subscriptionName);
        void SaveConfig();

        bool isUpdateInProgress = false;
        GroupId currentSubId = NullGroupId;
};
