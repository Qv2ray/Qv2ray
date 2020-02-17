#include "ConnectionIO.hpp"
#include "common/QvHelpers.hpp"

namespace Qv2ray::core::connection
{
    namespace ConnectionIO
    {
        CONFIGROOT ReadConnectionInternal(const QString &connection)
        {
            QString jsonString = StringFromFile(connection);
            auto conf = CONFIGROOT(JsonFromString(jsonString));

            if (conf.count() == 0) {
                LOG(MODULE_SETTINGS, "WARN: Possible file corruption, failed to load file: " + connection + " --> File might be empty.")
            }

            return conf;
        }

        CONFIGROOT GetConnectionRoot(const GroupId &group, const ConnectionId &id)
        {
            return ReadConnectionInternal(QV2RAY_CONNECTIONS_DIR + group.toString() + "/" + id.toString() + QV2RAY_CONFIG_FILE_EXTENSION);
        }

        CONFIGROOT GetConnectionRoot(const SubscriptionId &subscription, const ConnectionId &id)
        {
            return ReadConnectionInternal(QV2RAY_SUBSCRIPTION_DIR + subscription.toString() + "/" + id.toString() + QV2RAY_CONFIG_FILE_EXTENSION);
        }

        ////
        //// Save Connection to a place, with checking if there's existing file.
        //// If so, append "_N" to the name.
        //bool SaveConnectionConfig(CONFIGROOT obj, QString *alias, bool canOverrideExisting)
        //{
        //    auto str = JsonToString(obj);
        //    auto fullPath = QV2RAY_CONFIG_DIR + *alias + QV2RAY_CONFIG_FILE_EXTENSION;
        //
        //    // If there's already a file AND we CANNOT override existing file.
        //    if (QFile::exists(fullPath) && !canOverrideExisting) {
        //        // Alias is a pointer to a QString.
        //        DeducePossibleFileName(QV2RAY_CONFIG_DIR, alias, QV2RAY_CONFIG_FILE_EXTENSION);
        //        fullPath = QV2RAY_CONFIG_DIR + *alias + QV2RAY_CONFIG_FILE_EXTENSION;
        //    }
        //
        //    LOG(MODULE_SETTINGS, "Saving a config named: " + *alias)
        //    QFile config(fullPath);
        //    return StringToFile(&str, &config);
        //}
        //
        //bool SaveSubscriptionConfig(CONFIGROOT obj, const QString &subscription, QString *name)
        //{
        //    auto str = JsonToString(obj);
        //    auto fName = *name;
        //
        //    if (!IsValidFileName(fName)) {
        //        fName = RemoveInvalidFileName(fName);
        //    }
        //
        //    QFile config(QV2RAY_SUBSCRIPTION_DIR + subscription + "/" + fName + QV2RAY_CONFIG_FILE_EXTENSION);
        //
        //    // If there's already a file. THIS IS EXTREMELY RARE
        //    if (config.exists()) {
        //        LOG(MODULE_FILEIO, "Trying to overrwrite an existing subscription config file. THIS IS RARE")
        //    }
        //
        //    LOG(MODULE_SETTINGS, "Saving a subscription named: " + fName)
        //    bool result = StringToFile(&str, &config);
        //
        //    if (!result) {
        //        LOG(MODULE_FILEIO, "Failed to save a connection config from subscription: " + subscription + ", name: " + fName)
        //    }
        //
        //    *name = fName;
        //    return result;
        //}
        //
        //bool RemoveConnection(const QString &alias)
        //{
        //    QFile config(QV2RAY_CONFIG_DIR + alias + QV2RAY_CONFIG_FILE_EXTENSION);
        //
        //    if (!config.exists()) {
        //        LOG(MODULE_FILEIO, "Trying to remove a non-existing file?")
        //        return false;
        //    } else {
        //        return config.remove();
        //    }
        //}
        //
        //bool RemoveSubscriptionConnection(const QString &subsName, const QString &name)
        //{
        //    QFile config(QV2RAY_SUBSCRIPTION_DIR + subsName + "/" + name + QV2RAY_CONFIG_FILE_EXTENSION);
        //
        //    if (!config.exists()) {
        //        LOG(MODULE_FILEIO, "Trying to remove a non-existing file?")
        //        return false;
        //    } else {
        //        return config.remove();
        //    }
        //}
        //
        //bool RenameConnection(const QString &originalName, const QString &newName)
        //{
        //    LOG(MODULE_CONNECTION, "[RENAME] --> ORIGINAL: " + originalName + ", NEW: " + newName)
        //    return QFile::rename(QV2RAY_CONFIG_DIR + originalName + QV2RAY_CONFIG_FILE_EXTENSION, QV2RAY_CONFIG_DIR + newName + QV2RAY_CONFIG_FILE_EXTENSION);
        //}
        //
        //bool RenameSubscription(const QString &originalName, const QString &newName)
        //{
        //    LOG(MODULE_SUBSCRIPTION, "[RENAME] --> ORIGINAL: " + originalName + ", NEW: " + newName)
        //    return QDir().rename(QV2RAY_SUBSCRIPTION_DIR + originalName, QV2RAY_SUBSCRIPTION_DIR + newName);
        //}
        //
        //CONFIGROOT ConvertConfigFromFile(QString sourceFilePath, bool importComplex)
        //{
        //    QFile source(sourceFilePath);
        //
        //    if (!source.exists()) {
        //        LOG(MODULE_FILEIO, "Trying to import from an non-existing file.")
        //        return CONFIGROOT();
        //    }
        //
        //    auto root = CONFIGROOT(JsonFromString(StringFromFile(&source)));
        //
        //    if (!importComplex) {
        //        JSON_ROOT_TRY_REMOVE("inbounds")
        //        JSON_ROOT_TRY_REMOVE("routing")
        //    }
        //
        //    JSON_ROOT_TRY_REMOVE("log")
        //    JSON_ROOT_TRY_REMOVE("api")
        //    JSON_ROOT_TRY_REMOVE("stats")
        //    JSON_ROOT_TRY_REMOVE("dns")
        //    return root;
        //}
    }
}
