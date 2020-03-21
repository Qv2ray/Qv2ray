#pragma once
#include "base/Qv2rayBase.hpp"
#include "core/CoreSafeTypes.hpp"
namespace Qv2ray::core::connection
{
    namespace ConnectionIO
    {
        // File Protocol
        CONFIGROOT ConvertConfigFromFile(const QString &sourceFilePath, bool importComplex);
    } // namespace ConnectionIO
} // namespace Qv2ray::core::connection

using namespace Qv2ray::core::connection;
using namespace Qv2ray::core::connection::ConnectionIO;
