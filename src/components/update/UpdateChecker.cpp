#include "UpdateChecker.hpp"

#include "3rdparty/libsemver/version.hpp"
#include "base/Qv2rayBase.hpp"
#include "core/settings/SettingsBackend.hpp"
#include "utils/HTTPRequestHelper.hpp"
#include "utils/QvHelpers.hpp"

const inline QMap<int, QString> UpdateChannelLink //
    {
        { 0, "https://api.github.com/repos/Qv2ray/Qv2ray/releases/latest" },    //
        { 1, "https://api.github.com/repos/Qv2ray/Qv2ray/releases?per_page=1" } //
    };
#define QV_MODULE_NAME "Update"

namespace Qv2ray::components
{
    QvUpdateChecker::QvUpdateChecker(QObject *parent) : QObject(parent)
    {
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
        LOG("Start checking update for channel ID: " + QSTRN(updateChannel));
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
            LOG(QString("Received update info:") + NEWLINE +     //
                " --> Latest: " + newVersionStr + NEWLINE +      //
                " --> Current: " + currentVersionStr + NEWLINE + //
                " --> Ignored: " + ignoredVersionStr);
            // If the version is newer than us.
            // And new version is newer than the ignored version.
            hasUpdate = (newVersion > currentVersion && newVersion > ignoredVersion);
        }
        catch (...)
        {
            LOG("Some strange exception occured, cannot check update.");
        }
        if (hasUpdate)
        {
            const auto name = root["name"].toString("");
            if (name.contains("NO_RELEASE"))
            {
                LOG("Found the recent release title with NO_RELEASE tag. Ignoring");
                return;
            }
            const auto link = root["html_url"].toString("");
            const auto versionMessage =
                QString("A new version of Qv2ray has been found:" NEWLINE "v%1" NEWLINE NEWLINE "%2" NEWLINE "------------" NEWLINE "%3")
                    .arg(newVersionStr)
                    .arg(name)
                    .arg(root["body"].toString());

            const auto result = QvMessageBoxAsk(nullptr, tr("Qv2ray Update"), versionMessage, { Yes, No, Ignore });
            if (result == Yes)
            {
                QvCoreApplication->OpenURL(link);
            }
            else if (result == Ignore)
            {
                // Set and save ingored version.
                GlobalConfig.updateConfig.ignoredVersion = newVersionStr;
                SaveGlobalSettings();
            }
        }
        else
        {
            LOG("No suitable updates found on channel " + QSTRN(GlobalConfig.updateConfig.updateChannel));
        }
    }
} // namespace Qv2ray::components
