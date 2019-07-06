#ifndef HVCONF_H
#define HVCONF_H

#include <QDialog>
#include <ui_w_PrefrencesWindow.h>
#include "QvConfigObjects.hpp"

namespace Ui
{
    class PrefrencesWindow;
}

class PrefrencesWindow : public QDialog
{
        Q_OBJECT

    public:
        explicit PrefrencesWindow(QWidget *parent = nullptr);
        ~PrefrencesWindow();
        QWidget *parentMW;
    signals:
        void s_reload_config();
    private slots:
        void on_buttonBox_accepted();
        void on_httpCB_stateChanged(int arg1);
        void on_socksCB_stateChanged(int arg1);

        void on_httpAuthCB_stateChanged(int arg1);

        void on_runAsRootCheckBox_stateChanged(int arg1);

    private:
        Qv2ray::QvConfigModels::Qv2Config CurrentConfig;
        Ui::PrefrencesWindow *ui;
};
#endif // HVCONF_H
