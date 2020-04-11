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

        std::optional<QString> StartConnection(const ConnectionId &id, const CONFIGROOT &root);
        void RestartConnection();
        void StopConnection();
        const ConnectionId CurrentConnection() const
        {
            return currentConnectionId;
        }
        bool isConnected(const ConnectionId &id) const
        {
            return id == currentConnectionId;
        }

      signals:
        void OnConnected(const ConnectionId &id);
        void OnDisconnected(const ConnectionId &id);
        void OnCrashed(const ConnectionId &id);
        void OnStatsDataAvailable(const ConnectionId &id, const quint64 uploadSpeed, const quint64 downloadSpeed);
        void OnKernelLogAvailable(const ConnectionId &id, const QString &log);

      private slots:
        void OnKernelCrashed_p();
        void OnKernelLogAvailable_p(const QString &log);
        void OnStatsDataArrived_p(const quint64 uploadSpeed, const quint64 downloadSpeed);

      private:
        QMap<QString, std::shared_ptr<QvPluginKernel>> kernels;
        QMap<QString, QvPluginKernel *> activeKernels;
        CONFIGROOT root;
        V2rayKernelInstance *vCoreInstance = nullptr;
        ConnectionId currentConnectionId = NullConnectionId;
        ConnectionId lastConnectionId = NullConnectionId;
    };
} // namespace Qv2ray::core::handlers
