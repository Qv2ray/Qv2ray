#include <QtCore>
#ifdef Q_OS_LINUX
#include "components/plugins/toolbar/QvToolbar.hpp"
#include "common/QvHelpers.hpp"
#include <QLocalSocket>
#include <QLocalServer>
namespace Qv2ray::components::plugins::Toolbar
{
    namespace _linux
    {
        static QThread *linuxWorkerThread;
        static QLocalServer *server;
        static volatile bool isExiting = false;

        void qobject_proxy()
        {
            QLocalSocket *socket = server->nextPendingConnection();

            if (!socket->waitForConnected() || !socket->waitForReadyRead())
                return;

            try {
                while (!isExiting && socket->isOpen() && socket->isValid() && socket->waitForReadyRead()) {
                    // CANNOT PROPERLY READ...
                    // Temp-ly fixed (but why and how?)
                    auto in = QString(socket->readAll());

                    if (!isExiting && !in.isEmpty()) {
                        auto out = GetAnswerToRequest(in);
                        //
                        socket->write(out.toUtf8());
                        socket->flush();
                    } else {
                        QThread::msleep(200);
                    }
                }
            }  catch (...) {
                LOG(PLUGIN, "Closing a broken socket.")
            }
        }
        void DataMessageQThread()
        {
            server = new QLocalServer();
            // BUG Sometimes failed to listen due to improper close of last session.
            bool listening = server->listen(QV2RAY_NETSPEED_PLUGIN_PIPE_NAME_LINUX);

            while (!isExiting && !listening) {
                QThread::msleep(500);
                listening = server->listen(QV2RAY_NETSPEED_PLUGIN_PIPE_NAME_LINUX);
            }

            bool timeOut = false;
            server->setSocketOptions(QLocalServer::WorldAccessOption);
            QObject::connect(server, &QLocalServer::newConnection, &qobject_proxy);

            while (!isExiting) {
                bool result = server->waitForNewConnection(5000, &timeOut);
                DEBUG(PLUGIN, "Plugin thread listening failed: " + server->errorString())
                DEBUG(PLUGIN, "waitForNewConnection: " + QString(result ? "true" : "false") + ", " + QString(timeOut ? "true" : "false"))
            }

            server->close();
        }
        void StartMessageQThread()
        {
            linuxWorkerThread = QThread::create(_linux::DataMessageQThread);
            linuxWorkerThread->start();
        }

        void StopMessageQThread()
        {
            isExiting = true;

            if (linuxWorkerThread->isRunning()) {
                LOG(PLUGIN, "Waiting for linuxWorkerThread to stop.")
                linuxWorkerThread->wait();
            }

            delete _linux::linuxWorkerThread;
        }
    }
}
#endif
