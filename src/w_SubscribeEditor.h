#ifndef W_SUBSCRIBEEDITOR_H
#define W_SUBSCRIBEEDITOR_H

#include <QDialog>
#include "QvUtils.h"
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

    signals:
        void s_update_config();

    private:
        Ui::w_SubscribeEditor *ui;
};

#endif // W_SUBSCRIBEEDITOR_H
