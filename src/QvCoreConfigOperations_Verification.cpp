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
                auto vmessConf = JSONFromString(vmessString);
                // C is a quick hack...
#define C(k) vmessConf.contains(k)
                //string       v,        ps,        add,        port,        id,        aid,        net,        type,        host,        path,        tls;
                bool flag = C("v") && C("ps") && C("add") && C("port") && C("id") && C("aid") && C("net") && C("type") && C("host") && C("path") && C("tls");
#undef C
                return flag ? 0 : 1;
            } catch (exception *e) {
                LOG(MODULE_CONNECTION, "Failed to decode vmess string: " << e->what())
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
