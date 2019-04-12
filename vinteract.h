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
    void start(MainWindow *parent);
    void stop();
    QProcess *v2Process;
    ~v2Instance();
private:


};

#endif // VINTERACT_H
