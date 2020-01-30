#pragma once

#include <QDialog>
#include "core/CoreUtils.hpp"
#include "common/HTTPRequestHelper.hpp"

#include "ui_w_SubscriptionManager.h"
class SubscribeEditor : public QDialog, private Ui::w_SubscribeEditor
{
        Q_OBJECT

    public:
        explicit SubscribeEditor(QWidget *parent = nullptr);
        ~SubscribeEditor();
        QPair<QString, CONFIGROOT> GetSelectedConfig();

    private slots:
        void on_addSubsButton_clicked();

        void on_updateButton_clicked();

        void on_removeSubsButton_clicked();

        void on_subscriptionList_currentRowChanged(int currentRow);

        void on_buttonBox_accepted();

        void on_subscriptionList_itemSelectionChanged();

        void on_updateIntervalSB_valueChanged(double arg1);

        void on_connectionsList_itemClicked(QListWidgetItem *item);

    private:
        void StartUpdateSubscription(const QString &subscriptionName);
        void SaveConfig();
        void LoadSubscriptionList(QMap<QString, Qv2raySubscriptionConfig> list);

        bool isUpdateInProgress = false;
        QvHttpRequestHelper helper;
        QPair<QString, CONFIGROOT> currentSelectedConfig;
        QMap<QString, Qv2raySubscriptionConfig> subscriptions;
        QString currentSubName;
};
