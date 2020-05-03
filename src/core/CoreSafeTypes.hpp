#pragma once

#include "base/models/QvConfigIdentifier.hpp"

#include <QHash>
#include <QHashFunctions>
#include <QString>

namespace Qv2ray::core
{

    inline const static auto NullConnectionId = ConnectionId("null");
    inline const static auto NullGroupId = GroupId("null");

    template<typename IDType>
    QList<IDType> StringsToIdList(const QList<QString> &strings)
    {
        QList<IDType> list;
        for (const auto &str : strings) list << IDType(str);
        return list;
    }

    template<typename IDType>
    QList<QString> IdListToStrings(const QList<IDType> &ids)
    {
        QList<QString> list;
        for (const auto &id : ids) list << id.toString();
        return list;
    }
} // namespace Qv2ray::core

using namespace Qv2ray::core;
