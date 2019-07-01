#ifndef VINTERACT_H
#define VINTERACT_H
#include <QString>
#include <QProcess>

enum V2RAY_INSTANCE_STARTUP_STATUS {
    STOPPED,
    STARTING,
    STARTED
};

class v2Instance
{
public:
    explicit v2Instance(QWidget *parent);

    bool start();
    void stop();
    void restart();

    static bool checkVCoreExes();
    static bool checkConfigFile(QString path);
    ~v2Instance();
    QProcess *vProcess;
private:
    V2RAY_INSTANCE_STARTUP_STATUS processStatus;
};

#endif // VINTERACT_H
