#ifndef HVCONF_H
#define HVCONF_H

#include <QDialog>
#include <QJsonObject>
#include "mainwindow.h"

namespace Ui
{
    class hvConf;
}

class hvConf : public QDialog
{
    Q_OBJECT

public:
    explicit hvConf(MainWindow *parent = nullptr);
    ~hvConf();
    QJsonObject rootObj;
    MainWindow *parentMW;

private slots:
    void on_buttonBox_accepted();
    void on_httpCB_stateChanged(int arg1);
    void on_socksCB_stateChanged(int arg1);

private:
    Ui::hvConf *ui;
};

#endif // HVCONF_H
