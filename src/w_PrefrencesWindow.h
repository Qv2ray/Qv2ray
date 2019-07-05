#ifndef HVCONF_H
#define HVCONF_H

#include <QDialog>
#include <ui_w_PrefrencesWindow.h>
#include "HConfigObjects.hpp"

namespace Hv2ray
{
    namespace Ui
    {
        class PrefrencesWindow : public QDialog
        {
                Q_OBJECT

            public:
                explicit PrefrencesWindow(QWidget *parent = nullptr);
                ~PrefrencesWindow();
                QWidget *parentMW;

            private slots:
                void on_buttonBox_accepted();
                void on_httpCB_stateChanged(int arg1);
                void on_socksCB_stateChanged(int arg1);

                void on_httpAuthCB_stateChanged(int arg1);

                void on_runAsRootCheckBox_stateChanged(int arg1);

            private:
                Hv2ray::HConfigModels::Hv2Config CurrentConfig;
                Ui_PrefrencesWindow *ui;
        };
    }
}
#endif // HVCONF_H
