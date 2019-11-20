#include <QString>
#include <QDebug>
#include "QvPingModel.hpp"

namespace Qv2ray
{
    namespace Utils
    {
        QvPingModel::QvPingModel(QObject *parent) :
            QObject(parent),
            running(false)
        {
            pingworker = new QProcess(this);
            connect(pingworker, &QProcess::started, this, &QvPingModel::verifyStatus);
            connect(pingworker, SIGNAL(finished(int)), this, SLOT(readResult()));
        }

        QvPingModel::~QvPingModel()
        {
            //
        }

        void QvPingModel::verifyStatus()
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

        void QvPingModel::readResult()
        {
            qDebug() << "Acabou!!!";
            running = false;
            qDebug() << "LENDO: " << pingworker->readLine();
        }

        void QvPingModel::start_command()
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

        bool QvPingModel::is_running()
        {
            return running;
        }

        bool QvPingModel::finished()
        {
            return pingworker->atEnd();
        }

    }
}
