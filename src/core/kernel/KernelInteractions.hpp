#pragma once
#include "base/Qv2rayBase.hpp"
#include "core/CoreSafeTypes.hpp"

#include <QProcess>

namespace Qv2ray::core::kernel
{
    class APIWorker;
    class V2rayKernelInstance : public QObject
    {
        Q_OBJECT
      public:
        explicit V2rayKernelInstance();
        ~V2rayKernelInstance() override;
        //
        // Speed
        qulonglong getTagSpeedUp(const QString &tag);
        qulonglong getTagSpeedDown(const QString &tag);
        qulonglong getAllSpeedUp();
        qulonglong getAllSpeedDown();
        //
        optional<QString> StartConnection(const ConnectionId &id, const CONFIGROOT &root);
        void StopConnection();
        bool KernelStarted = false;
        //
        static bool ValidateConfig(const QString &path);
        static bool ValidateKernel(const QString &vCorePath, const QString &vAssetsPath, QString *message);

      signals:
        void OnProcessErrored(const ConnectionId &id);
        void OnProcessOutputReadyRead(const ConnectionId &id, const QString &output);
        void OnNewStatsDataArrived(const ConnectionId &id, const quint64 _totalUp, const quint64 _totalDown);

      public slots:
        void onAPIDataReady(const quint64 _totalUp, const quint64 _totalDown);

      private:
        APIWorker *apiWorker;
        QProcess *vProcess;
        bool apiEnabled;
        //
        ConnectionId id = NullConnectionId;
    };
} // namespace Qv2ray::core::kernel

using namespace Qv2ray::core::kernel;
