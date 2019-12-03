#ifndef W_SUBSCRIBEEDITOR_H
#define W_SUBSCRIBEEDITOR_H

#include <QDialog>
#include "QvUtils.hpp"
#include "QvHTTPRequestHelper.hpp"

#include "ui_w_SubscriptionEditor.h"
class SubscribeEditor : public QDialog, private Ui::w_SubscribeEditor
{
        Q_OBJECT

    public:
        explicit SubscribeEditor(QWidget *parent = nullptr);
        ~SubscribeEditor();

    private slots:
        void on_addSubsButton_clicked();

        void on_updateButton_clicked();

        void on_removeSubsButton_clicked();

        void on_subscriptionList_currentRowChanged(int currentRow);

        void on_applyChangesBtn_clicked();

        void on_buttonBox_accepted();

    private:
        void StartUpdateSubscription(const QString &subscriptionName);
        void SaveConfig();
        void LoadSubscriptionList(QMap<QString, QString> list);

        bool isUpdateInProgress = false;
        QvHttpRequestHelper helper;
        QMap<QString, QString> subscriptions;
        QString currentSubName;
};

#endif // W_SUBSCRIBEEDITOR_H
