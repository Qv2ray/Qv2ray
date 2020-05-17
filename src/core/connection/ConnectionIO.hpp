#pragma once
#include "base/Qv2rayBase.hpp"
namespace Qv2ray::core::connection
{
    namespace ConnectionIO
    {
        // File Protocol
        CONFIGROOT ConvertConfigFromFile(const QString &sourceFilePath, bool importComplex);
        QMultiHash<QString, CONFIGROOT> GetConnectionConfigFromSubscription(const QUrl &subscriptionUrl, const QString &groupName);
    } // namespace ConnectionIO
} // namespace Qv2ray::core::connection

using namespace Qv2ray::core::connection;
using namespace Qv2ray::core::connection::ConnectionIO;
