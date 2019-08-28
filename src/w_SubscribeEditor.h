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
        void httpReqCallBack(QByteArray result);

    signals:
        void s_update_config();

    private:
        Ui::w_SubscribeEditor *ui;
        QvHttpRequestHelper helper;
};

#endif // W_SUBSCRIBEEDITOR_H
