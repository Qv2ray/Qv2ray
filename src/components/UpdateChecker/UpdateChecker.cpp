#include "UpdateChecker.hpp"

#include "Qv2rayApplication.hpp"
#include "Qv2rayBase/Common/HTTPRequestHelper.hpp"
#include "Qv2rayBase/Qv2rayBaseLibrary.hpp"
#include "semver.hpp"

#include <QJsonDocument>

const inline QStringList UpdateChannelLink{
    /*{ 0, "https://api.github.com/repos/moodyhunter/QvPersonal/releases/latest" },*/
    "https://api.github.com/repos/moodyhunter/QvPersonal/releases?per_page=1",
};

using namespace Qv2rayBase;

namespace Qv2ray::components::UpdateChecker
{
    void VersionUpdate(const Qv2rayPlugin::Utils::INetworkRequestHelper::GetResult &result)
    {
        const auto &[err, errString, data] = result;

        QJsonObject root;
        {
            const auto doc = QJsonDocument::fromJson(data);
            root = doc.isArray() ? doc.array().first().toObject() : doc.object();
            if (root.isEmpty())
                return;
        }

        const auto newVersionStr = root["tag_name"].toString("v").mid(1);
        const auto currentVersionStr = QString(QV2RAY_VERSION_STRING);
        const auto ignoredVersionStr = GlobalConfig->updateConfig->IgnoredVersion->isEmpty() ? "0.0.0" : *GlobalConfig->updateConfig->IgnoredVersion;
        //
        bool hasUpdate = false;
        try
        {
            const auto newVersion = semver::version::from_string(newVersionStr.toStdString());
            const auto currentVersion = semver::version::from_string(currentVersionStr.toStdString());
            const auto ignoredVersion = semver::version::from_string(ignoredVersionStr.toStdString());

            qInfo() << "Received update info:";
            qInfo() << " --> Latest: " << newVersionStr;
            qInfo() << " --> Current: " << currentVersionStr;
            qInfo() << " --> Ignored: " << ignoredVersionStr;
            // If the version is newer than us.
            // And new version is newer than the ignored version.
            hasUpdate = (newVersion > currentVersion && newVersion > ignoredVersion);
        }
        catch (std::exception e)
        {
            qInfo() << "Some strange exception occured, cannot check update." << e.what();
        }
        if (hasUpdate)
        {
            const auto name = root["name"].toString();
            if (name.contains("NO_RELEASE"))
            {
                qInfo() << "Found the recent release title with NO_RELEASE tag. Ignoring";
                return;
            }
            const auto link = root["html_url"].toString("");
            const auto versionMessage = QString("A new version of Qv2ray has been found:" NEWLINE "v%1" NEWLINE NEWLINE "%2" NEWLINE "------------" NEWLINE "%3")
                                            .arg(newVersionStr)
                                            .arg(name)
                                            .arg(root["body"].toString());

            const auto result = QvBaselib->Ask(QObject::tr("Qv2ray Update"), versionMessage, { MessageOpt::Yes, MessageOpt::No, MessageOpt::Ignore });
            if (result == MessageOpt::Yes)
            {
                QvBaselib->OpenURL(link);
            }
            else if (result == MessageOpt::Ignore)
            {
                GlobalConfig->updateConfig->IgnoredVersion = newVersionStr;
            }
        }
        else
        {
            qInfo() << "No suitable updates found on channel" << GlobalConfig->updateConfig->UpdateChannel;
        }
    }

    void CheckUpdate()
    {
#ifndef QV2RAY_NO_AUTOUPDATE
        const auto updateChannel = GlobalConfig->updateConfig->UpdateChannel;
        qInfo() << "Start checking update for channel ID: " << updateChannel;
        Qv2rayBase::Utils::NetworkRequestHelper::StaticAsyncGet(UpdateChannelLink[updateChannel], QvApp, &VersionUpdate);
#endif
    }
} // namespace Qv2ray::components::UpdateChecker
