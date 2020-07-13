#pragma once
#include "base/Qv2rayBase.hpp"
#include "core/kernel/QvKernelABIChecker.hpp"

class QProcess;

namespace Qv2ray::core::kernel
{
    class APIWorker;
    class V2RayKernelInstance : public QObject
    {
        Q_OBJECT
      public:
        explicit V2RayKernelInstance(QObject *parent = nullptr);
        ~V2RayKernelInstance() override;
        //
        std::optional<QString> StartConnection(const CONFIGROOT &root);
        void StopConnection();
        bool KernelStarted = false;
        //
        static bool ValidateConfig(const QString &path);
        static bool ValidateKernel(const QString &vCorePath, const QString &vAssetsPath, QString *message);
        static std::pair<bool, std::optional<QString>> CheckAndSetCoreExecutableState(const QString &vCorePath);

      signals:
        void OnProcessErrored(const QString &errMessage);
        void OnProcessOutputReadyRead(const QString &output);
        void OnNewStatsDataArrived(const QMap<StatisticsType, QvStatsSpeed> &data);

      private:
        APIWorker *apiWorker;
        QProcess *vProcess;
        bool apiEnabled;
    };
} // namespace Qv2ray::core::kernel

using namespace Qv2ray::core::kernel;
