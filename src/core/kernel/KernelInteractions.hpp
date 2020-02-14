#pragma once
#include <QProcess>
#include "base/Qv2rayBase.hpp"
#include "APIBackend.hpp"

namespace Qv2ray::core::kernel
{
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
            qulonglong getTagDataUp(const QString &tag);
            qulonglong getTagDataDown(const QString &tag);
            qulonglong getAllDataUp();
            qulonglong getAllDataDown();
            qulonglong getAllSpeedUp();
            qulonglong getAllSpeedDown();
            //
            bool StartConnection(CONFIGROOT root);
            void StopConnection();
            bool KernelStarted = false;
            //
            static bool ValidateConfig(const QString &path);
            static bool ValidateKernel(const QString &vCorePath, const QString &vAssetsPath, QString *message);

        signals:
            void onProcessErrored();
            void onProcessOutputReadyRead(QString);

        public slots:
            void onAPIDataReady(QString tag, qulonglong totalUp, qulonglong totalDown);

        private:
            APIWorkder *apiWorker;
            QProcess *vProcess;
            bool apiEnabled;
            QMap<QString, qulonglong> transferDataUp;
            QMap<QString, qulonglong> transferDataDown;
            QMap<QString, qulonglong> transferSpeedUp;
            QMap<QString, qulonglong> transferSpeedDown;
    };
}

using namespace Qv2ray::core::kernel;
