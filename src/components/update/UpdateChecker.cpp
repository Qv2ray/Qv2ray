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
        requestHelper = new NetworkRequestHelper(this);
    }

    QvUpdateChecker::~QvUpdateChecker()
    {
    }

    void QvUpdateChecker::CheckUpdate()
    {
#ifndef DISABLE_AUTO_UPDATE
        if (QFile(QV2RAY_CONFIG_DIR + "QV2RAY_FEATURE_DISABLE_AUTO_UPDATE").exists())
            return;
        const auto &updateChannel = GlobalConfig.updateConfig.updateChannel;
        LOG(MODULE_NETWORK, "Start checking update for channel ID: " + QSTRN(updateChannel))
        requestHelper->AsyncHttpGet(UpdateChannelLink[updateChannel], &QvUpdateChecker::VersionUpdate);
#endif
    }

    void QvUpdateChecker::VersionUpdate(const QByteArray &data)
    {
        // Version update handler.
        const auto doc = QJsonDocument::fromJson(data);
        const auto root = doc.isArray() ? doc.array().first().toObject() : doc.object();
        if (root.isEmpty())
            return;

        const auto newVersionStr = root["tag_name"].toString("v").mid(1);
        const auto currentVersionStr = QString(QV2RAY_VERSION_STRING);
        const auto ignoredVersionStr = GlobalConfig.updateConfig.ignoredVersion.isEmpty() ? "0.0.0" : GlobalConfig.updateConfig.ignoredVersion;
        //
        bool hasUpdate = false;
        try
        {
            const auto newVersion = semver::version::from_string(newVersionStr.toStdString());
            const auto currentVersion = semver::version::from_string(currentVersionStr.toStdString());
            const auto ignoredVersion = semver::version::from_string(ignoredVersionStr.toStdString());
            //
            LOG(MODULE_UPDATE, QString("Received update info:") + NEWLINE +         //
                                   " --> Latest: " + newVersionStr + NEWLINE +      //
                                   " --> Current: " + currentVersionStr + NEWLINE + //
                                   " --> Ignored: " + ignoredVersionStr)
            // If the version is newer than us.
            // And new version is newer than the ignored version.
            hasUpdate = (newVersion > currentVersion && newVersion > ignoredVersion);
        }
        catch (...)
        {
            LOG(MODULE_UPDATE, "Some strange exception occured, cannot check update.")
        }
        if (hasUpdate)
        {
            const auto name = root["name"].toString("");
            if (name.contains("NO_RELEASE"))
            {
                LOG(MODULE_UPDATE, "Found the recent release title with NO_RELEASE tag. Ignoring")
                return;
            }
            const auto link = root["html_url"].toString("");
            auto result = QvMessageBoxAsk(nullptr, tr("Qv2ray Update"),
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
                GlobalConfig.updateConfig.ignoredVersion = newVersionStr;
                SaveGlobalSettings();
            }
        }
        else
        {
            LOG(MODULE_UPDATE, "No suitable updates found on channel " + QSTRN(GlobalConfig.updateConfig.updateChannel))
        }
    }
} // namespace Qv2ray::components
