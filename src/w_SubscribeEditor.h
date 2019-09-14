#ifndef W_SUBSCRIBEEDITOR_H
#define W_SUBSCRIBEEDITOR_H

#include <QDialog>
#include "QvUtils.h"
#include "QvHTTPRequestHelper.h"

namespace Ui
{
    class w_SubscribeEditor;
}

class SubscribeEditor : public QDialog
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
        Ui::w_SubscribeEditor *ui;
        QvHttpRequestHelper helper;
        QMap<QString, QList<QJsonObject>> subscriptions;
};

#endif // W_SUBSCRIBEEDITOR_H
