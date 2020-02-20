#include "base/Qv2rayBase.hpp"
#include "core/CoreSafeTypes.hpp"

namespace Qv2ray::core::connection
{
    namespace ConnectionIO
    {
        // File Protocol
        CONFIGROOT ConvertConfigFromFile(QString sourceFilePath, bool importComplex);
    }
}

using namespace Qv2ray::core::connection;
using namespace Qv2ray::core::connection::ConnectionIO;
