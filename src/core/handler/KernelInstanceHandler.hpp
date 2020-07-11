#pragma once
#include "components/plugins/QvPluginHost.hpp"
#include "core/CoreUtils.hpp"
#include "core/kernel/V2RayKernelInteractions.hpp"

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
        const QMap<QString, InboundInfoObject> GetInboundInfo() const
        {
            return inboundInfo;
        }

      signals:
        void OnConnected(const ConnectionGroupPair &id);
        void OnDisconnected(const ConnectionGroupPair &id);
        void OnCrashed(const ConnectionGroupPair &id, const QString &errMessage);
        void OnStatsDataAvailable(const ConnectionGroupPair &id, const QMap<StatisticsType, QvStatsSpeed> &data);
        void OnKernelLogAvailable(const ConnectionGroupPair &id, const QString &log);

      private slots:
        void OnKernelCrashed_p(const QString &msg);
        void OnKernelLog_p(const QString &log);
        void OnV2RayStatsDataRcvd_p(const QMap<StatisticsType, QvStatsSpeed> &data);
        void OnPluginStatsDataRcvd_p(const long uploadSpeed, const long downloadSpeed);

      private:
        static std::optional<QString> CheckPort(const QMap<QString, InboundInfoObject> &info, int plugins);

      private:
        QMap<QString, int> GetInboundPorts() const
        {
            QMap<QString, int> result;
            for (const auto &[tag, info] : inboundInfo.toStdMap())
            {
                result[tag] = info.port;
            }
            return result;
        }
        QMap<QString, QString> GetInboundHosts() const
        {
            QMap<QString, QString> result;
            for (const auto &[tag, info] : inboundInfo.toStdMap())
            {
                result[tag] = info.listenIp;
            }
            return result;
        }

        QMap<QString, QString> outboundKernelMap;
        // Since QMap does not support std::unique_ptr, we use std::map<>
        std::list<std::pair<QString, std::unique_ptr<QvPluginKernel>>> activeKernels;
        QMap<QString, InboundInfoObject> inboundInfo;
        V2RayKernelInstance *vCoreInstance = nullptr;
        ConnectionGroupPair currentId = {};
    };
    inline const KernelInstanceHandler *KernelInstance;
} // namespace Qv2ray::core::handler
