#pragma once
#include "base/Qv2rayBase.hpp"
namespace Qv2ray::core::connection::connectionIO
{
    CONFIGROOT ConvertConfigFromFile(const QString &sourceFilePath, bool importComplex);
    QMultiHash<QString, CONFIGROOT> GetConnectionConfigFromSubscription(const QUrl &subscriptionUrl, const QString &groupName);
} // namespace Qv2ray::core::connection::connectionIO

using namespace Qv2ray::core::connection;
using namespace Qv2ray::core::connection::connectionIO;
