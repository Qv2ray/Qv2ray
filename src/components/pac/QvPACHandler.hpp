#pragma once

#include <QObject>
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
    class PACServer : public QObject
    {
        Q_OBJECT
      public:
        explicit PACServer();
        ~PACServer();
        void SetProxyString(const QString &proxyString);
        void StartListen();
        void StopServer();

        QString gfwFilePath;

      private:
        void onNewRequest(const httplib::Request &req, httplib::Response &rsp);
        bool isStarted;
        httplib::Server *pacServer;
        QString pacContent;
        QString proxyString;
    };
} // namespace Qv2ray::components::pac

using namespace Qv2ray::components;
using namespace Qv2ray::components::pac;
