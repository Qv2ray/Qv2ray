#pragma once
#include "components/plugins/QvPluginHost.hpp"
#include "core/CoreSafeTypes.hpp"
#include "core/kernel/V2rayKernelInteractions.hpp"

#include <QObject>
#include <optional>

namespace Qv2ray::core::handlers
{
    class KernelInstanceHandler : public QObject
    {
        Q_OBJECT
      public:
        explicit KernelInstanceHandler(QObject *parent = nullptr);
        ~KernelInstanceHandler();

        std::optional<QString> StartConnection(const ConnectionGroupPair &id, const CONFIGROOT &root);
        void RestartConnection();
        void StopConnection();
        const ConnectionGroupPair CurrentConnection() const
        {
            return currentId;
        }
        bool isConnected(const ConnectionGroupPair &id) const
        {
            return id == currentId;
        }
        const QMap<QString, int> InboundPorts() const
        {
            return inboundPorts;
        }

      signals:
        void OnConnected(const ConnectionGroupPair &id);
        void OnDisconnected(const ConnectionGroupPair &id);
        void OnCrashed(const ConnectionGroupPair &id, const QString &errMessage);
        void OnStatsDataAvailable(const ConnectionId &id, const quint64 uploadSpeed, const quint64 downloadSpeed);
        void OnKernelLogAvailable(const ConnectionId &id, const QString &log);

      private slots:
        void OnKernelCrashed_p(const QString &msg);
        void OnKernelLogAvailable_p(const QString &log);
        void OnStatsDataArrived_p(const quint64 uploadSpeed, const quint64 downloadSpeed);

      private:
        QMap<QString, QvPluginKernel *> kernels;
        QMap<QString, QvPluginKernel *> activeKernels;
        QMap<QString, int> inboundPorts;
        CONFIGROOT root;
        V2rayKernelInstance *vCoreInstance = nullptr;
        ConnectionGroupPair currentId = {};
        ConnectionGroupPair lastConnectionId = {};
    };
    inline const KernelInstanceHandler *KernelInstance;
} // namespace Qv2ray::core::handlers
