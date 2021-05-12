#pragma once
#include "base/Qv2rayBase.hpp"

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
        bool IsKernelRunning() const
        {
            return kernelStarted;
        }
        //
        static std::optional<QString> ValidateConfig(const QString &path);
        static std::pair<bool, std::optional<QString>> ValidateKernel(const QString &vCorePath, const QString &vAssetsPath);

      signals:
        void OnProcessErrored(const QString &errMessage);
        void OnProcessOutputReadyRead(const QString &output);
        void OnNewStatsDataArrived(const QMap<StatisticsType, QvStatsSpeed> &data);

      private:
        APIWorker *apiWorker;
        QProcess *vProcess;
        bool apiEnabled;
        bool kernelStarted = false;
    };
} // namespace Qv2ray::core::kernel

using namespace Qv2ray::core::kernel;
