#include "QvCoreConfigOperations.h"
namespace Qv2ray
{
    namespace ConfigOperations
    {
        // -------------------------- BEGIN CONFIG CONVERSIONS ----------------------------------------------------------------------------
        int SaveConnectionConfig(QJsonObject obj, const QString *alias)
        {
            QFile config(QV2RAY_CONFIG_PATH + alias + QV2RAY_CONNECTION_FILE_EXTENSION);

            if (config.exists()) {
                return -1;
            }

            QJsonDocument doc;
            doc.setObject(obj);
            SaveStringToFile(doc.toJson(), &config);
            return 0;
        }
    }
}
