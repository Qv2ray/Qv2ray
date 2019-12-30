#include "QvCoreConfigOperations.hpp"

namespace Qv2ray
{
    namespace ConfigOperations
    {
        CONFIGROOT _ReadConnection(const QString &connection)
        {
            QString jsonString = StringFromFile(new QFile(connection));
            auto conf = CONFIGROOT(JsonFromString(jsonString));

            if (conf.count() == 0) {
                LOG(MODULE_CONFIG, "WARN: Possible file corruption, failed to load file: " + connection.toStdString() + " --> File might be empty.")
            }

            return conf;
        }
        QMap<QString, CONFIGROOT> GetRegularConnections(QStringList connectionNames)
        {
            QMap<QString, CONFIGROOT> list;

            for (auto conn : connectionNames) {
                list.insert(conn, _ReadConnection(QV2RAY_CONFIG_DIR + conn + QV2RAY_CONFIG_FILE_EXTENSION));
            }

            return list;
        }

        QMap<QString, CONFIGROOT> GetSubscriptionConnection(QString subscription)
        {
            auto _files = GetFileList(QV2RAY_SUBSCRIPTION_DIR + subscription);
            QMap<QString, CONFIGROOT> _config;

            for (auto _file : _files) {
                // check if is proper connection file.
                if (_file.endsWith(QV2RAY_CONFIG_FILE_EXTENSION)) {
                    auto confName = _file;
                    // Remove the extension
                    confName.chop(sizeof(QV2RAY_CONFIG_FILE_EXTENSION) - 1);
                    _config[confName] = _ReadConnection(QV2RAY_SUBSCRIPTION_DIR + subscription + "/" + _file);
                } else {
                    LOG(MODULE_SUBSCRIPTION, "Found a file in subscription folder but without proper suffix: " + _file.toStdString())
                }
            }

            if (_config.isEmpty()) {
                LOG(MODULE_SUBSCRIPTION, "WARN: Maybe loading an empty subscrption: " + subscription.toStdString())
            }

            return _config;
        }

        QMap<QString, QMap<QString, CONFIGROOT>> GetSubscriptionConnections(QStringList subscriptions)
        {
            //   SUB-NAME    CONN-NAME  CONN-ROOT
            QMap<QString, QMap<QString, CONFIGROOT>> list;

            for (auto singleSub : subscriptions) {
                LOG(MODULE_SUBSCRIPTION, "Processing subscription: " + singleSub.toStdString())
                list[singleSub] = GetSubscriptionConnection(singleSub);
            }

            return list;
        }
        bool CheckIsComplexConfig(CONFIGROOT root)
        {
            bool cRouting = root.contains("routing");
            bool cRule = cRouting && root["routing"].toObject().contains("rules");
            bool cRules = cRule && root["routing"].toObject()["rules"].toArray().count() > 0;
            return cRules;
        }

        int FindIndexByTag(INOUTLIST list, const QString &tag)
        {
            for (int i = 0; i < list.count(); i++) {
                auto value = list[i].toObject();

                if (value.contains("tag") && value["tag"].toString() == tag)
                    return i;
            }

            return -1;
        }
    }
}
