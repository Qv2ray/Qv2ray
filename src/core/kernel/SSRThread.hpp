#ifndef SSRTHREAD_HPP
#define SSRTHREAD_HPP
#ifndef _MSC_VER
#include <QThread>
#else
#include "core/kernel/QtUILog.hpp"
#endif

class SSRThread :
        #ifndef _MSC_VER
        public QThread
        #else
        public QtUILog
        #endif
{
    Q_OBJECT
public:
    explicit SSRThread();
    explicit SSRThread(
   int localPort,
   int remotePort,
   std::string local_addr,
   std::string remote_host,
   std::string method,
   std::string password,
   std::string obfs,
   std::string obfs_param,
   std::string protocol,
   std::string protocol_param,
   QString inboundTag
   );
   QString getInboundTag();
   void run() override;
   ~SSRThread() override;
        #ifndef _MSC_VER
signals:
    void OnDataReady(quint64 dataUp, quint64 dataDown);
    void onSSRThreadLog(QString);
#endif
private:
   int localPort;
   int remotePort;
   std::string local_addr;
   std::string remote_host;
   std::string method;
   std::string password;
   std::string obfs;
   std::string obfs_param;
   std::string protocol;
   std::string protocol_param;
   QString inboundTag;
};
#endif // SSRTHREAD_HPP
