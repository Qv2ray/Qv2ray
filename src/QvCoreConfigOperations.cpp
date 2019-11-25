#include "QvCoreConfigOperations.hpp"

namespace Qv2ray
{
    namespace ConfigOperations
    {

        QMap<QString, QJsonObject> GetConnections(list<string> connectionNames)
        {
            QMap<QString, QJsonObject> list;

            for (auto conn : connectionNames) {
                QString jsonString = StringFromFile(new QFile(QV2RAY_CONFIG_DIR + QSTRING(conn) + QV2RAY_CONFIG_FILE_EXTENSION));
                QJsonObject connectionObject = JsonFromString(jsonString);
                list.insert(QString::fromStdString(conn), connectionObject);
            }

            return list;
        }

        bool CheckIsComplexConfig(QJsonObject root)
        {
            bool cRouting = root.contains("routing");
            bool cRule = cRouting && root["routing"].toObject().contains("rules");
            bool cRules = cRule && root["routing"].toObject()["rules"].toArray().count() > 0;
            return cRules;
        }

        int StartPreparation(QJsonObject fullConfig)
        {
            // Writes the final configuration to the disk.
            QString json = JsonToString(fullConfig);
            StringToFile(&json, new QFile(QV2RAY_GENERATED_FILE_PATH));
            return 0;
        }
        int FindIndexByTag(QJsonArray list, QString *tag)
        {
            for (int i = 0; i < list.count(); i++) {
                auto value = list[i].toObject();

                if (value.contains("tag") && value["tag"].toString() == *tag)
                    return i;
            }

            return -1;
        }
    }
}
