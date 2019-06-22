#ifndef HVCONF_H
#define HVCONF_H

#include <QDialog>
#include <QJsonObject>
#include "MainWindow.h"

namespace Ui
{
    class PrefrencesWindow;
}

class PrefrencesWindow : public QDialog
{
    Q_OBJECT

public:
    explicit PrefrencesWindow(MainWindow *parent = nullptr);
    ~PrefrencesWindow();
    QJsonObject rootObj;
    MainWindow *parentMW;

private slots:
    void on_buttonBox_accepted();
    void on_httpCB_stateChanged(int arg1);
    void on_socksCB_stateChanged(int arg1);

private:
    Ui::PrefrencesWindow *ui;
};

#endif // HVCONF_H
