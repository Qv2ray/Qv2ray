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
        void on_buttonBox_accepted();

        void on_addSubsButton_clicked();

        void on_updateButton_clicked();

        void on_updateAllButton_clicked();

    signals:
        void s_update_config();

    private:
        void ProcessSubscriptionEntry(QByteArray result, QString subsciptionName);

        bool isUpdateInProgress = false;
        QvHttpRequestHelper helper;
        QMap<QString, QList<QJsonObject>> subscriptions;
};

#endif // W_SUBSCRIBEEDITOR_H
