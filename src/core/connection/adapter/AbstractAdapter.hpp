#pragma once

#include <QList>

namespace Qv2ray::core::connection::adapter
{
    template<typename T>
    class AbstractNodeAdapter
    {
      public:
        virtual bool decodeNodes(const QString &data, QList<QPair<QString, T>> &nodeList, QStringList &errorList) const = 0;
    };

}; // namespace Qv2ray::core::connection::adapter
