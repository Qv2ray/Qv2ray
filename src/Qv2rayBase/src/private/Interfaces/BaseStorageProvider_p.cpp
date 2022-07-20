//  Qv2rayBase, the modular feature-rich infrastructure library for Qv2ray.
//  Copyright (C) 2021 Moody and relavent Qv2ray contributors.
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "Qv2rayBase/private/Interfaces/BaseStorageProvider_p.hpp"

#include "Qv2rayBase/Common/Utils.hpp"
#include "Qv2rayBase/Qv2rayBaseLibrary.hpp"

#include <QCoreApplication>
#include <QStandardPaths>

const auto QV2RAY_CONFIG_PATH_ENV_NAME = "QV2RAY_CONFIG_PATH";
const auto QV2RAY_CONFIG_FILE_NAME = "Qv2ray.conf";
const auto CONNECTIONS = "connections";
const auto GROUPS = "groups";
const auto ROUTINGS = "routings";
const auto PLUGINS = "plugins";
const auto PLUGIN_FILES = "plugin_files";
const auto PLUGIN_SETTINGS = "plugin_settings";
const auto EXTRA_SETTINGS = "extra_settings";

#define DEBUG_SUFFIX (RuntimeContext.contains(StorageContextFlags::STORAGE_CTX_IS_DEBUG) ? u"_debug/"_qs : u"/"_qs)

#define ConnectionsJson ConfigDirPath + CONNECTIONS + ".json"
#define GroupsJson ConfigDirPath + GROUPS + ".json"
#define RoutingsJson ConfigDirPath + ROUTINGS + ".json"

#define ConnectionJson(id) ConfigDirPath + CONNECTIONS + "/" + id + ".json"
#define GroupJson(id) ConfigDirPath + GROUPS + "/" + id + ".json"
#define RoutingJson(id) ConfigDirPath + ROUTINGS + "/" + id + ".json"
#define PluginSettingsJson(id) ConfigDirPath + PLUGIN_SETTINGS + "/" + id + ".json"

bool CheckPathAvailability(const QString &_dirPath, bool checkExistingConfig)
{
    auto path = _dirPath;
    if (!path.endsWith(u"/"))
        path.append("/");

    // Does not exist.
    if (!QDir(path).exists())
        return false;

    {
        // A temp file used to test file permissions in that folder.
        QFile testFile(path + ".qv2ray_test_file" + QString::number(QTime::currentTime().msecsSinceStartOfDay()));
        if (!testFile.open(QFile::OpenModeFlag::ReadWrite))
        {
            qInfo() << "Directory at:" << path << "cannot be used as a valid config file path.";
            qInfo() << "---> Cannot create a new file or open a file for writing.";
            return false;
        }
        testFile.write("Qv2ray test file, feel free to remove.");
        testFile.flush();
        testFile.close();
        if (!testFile.remove())
        {
            // This is rare, as we can create a file but failed to remove it.
            qInfo() << "Directory at:" << path << "cannot be used as a valid config file path.";
            qInfo() << "---> Cannot remove a file.";
            return false;
        }
    }

    if (!checkExistingConfig)
    {
        // Just pass the test
        return true;
    }

    QFile configFile(path + QV2RAY_CONFIG_FILE_NAME);

    // No such config file.
    if (!configFile.exists())
        return false;

    if (!configFile.open(QIODevice::ReadWrite))
    {
        qInfo() << "File:" << configFile.fileName() << " cannot be opened!";
        return false;
    }

    const auto err = VerifyJsonString(configFile.readAll());
    if (err)
    {
        qInfo() << "Json parse returns:" << *err;
        return false;
    }

    return true;
}

namespace Qv2rayBase::Interfaces
{
    Qv2rayBasePrivateStorageProvider::Qv2rayBasePrivateStorageProvider(){};

    QString Qv2rayBasePrivateStorageProvider::StorageLocation() const
    {
        return ConfigDirPath;
    }

    bool Qv2rayBasePrivateStorageProvider::LookupConfigurations(const StorageContext &runtimeContext)
    {
        QStringList configSearchPaths;

        {
            // Application directory
            if (runtimeContext.contains(STORAGE_CTX_HAS_ASIDE_CONFIGURATION))
                configSearchPaths << qApp->applicationDirPath() + "/config" + DEBUG_SUFFIX;

            // Standard platform-independent configuration location
            configSearchPaths << QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) + "/" + QCoreApplication::applicationName().toLower() + "/";
        }

        // Custom configuration path
        if (qEnvironmentVariableIsSet(QV2RAY_CONFIG_PATH_ENV_NAME))
        {
            const auto manualConfigPath = qEnvironmentVariable(QV2RAY_CONFIG_PATH_ENV_NAME);
            qInfo() << "Using config path from env:" << manualConfigPath;
            configSearchPaths.clear();
            configSearchPaths << manualConfigPath;
        }

        QString selectedConfigurationFile;

        for (const auto &dirPath : configSearchPaths)
        {
            // Verify the config path, check if the config file exists and in the correct JSON format.
            // True means we check for config existence as well. --|HERE|
            bool isValidConfigPath = CheckPathAvailability(dirPath, true);

            if (isValidConfigPath)
            {
                qDebug() << "Path:" << dirPath << "is valid.";
                selectedConfigurationFile = dirPath + QV2RAY_CONFIG_FILE_NAME;
                break;
            }
            qInfo() << "Path:" << dirPath << "does not contain a valid config file.";
        }

        if (selectedConfigurationFile.isEmpty())
        {
            // If there's no existing config, use the first one in the search list.
            selectedConfigurationFile = configSearchPaths.first() + QV2RAY_CONFIG_FILE_NAME;

            // Check if the dirs are writeable
            const auto dir = QFileInfo(selectedConfigurationFile).path();
            const auto hasPossibleNewLocation = QDir(dir).mkpath(dir) && CheckPathAvailability(dir, false);
            if (!hasPossibleNewLocation)
            {
                // None of the path above can be used as a dir for storing config.
                // Even the last folder failed to pass the check.
                qInfo() << "FATAL:";
                qInfo() << "Cannot load configuration file Qv2ray";
                qInfo() << "Cannot find a place to store config files." << NEWLINE << "Qv2ray has searched these paths below:";
                qInfo() << "";
                qInfo() << configSearchPaths;
                qInfo() << "It usually means you don't have the write permission to all of those locations.";
                return false;
            }

            // Found a valid config dir, with write permission, but assume no config is located in it.
            if (QFile::exists(selectedConfigurationFile))
            {
                // As we already tried to load config from every possible dir.
                //
                // This condition branch (!hasExistingConfig check) holds the fact that current config dir,
                // should NOT contain any valid file (at least in the same name)
                //
                // It usually means that QV2RAY_CONFIG_FILE here has a corrupted JSON format.
                //
                // Otherwise Qv2ray would have loaded this config already instead of notifying to create a new config in this folder.
                //
                qInfo() << "This should not occur: Qv2ray config exists but cannot be load.";
                qInfo() << "Failed to initialise Qv2rayBase";
                qInfo() << "Qv2ray has found a config file, but it failed to be loaded due to some errors.";
                qInfo() << "A workaround is to remove the this file and restart Qv2ray:";
                qInfo() << selectedConfigurationFile;
                return false;
            }

            // Now make the file exist.
            WriteFile("{}", selectedConfigurationFile);
        }

        // At this step, the "selectedConfigurationFile" is ensured to be OK for storing configuration.
        // Use the config path found by the checks above
        RuntimeContext = runtimeContext;
        ExecutableDirPath = qApp->applicationDirPath();
        ConfigFilePath = selectedConfigurationFile;
        ConfigDirPath = QFileInfo(ConfigFilePath).path() + "/";
        qInfo() << "Using" << selectedConfigurationFile << "as the config path.";
        return true;
    }

    void Qv2rayBasePrivateStorageProvider::EnsureSaved()
    {
    }

    QJsonObject Qv2rayBasePrivateStorageProvider::GetBaseConfiguration()
    {
        return JsonFromString(ReadFile(ConfigFilePath));
    }

    bool Qv2rayBasePrivateStorageProvider::StoreBaseConfiguration(const QJsonObject &json)
    {
        return WriteFile(JsonToString(json).toUtf8(), ConfigFilePath);
    }

    QHash<ConnectionId, ConnectionObject> Qv2rayBasePrivateStorageProvider::GetConnections()
    {
        const auto connectionJson = JsonFromString(ReadFile(ConnectionsJson));

        QHash<ConnectionId, ConnectionObject> result;
        for (auto it = connectionJson.constBegin(); it != connectionJson.constEnd(); it++)
        {
            ConnectionObject o;
            o.loadJson(it.value());
            result.insert(ConnectionId{ it.key() }, o);
        }
        return result;
    }

    QHash<GroupId, GroupObject> Qv2rayBasePrivateStorageProvider::GetGroups()
    {
        const auto groupsJson = JsonFromString(ReadFile(GroupsJson));

        QHash<GroupId, GroupObject> result;
        for (auto it = groupsJson.constBegin(); it != groupsJson.constEnd(); it++)
        {
            GroupObject o;
            o.loadJson(it.value());
            result.insert(GroupId{ it.key() }, o);
        }
        return result;
    }

    QHash<RoutingId, RoutingObject> Qv2rayBasePrivateStorageProvider::GetRoutings()
    {
        const auto routingsJson = JsonFromString(ReadFile(RoutingsJson));

        QHash<RoutingId, RoutingObject> result;
        for (auto it = routingsJson.constBegin(); it != routingsJson.constEnd(); it++)
        {
            RoutingObject o;
            o.loadJson(it.value());
            result.insert(RoutingId{ it.key() }, o);
        }
        return result;
    }

    void Qv2rayBasePrivateStorageProvider::StoreConnections(const QHash<ConnectionId, ConnectionObject> &conns)
    {
        QJsonObject obj;
        for (auto it = conns.constKeyValueBegin(); it != conns.constKeyValueEnd(); it++)
            obj[it->first.toString()] = it->second.toJson();
        WriteFile(JsonToString(obj).toUtf8(), ConnectionsJson);
    }

    void Qv2rayBasePrivateStorageProvider::StoreGroups(const QHash<GroupId, GroupObject> &groups)
    {
        QJsonObject obj;
        for (auto it = groups.constKeyValueBegin(); it != groups.constKeyValueEnd(); it++)
            obj[it->first.toString()] = it->second.toJson();
        WriteFile(JsonToString(obj).toUtf8(), GroupsJson);
    }

    void Qv2rayBasePrivateStorageProvider::StoreRoutings(const QHash<RoutingId, RoutingObject> &routings)
    {
        QJsonObject obj;
        for (auto it = routings.constKeyValueBegin(); it != routings.constKeyValueEnd(); it++)
            obj[it->first.toString()] = it->second.toJson();
        WriteFile(JsonToString(obj).toUtf8(), RoutingsJson);
    }

    ProfileContent Qv2rayBasePrivateStorageProvider::GetConnectionContent(const ConnectionId &id)
    {
        return ProfileContent::fromJson(JsonFromString(ReadFile(ConnectionJson(id.toString()))));
    }

    bool Qv2rayBasePrivateStorageProvider::StoreConnection(const ConnectionId &id, const ProfileContent &profile)
    {
        return WriteFile(JsonToString(profile.toJson()).toUtf8(), ConnectionJson(id.toString()));
    }

    bool Qv2rayBasePrivateStorageProvider::DeleteConnection(const ConnectionId &id)
    {
        return QFile::remove(ConnectionJson(id.toString()));
    }

    QDir Qv2rayBasePrivateStorageProvider::GetUserPluginDirectory()
    {
        QDir d(ConfigDirPath + PLUGINS + "/");
        if (!d.exists())
            d.mkpath(d.absolutePath());
        return d;
    }

    QDir Qv2rayBasePrivateStorageProvider::GetPluginWorkingDirectory(const PluginId &pid)
    {
        QDir d(ConfigDirPath + PLUGIN_FILES + "/" + pid.toString() + "/");
        if (!d.exists())
            d.mkpath(d.absolutePath());
        return d;
    }

    QJsonObject Qv2rayBasePrivateStorageProvider::GetPluginSettings(const PluginId &pid)
    {
        return JsonFromString(ReadFile(PluginSettingsJson(pid.toString())));
    }

    void Qv2rayBasePrivateStorageProvider::SetPluginSettings(const PluginId &pid, const QJsonObject &obj)
    {
        WriteFile(JsonToString(obj).toUtf8(), PluginSettingsJson(pid.toString()));
    }

    QJsonObject Qv2rayBasePrivateStorageProvider::GetExtraSettings(const QString &key)
    {
        return JsonFromString(ReadFile(ConfigDirPath + EXTRA_SETTINGS + "/" + key + ".json"));
    }

    bool Qv2rayBasePrivateStorageProvider::StoreExtraSettings(const QString &key, const QJsonObject &j)
    {
        return WriteFile(JsonToString(j).toUtf8(), ConfigDirPath + EXTRA_SETTINGS + "/" + key + ".json");
    }

    QStringList Qv2rayBasePrivateStorageProvider::GetAssetsPath(const QString &dirName)
    {
        static const auto makeAbs = [](const QDir &p) { return p.absolutePath(); };

        QStringList list;
        // Default behavior on Windows
        list << makeAbs(QCoreApplication::applicationDirPath() + "/" + dirName);

        list << ":/" + dirName;

        list << QStandardPaths::locateAll(QStandardPaths::AppDataLocation, dirName, QStandardPaths::LocateDirectory);
        list << QStandardPaths::locateAll(QStandardPaths::AppConfigLocation, dirName, QStandardPaths::LocateDirectory);

        if (qEnvironmentVariableIsSet("XDG_DATA_DIRS"))
            list << makeAbs(qEnvironmentVariable("XDG_DATA_DIRS") + "/" + dirName);

#ifdef Q_OS_UNIX
        if (qEnvironmentVariableIsSet("APPIMAGE"))
            list << makeAbs(QCoreApplication::applicationDirPath() + "/../share/Qv2rayBase" + DEBUG_SUFFIX + dirName);

        if (qEnvironmentVariableIsSet("SNAP"))
            list << makeAbs(qEnvironmentVariable("SNAP") + "/usr/share/Qv2rayBase" + DEBUG_SUFFIX + dirName);

        list << makeAbs("/usr/local/share/Qv2rayBase" + DEBUG_SUFFIX + dirName);
        list << makeAbs("/usr/local/lib/Qv2rayBase" + DEBUG_SUFFIX + dirName);
        list << makeAbs("/usr/local/lib64/Qv2rayBase" + DEBUG_SUFFIX + dirName);
        list << makeAbs("/usr/share/Qv2rayBase" + DEBUG_SUFFIX + dirName);
        list << makeAbs("/usr/lib/Qv2rayBase" + DEBUG_SUFFIX + dirName);
        list << makeAbs("/usr/lib64/Qv2rayBase" + DEBUG_SUFFIX + dirName);
        list << makeAbs("/lib/Qv2rayBase" + DEBUG_SUFFIX + dirName);
        list << makeAbs("/lib64/Qv2rayBase" + DEBUG_SUFFIX + dirName);
#endif
        list << ConfigDirPath + dirName;

#ifdef Q_OS_MAC
        // macOS platform directories.
        list << QDir(QCoreApplication::applicationDirPath() + "/../Resources/" + dirName).absolutePath();
#endif
        return list;
    }
} // namespace Qv2rayBase::Interfaces
