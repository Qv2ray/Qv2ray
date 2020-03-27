#pragma once
#include <QObject>
namespace Qv2ray::common
{
    class QvHttpRequestHelper;
}
namespace Qv2ray::components
{
    struct QvUpdateInfo
    {
        int channel;
        QString tag;
        QString title;
        QString releaseNotes;
        QString downloadLink;
    };

    class QvUpdateChecker : public QObject
    {
        Q_OBJECT
      public:
        explicit QvUpdateChecker(QObject *parent = nullptr);
        void CheckUpdate();
        ~QvUpdateChecker();
      signals:
        void OnCheckUpdateCompleted(bool hasUpdate, const QvUpdateInfo &updateInfo);

      private:
        void VersionUpdate(QByteArray &data);
        common::QvHttpRequestHelper *requestHelper;
    };
    inline QvUpdateChecker UpdateChecker;
} // namespace Qv2ray::components
using namespace Qv2ray::components;
