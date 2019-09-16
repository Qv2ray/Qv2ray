#include "QvCoreConfigOperations.h"
#include "QvUtils.h"

namespace Qv2ray
{
    namespace ConfigOperations
    {
        // -------------------------- BEGIN CONFIG VALIDATIONS ----------------------------------------------------------------------------

        int VerifyVMessProtocolString(QString vmess)
        {
            if (!vmess.toLower().startsWith("vmess://")) {
                return -1;
            }

            try {
                QStringRef vmessJsonB64(&vmess, 8, vmess.length() - 8);
                auto vmessString = Base64Decode(vmessJsonB64.toString());
                auto vmessConf = StructFromJSONString<VMessProtocolConfigObject>(vmessString);
                return 0;
            } catch (exception *e) {
                LOG(MODULE_CONNECTION, QObject::tr("#VMessDecodeError").toStdString() << e->what())
                return -2;
            }
        }

        QString GetVmessFromBase64OrPlain(QByteArray arr)
        {
            auto result = QString::fromUtf8(arr).trimmed();
            return result.startsWith("vmess://") ? result : Base64Decode(result);
        }
    }
}
