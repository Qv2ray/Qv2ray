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
