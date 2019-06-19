#ifndef HVCONF_H
#define HVCONF_H

#include <QDialog>
#include <QJsonObject>
#include "mainwindow.h"

namespace Ui
{
    class inbound_settings_window;
}

class inbound_settings_window : public QDialog
{
    Q_OBJECT

public:
    explicit inbound_settings_window(MainWindow *parent = nullptr);
    ~inbound_settings_window();
    QJsonObject rootObj;
    MainWindow *parentMW;

private slots:
    void on_buttonBox_accepted();
    void on_httpCB_stateChanged(int arg1);
    void on_socksCB_stateChanged(int arg1);

private:
    Ui::inbound_settings_window *ui;
};

#endif // HVCONF_H
