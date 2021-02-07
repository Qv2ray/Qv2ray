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
        size_t ActivePluginKernelsCount() const
        {
            return activeKernels.size();
        }
        const QMap<QString, ProtocolSettingsInfoObject> GetCurrentConnectionInboundInfo() const
        {
            return inboundInfo;
        }
        const QStringList GetActiveKernelProtocols() const
        {
            QStringList list;
            for (const auto &[protocol, kernel] : activeKernels)
            {
                list << protocol;
            }
            return list;
        }

      signals:
        void OnConnected(const ConnectionGroupPair &id);
        void OnDisconnected(const ConnectionGroupPair &id);
        void OnCrashed(const ConnectionGroupPair &id, const QString &errMessage);
        void OnStatsDataAvailable(const ConnectionGroupPair &id, const QMap<StatisticsType, QvStatsSpeed> &data);
        void OnKernelLogAvailable(const ConnectionGroupPair &id, const QString &log);

      private slots:
        void OnKernelCrashed_p(const QString &msg);
        void OnPluginKernelLog_p(const QString &log);
        void OnV2RayKernelLog_p(const QString &log);
        void OnV2RayStatsDataRcvd_p(const QMap<StatisticsType, QvStatsSpeed> &data);
        void OnPluginStatsDataRcvd_p(const long uploadSpeed, const long downloadSpeed);

      private:
        void emitLogMessage(const QString &);
        static std::optional<QString> CheckPort(const QMap<QString, ProtocolSettingsInfoObject> &info, int plugins);

      private:
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        int pluginLogPrefixPadding = 0;
#else
        qsizetype pluginLogPrefixPadding = 0;
#endif
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
                result[tag] = info.address;
            }
            return result;
        }

        QMap<QString, QString> kernelMap;
        // Since QMap does not support std::unique_ptr, we use std::map<>
        std::list<std::pair<QString, std::unique_ptr<PluginKernel>>> activeKernels;
        QMap<QString, ProtocolSettingsInfoObject> inboundInfo;
        V2RayKernelInstance *vCoreInstance = nullptr;
        ConnectionGroupPair currentId = {};
    };
    inline const KernelInstanceHandler *KernelInstance;
} // namespace Qv2ray::core::handler
