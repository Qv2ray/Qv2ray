#ifndef CONFEDIT_H
#define CONFEDIT_H

#include <QtCore>
#include <QDialog>
#include "QvCoreConfigObjects.h"
#include "ui_w_ConnectionEditWindow.h"

namespace Ui
{
    class ConnectionEditWindow;
}

class ConnectionEditWindow : public QDialog
{
        Q_OBJECT

    public:
        explicit ConnectionEditWindow(QWidget *parent = nullptr);
        explicit ConnectionEditWindow(QJsonObject editRootObject, QWidget *parent = nullptr);
        ~ConnectionEditWindow();
    signals:
        void s_reload_config();
    private slots:
        void on_buttonBox_accepted();

    private:
        QJsonObject editRootJson;
        VMessOut vmess;
        Ui::ConnectionEditWindow *ui;
};
#endif // CONFEDIT_H
