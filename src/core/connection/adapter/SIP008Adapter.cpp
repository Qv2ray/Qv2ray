#include "AbstractAdapter.hpp"
#include "base/Qv2rayBase.hpp"

#include <QJsonDocument>
#include <QJsonObject>

namespace Qv2ray::core::connection::adapter
{
    /**
     * A Naive SIP008 Subscription Adapter for Shadowsocks.
     * @author DuckSoft <realducksoft@gmail.com>
     * @copyright Licensed under GPLv3.
     */
    class SIP008Adapter : public AbstractNodeAdapter<CONFIGROOT>
    {
      public:
        [[nodiscard]] bool decodeNodes(const QString &data, QList<QPair<QString, CONFIGROOT>> &nodeList, QStringList &errorList)
        {
            QJsonParseError err;
            const auto json = QJsonDocument::fromJson(data.toUtf8(), &err);

            // valid json required
            if (err.error != QJsonParseError::NoError)
            {
                errorList << QObject::tr("failed to parse as JSON: %1").arg(err.errorString());
                return false;
            }

            // json requires root to be an object
            if (!json.isObject())
            {
                errorList << QObject::tr("JSON root is not an object");
                return false;
            }

            // casting root into JsonObject
            const auto root = json.object();

            // check the existance of 'server' field.
            const auto serverField = root["server"];
            if (serverField.isUndefined())
            {
                errorList << QObject::tr("JSON object does not contain 'server' field.");
                return false;
            }

            // check the type of 'server' field.
            if (!serverField.isArray())
            {
                errorList << QObject::tr("'server' field is not an array");
                return false;
            }

            // cast 'server' field to QJsonArray.
            const auto serverArray = serverField.toArray();

            // iterate through the servers:
            for (const auto &&serverDoc : serverArray)
            {
                // server entry must be JsonObject
                if (!serverDoc.isObject())
                {
                    errorList << QObject::tr("non-object entry encountered in server list");
                    continue;
                }

                // cast the entry to QJsonObject.
                const auto &serverObj = serverDoc.toObject();

                // fetch information from object
                const auto &serverAddr = serverObj["server"].toString();
                const auto &serverPort = serverObj["server_port"].toInt();
                const auto &password = serverObj["password"].toString();
                const auto &method = serverObj["method"].toString();
                const auto &remarks = serverObj["remarks"].toString();

                // TODO: validate the information and creating entry into nodeList.
            }

            return true;
        }
    };

} // namespace Qv2ray::core::connection::adapter
