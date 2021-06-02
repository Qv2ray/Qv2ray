#pragma once
#include "QJsonStruct.hpp"

#include <QMap>

namespace Qv2ray::base::safetype
{
    template<typename T1, typename T2 = T1>
    struct QvPair
    {
        T1 value1;
        T2 value2;
        friend bool operator==(const QvPair<T1, T2> &one, const QvPair<T1, T2> &another)
        {
            return another.value1 == one.value1 && another.value2 == one.value2;
        }
        QJsonObject toJson() const
        {
            return QJsonObject{ { "value1", JsonStructHelper::Serialize(value1) }, { "value2", JsonStructHelper::Serialize(value2) } };
        }

        void loadJson(const QJsonValue &val)
        {
            JsonStructHelper::Deserialize(value1, val["value1"]);
            JsonStructHelper::Deserialize(value2, val["value2"]);
        }

      private:
        typedef QvPair<T1, T2> ___qvpair_t;
    };

    template<typename enumKey, typename TValue, typename = typename std::enable_if_t<std::is_enum_v<enumKey>>>
    struct QvEnumMap : QMap<enumKey, TValue>
    {
        // WARN: Changing this will break all existing JSON.
        static constexpr auto ENUM_JSON_KEY_PREFIX = "$";
        void loadJson(const QJsonValue &json_object)
        {
            QMap<QString, TValue> data;
            JsonStructHelper::Deserialize(data, json_object);
            this->clear();
            for (QString k_str : data.keys())
            {
                enumKey k = static_cast<enumKey>(k_str.remove(ENUM_JSON_KEY_PREFIX).toInt());
                this->insert(k, data[ENUM_JSON_KEY_PREFIX + k_str]);
            }
        }
        [[nodiscard]] const QJsonObject toJson() const
        {
            QMap<QString, TValue> data;
            for (const auto &k : this->keys())
            {
                data[ENUM_JSON_KEY_PREFIX + QString::number(k)] = this->value(k);
            }
            return JsonStructHelper::Serialize(data).toObject();
        }
    };

} // namespace Qv2ray::base::safetype

using namespace Qv2ray::base::safetype;
