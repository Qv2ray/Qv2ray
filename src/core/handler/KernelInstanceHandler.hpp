#pragma once
#include "components/plugins/QvPluginHost.hpp"
#include "core/kernel/V2rayKernelInteractions.hpp"

#include <QObject>
#include <optional>

namespace Qv2ray::core::handler
{
    class KernelInstanceHandler : public QObject
    {
        Q_OBJECT
      public:
        explicit KernelInstanceHandler(QObject *parent = nullptr);
        ~KernelInstanceHandler();

        std::optional<QString> StartConnection(const ConnectionGroupPair &id, CONFIGROOT root);
        void StopConnection();
        const ConnectionGroupPair CurrentConnection() const
        {
            return currentId;
        }
        int ActivePluginKernelsCount() const
        {
            return activeKernels.size();
        }
        const QMap<QString, int> InboundPorts() const
        {
            return inboundPorts;
        }
        const QMap<QString, QString> InboundHosts() const
        {
            return inboundHosts;
        }

      signals:
        void OnConnected(const ConnectionGroupPair &id);
        void OnDisconnected(const ConnectionGroupPair &id);
        void OnCrashed(const ConnectionGroupPair &id, const QString &errMessage);
        void OnStatsDataAvailable(const ConnectionGroupPair &id, const quint64 uploadSpeed, const quint64 downloadSpeed);
        void OnKernelLogAvailable(const ConnectionGroupPair &id, const QString &log);

      private slots:
        void OnKernelCrashed_p(const QString &msg);
        void OnKernelLog_p(const QString &log);
        void OnStatsDataRcvd_p(const quint64 uploadSpeed, const quint64 downloadSpeed);

      private:
        static std::optional<QString> CheckPort(QMap<QString, QString> hosts, QMap<QString, int> ports, int plugins);

      private:
        QMap<QString, QString> outboundKernelMap;
        // Since QMap does not support std::unique_ptr, we use std::map<>
        std::map<QString, std::unique_ptr<QvPluginKernel>> activeKernels;
        QMap<QString, int> inboundPorts;
        QMap<QString, QString> inboundHosts;
        V2rayKernelInstance *vCoreInstance = nullptr;
        ConnectionGroupPair currentId = {};
    };
    inline const KernelInstanceHandler *KernelInstance;
} // namespace Qv2ray::core::handler
