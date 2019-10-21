#include "QvNetSpeedPlugin.h"
#include "QvUtils.h"
#include <QLocalSocket>
#include <QLocalServer>
namespace Qv2ray
{
    namespace Utils
    {
        namespace NetSpeedPlugin
        {
#ifdef __linux__
            namespace _linux
            {
                static QThread *linuxWorkerThread;
                static QLocalServer *server;
                static QObjectMessageProxy *messageProxy;
                static bool isExiting = false;

                void StartMessageQThread()
                {
                    linuxWorkerThread = QThread::create(_linux::DataMessageQThread);
                    linuxWorkerThread->start();
                }

                void StopMessageQThread()
                {
                    isExiting = true;
                    linuxWorkerThread->wait();
                    delete _linux::linuxWorkerThread;
                }

                void qobject_proxy()
                {
                    QLocalSocket *socket = server->nextPendingConnection();

                    if (!socket->waitForConnected() || !socket->waitForReadyRead())
                        return;

                    try {
                        while (!isExiting && socket->isOpen() && socket->isValid() && socket->waitForReadyRead()) {
                            // BUG CANNOT PROPERLY READ...
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
                        LOG(MODULE_PLUGIN, "Closing a broken socket.")
                    }
                }
                void DataMessageQThread()
                {
                    server = new QLocalServer();
                    // BUG Sometimes failed to listen due to improper close of last session.
                    server->listen("Qv2ray_NetSpeed_Widget_LocalSocket");
                    bool timeOut = false;
                    server->setSocketOptions(QLocalServer::WorldAccessOption);
                    messageProxy = new QObjectMessageProxy(&qobject_proxy);
                    QObject::connect(server, &QLocalServer::newConnection, messageProxy, &QObjectMessageProxy::processMessage);

                    while (!isExiting) {
                        bool result = server->waitForNewConnection(100, &timeOut);
                        auto str  = server->errorString();
                    }

                    server->close();
                }
            }
#endif
        }
    }
}
