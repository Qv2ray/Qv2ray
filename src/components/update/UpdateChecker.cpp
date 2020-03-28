#include "UpdateChecker.hpp"

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
        auto newVersion = QVersionNumber::fromString(newVersionTag);
        auto currentVersion = QVersionNumber::fromString(QString(QV2RAY_VERSION_STRING).remove(0, 1));
        auto ignoredVersion = QVersionNumber::fromString(GlobalConfig.updateConfig.ignoredVersion);
        //
        LOG(MODULE_UPDATE, "Received update info, Latest: " + newVersion.toString() + //
                               " Current: " + currentVersion.toString() +             //
                               " Ignored: " + ignoredVersion.toString())
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
                GlobalConfig.updateConfig.ignoredVersion = newVersion.toString();
                SaveGlobalSettings();
            }
        }
    }
} // namespace Qv2ray::components
