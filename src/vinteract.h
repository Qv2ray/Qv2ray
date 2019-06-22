#ifndef VINTERACT_H
#define VINTERACT_H
#include <QString>
#include <QProcess>

bool validationCheck(QString path);

class v2Instance
{
public:
    explicit v2Instance();
    bool start(QWidget *parent);
    void stop();
    void restart();
    QProcess *vProcess;
    ~v2Instance();
};

#endif // VINTERACT_H
