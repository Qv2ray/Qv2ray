#include "UpdateChecker.hpp"

#include "3rdparty/libsemver/version.hpp"
#include "base/Qv2rayBase.hpp"
#include "common/HTTPRequestHelper.hpp"
#include "common/QvHelpers.hpp"
#include "core/settings/SettingsBackend.hpp"

#include <QDesktopServices>
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
        connect(requestHelper, &QvHttpRequestHelper::OnRequestFinished, this, &QvUpdateChecker::VersionUpdate);
    }
    QvUpdateChecker::~QvUpdateChecker()
    {
    }
    void QvUpdateChecker::CheckUpdate()
    {
#ifndef DISABLE_AUTO_UPDATE
        auto updateChannel = GlobalConfig.updateConfig.updateChannel;
        LOG(MODULE_NETWORK, "Start checking update for channel ID: " + QSTRN(updateChannel))
        requestHelper->AsyncGet(UpdateChannelLink[updateChannel]);
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
        const auto newVersionStr = root["tag_name"].toString("v").mid(1);
        const auto currentVersionStr = QString(QV2RAY_VERSION_STRING);
        const auto ignoredVersionStr = GlobalConfig.updateConfig.ignoredVersion.isEmpty() ? "0.0.0" : GlobalConfig.updateConfig.ignoredVersion;
        //
        auto newVersion = semver::version::from_string(newVersionStr.toStdString());
        auto currentVersion = semver::version::from_string(currentVersionStr.toStdString());
        auto ignoredVersion = semver::version::from_string(ignoredVersionStr.toStdString());
        //
        LOG(MODULE_UPDATE, "Received update info, Latest: " + newVersionStr + //
                               " Current: " + currentVersionStr +             //
                               " Ignored: " + ignoredVersionStr)
        // If the version is newer than us.
        // And new version is newer than the ignored version.
        if (newVersion > currentVersion && newVersion > ignoredVersion)
        {
            const auto name = root["name"].toString("");
            if (name.contains("NO_RELEASE"))
            {
                LOG(MODULE_UPDATE, "Found the recent release title with NO_RELEASE tag. Ignoring")
                return;
            }
            const auto link = root["html_url"].toString("");
            auto result = QvMessageBoxAsk(nullptr, //
                                          tr("Qv2ray Update"),
                                          tr("A new version of Qv2ray has been found:") + //
                                              "v" + newVersionStr + NEWLINE + NEWLINE +   //
                                              name + NEWLINE "------------" NEWLINE +     //
                                              root["body"].toString(""),
                                          QMessageBox::Ignore);

            if (result == QMessageBox::Yes)
            {
                QDesktopServices::openUrl(link);
            }
            else if (result == QMessageBox::Ignore)
            {
                // Set and save ingored version.
                GlobalConfig.updateConfig.ignoredVersion = QString::fromStdString(newVersion.str());
                SaveGlobalSettings();
            }
        }
        else
        {
            LOG(MODULE_UPDATE, "No suitable updates found on channel " + QSTRN(GlobalConfig.updateConfig.updateChannel))
        }
    }
} // namespace Qv2ray::components
