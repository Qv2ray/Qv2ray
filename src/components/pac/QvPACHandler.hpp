#pragma once

#include <QObject>
#include <QThread>
#include <memory>

namespace httplib
{
    class Server;
    struct Request;
    struct Response;
} // namespace httplib

namespace Qv2ray::components::pac
{
    QString ConvertGFWToPAC(const QString &rawContent, const QString &customProxyString);
    class PACServer : public QThread
    {
        Q_OBJECT
      public:
        explicit PACServer();
        ~PACServer();
        void SetProxyString(const QString &proxyString);
        void StartListen()
        {
            start();
        }
        void StopServer();

        QString gfwFilePath;

      private:
        void run() override;
        httplib::Server *pacServer;
        QString proxyString;

      private:
        static void onNewRequest(const httplib::Request &req, httplib::Response &rsp);
        static inline QString pacContent;
    };
} // namespace Qv2ray::components::pac

using namespace Qv2ray::components;
using namespace Qv2ray::components::pac;
