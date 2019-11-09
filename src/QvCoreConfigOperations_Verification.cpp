#include "QvCoreConfigOperations.hpp"
#include "QvUtils.hpp"

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
                auto vmessConf = JsonFromString(vmessString);
                // C is a quick hack...
#define C(k) vmessConf.contains(k)
                bool flag = true;
                flag = flag && C("id");
                flag = flag && C("aid");
                flag = flag && C("port");
                flag = flag && C("add");
                // Stream Settings
                auto net = C("net") ? vmessConf["net"].toString() : "tcp";

                if (net == "http" || net == "ws")
                    flag = flag && C("host") && C("path");
                else if (net == "domainsocket")
                    flag = flag && C("path");
                else if (net == "quic")
                    flag = flag && C("host") && C("type") && C("path");

#undef C
                return flag ? 0 : 1;
            } catch (exception *e) {
                LOG(MODULE_CONNECTION_VMESS, "Failed to decode vmess string: " << e->what())
                return -2;
            }
        }

        QString GetVmessFromBase64OrPlain(QByteArray arr)
        {
            // Fine this is just a hack due to some EXTREMELY strange proxy providers.
            auto result = QString::fromUtf8(arr).trimmed();
            return result.startsWith("vmess://") ? result : Base64Decode(result);
        }
    }
}
