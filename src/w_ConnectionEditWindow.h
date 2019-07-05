#ifndef CONFEDIT_H
#define CONFEDIT_H

#include <QDialog>
#include "ui_w_ConnectionEditWindow.h"

namespace Qv2ray
{
    namespace Ui_Impl
    {
        class ConnectionEditWindow : public QDialog
        {
                Q_OBJECT

            public:
                explicit ConnectionEditWindow(QWidget *parent = nullptr);
                ~ConnectionEditWindow();

            private:
                Ui_ConnectionEditWindow *ui;
        };
    }
}
#endif // CONFEDIT_H
