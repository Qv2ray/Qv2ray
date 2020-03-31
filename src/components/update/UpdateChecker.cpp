#include "UpdateChecker.hpp"

#include "3rdparty/libsemver/version.hpp"
#include "base/Qv2rayBase.hpp"
#include "common/HTTPRequestHelper.hpp"
#include "common/QvHelpers.hpp"
#include "core/settings/SettingsBackend.hpp"

#include <QVersionNumber>

const inline QMap<int, QString> UpdateChannelLink //
    {
        { 0, "https://api.github.com/repos/Qv2ray/Qv2ray/releases/latest" },    //
        { 1, "https://api.github.com/repos/Qv2ray/Qv2ray/releases?per_page=1" } //
    };

namespace Qv2ray::components
{
    QvUpdateChecker::QvUpdateChecker(QObject *parent) : QObject(parent)
    {
        requestHelper = new QvHttpRequestHelper(this);
        connect(requestHelper, &QvHttpRequestHelper::httpRequestFinished, this, &QvUpdateChecker::VersionUpdate);
    }
    QvUpdateChecker::~QvUpdateChecker()
    {
    }
    void QvUpdateChecker::CheckUpdate()
    {
#ifndef DISABLE_AUTO_UPDATE
        auto updateChannel = GlobalConfig.updateConfig.updateChannel;
        LOG(MODULE_NETWORK, "Start checking update for channel ID: " + QSTRN(updateChannel))
        requestHelper->get(UpdateChannelLink[updateChannel]);
#endif
    }
    void QvUpdateChecker::VersionUpdate(QByteArray &data)
    {
        // Version update handler.
        auto doc = QJsonDocument::fromJson(data);
        QJsonObject root = doc.isArray() ? doc.array().first().toObject() : doc.object();
        if (root.isEmpty())
            return;
        //
        const auto newVersionTag = root["tag_name"].toString("v").mid(1);
        //
        auto newVersion = semver::version::from_string(newVersionTag.toStdString());
        auto currentVersion = semver::version::from_string(QString(QV2RAY_VERSION_STRING).remove(0, 1).toStdString());
        auto ignoredVersion = semver::version::from_string(GlobalConfig.updateConfig.ignoredVersion.toStdString());
        //
        LOG(MODULE_UPDATE, "Received update info, Latest: " + QString::fromStdString(newVersion.str()) + //
                               " Current: " + QString::fromStdString(currentVersion.str()) +             //
                               " Ignored: " + QString::fromStdString(ignoredVersion.str()))
        //
        const auto name = root["name"].toString("");
        if (name.contains("NO_RELEASE"))
        {
            LOG(MODULE_UPDATE, "Found the recent release title with NO_RELEASE tag. Ignoring")
            return;
        }
        // If the version is newer than us.
        // And new version is newer than the ignored version.
        if (newVersion > currentVersion && newVersion > ignoredVersion)
        {
            const auto link = root["html_url"].toString("");
            auto result = QvMessageBoxAsk(nullptr, //
                                          tr("Qv2ray Update"),
                                          tr("A new version of Qv2ray has been found:") + //
                                              "v" + newVersionTag + NEWLINE +             //
                                              name + NEWLINE "------------" NEWLINE +     //
                                              root["body"].toString(""),
                                          QMessageBox::Ignore);

            if (result == QMessageBox::Yes)
            {
                QDesktopServices::openUrl(QUrl::fromUserInput(link));
            }
            else if (result == QMessageBox::Ignore)
            {
                // Set and save ingored version.
                GlobalConfig.updateConfig.ignoredVersion = QString::fromStdString(newVersion.str());
                SaveGlobalSettings();
            }
        }
    }
} // namespace Qv2ray::components
