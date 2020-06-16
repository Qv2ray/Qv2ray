#pragma once
#include "common/HTTPRequestHelper.hpp"

#include <QObject>
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
        NetworkRequestHelper requestHelper;
        void static VersionUpdate(const QByteArray &data);
    };
} // namespace Qv2ray::components
using namespace Qv2ray::components;
