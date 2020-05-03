#pragma once
#include "base/Qv2rayBase.hpp"
#include "core/CoreSafeTypes.hpp"
#include "core/kernel/QvKernelABIChecker.hpp"

#include <QProcess>

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
        // Speed
        qulonglong getTagSpeedUp(const QString &tag);
        qulonglong getTagSpeedDown(const QString &tag);
        qulonglong getAllSpeedUp();
        qulonglong getAllSpeedDown();
        //
        std::optional<QString> StartConnection(const CONFIGROOT &root);
        void StopConnection();
        bool KernelStarted = false;
        //
        static bool ValidateConfig(const QString &path);
        static bool ValidateKernel(const QString &vCorePath, const QString &vAssetsPath, QString *message);

      signals:
        void OnProcessErrored(const QString &errMessage);
        void OnProcessOutputReadyRead(const QString &output);
        void OnNewStatsDataArrived(const quint64 speedUp, const quint64 speedDown);

      private slots:
        void onAPIDataReady(const quint64 speedUp, const quint64 speedDown);

      private:
        APIWorker *apiWorker;
        QProcess *vProcess;
        bool apiEnabled;
    };
} // namespace Qv2ray::core::kernel

using namespace Qv2ray::core::kernel;
