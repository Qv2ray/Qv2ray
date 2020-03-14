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
        explicit PACServer(QObject *parent = nullptr);
        ~PACServer();
        inline void setPACProxyString(const QString &proxyStr)
        {
            proxyString = proxyStr;
        }
        void stopServer();

      private:
        void run() override;
        QString proxyString;

      private:
        httplib::Server *server;
        static void pacRequestHandler(const httplib::Request &req, httplib::Response &rsp);
        static inline QString pacContent;
    };
} // namespace Qv2ray::components::pac

using namespace Qv2ray::components;
using namespace Qv2ray::components::pac;
