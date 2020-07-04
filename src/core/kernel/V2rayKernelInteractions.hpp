#pragma once
#include "base/Qv2rayBase.hpp"
#include "core/kernel/QvKernelABIChecker.hpp"

class QProcess;

namespace Qv2ray::core::kernel
{
    class APIWorker;
    class V2rayKernelInstance : public QObject
    {
        Q_OBJECT
      public:
        explicit V2rayKernelInstance(QObject *parent = nullptr);
        ~V2rayKernelInstance() override;
        //
        std::optional<QString> StartConnection(const CONFIGROOT &root);
        void StopConnection();
        bool KernelStarted = false;
        //
        static bool ValidateConfig(const QString &path);
        static bool ValidateKernel(const QString &vCorePath, const QString &vAssetsPath, QString *message);
        static std::tuple<bool, std::optional<QString>> CheckAndSetCoreExecutableState(const QString &vCorePath);

      signals:
        void OnProcessErrored(const QString &errMessage);
        void OnProcessOutputReadyRead(const QString &output);
        void OnNewStatsDataArrived(const std::map<Qv2rayStatisticsType, std::pair<long, long>> &data);

      private slots:
        void onAPIDataReady(const std::map<Qv2rayStatisticsType, std::pair<long, long>> &data);

      private:
        APIWorker *apiWorker;
        QProcess *vProcess;
        bool apiEnabled;
    };
} // namespace Qv2ray::core::kernel

using namespace Qv2ray::core::kernel;
