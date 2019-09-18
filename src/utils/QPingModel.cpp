#include <QString>
#include <QDebug>
#include "QPingModel.h"

namespace Qv2ray
{
    namespace Utils
    {
        PingModel::PingModel(QObject *parent) :
            QObject(parent),
            running(false)
        {
            pingworker = new QProcess(this);
            connect(pingworker, SIGNAL(started()), this, SLOT(verifyStatus()));
            connect(pingworker, SIGNAL(finished(int)), this, SLOT(readResult()));
        }

        PingModel::~PingModel()
        {
        }

        void PingModel::verifyStatus()
        {
            if (pingworker->isReadable()) {
                qDebug() << "read on ...";
                connect(pingworker, SIGNAL(readyRead()), this, SLOT(readResult()));

                if (pingworker->canReadLine()) {
                    qDebug() << "LINE read on ...";
                }
            } else {
                qDebug() << "not able to read ...";
            }
        }

        void PingModel::readResult()
        {
            qDebug() << "Acabou!!!";
            running = false;
            qDebug() << "LENDO: " << pingworker->readLine();
        }

        void PingModel::start_command()
        {
            if (pingworker) {
                QString command = "ping";
                QStringList args;
                args << "-w" <<  "3" <<  "www.google.com";
                pingworker->start(command, args);
                pingworker->waitForStarted(7000);
                running = true;
                pingworker->waitForFinished(5000);
            }
        }

        bool PingModel::is_running()
        {
            return running;
        }

        bool PingModel::finished()
        {
            return pingworker->atEnd();
        }

    }
}
