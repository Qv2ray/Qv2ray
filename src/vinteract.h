#ifndef VINTERACT_H
#define VINTERACT_H
#include <QString>
#include <QProcess>
#include "mainwindow.h"

bool validationCheck(QString path);
class v2Instance
{
public:
    explicit v2Instance();
    bool start(MainWindow *parent);
    void stop();
    void restart();
    QProcess *v2Process;
    ~v2Instance();
private:


};

#endif // VINTERACT_H
