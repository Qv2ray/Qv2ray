#pragma once
#include "libs/QJsonStruct/QJsonStruct.hpp"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMap>

#define SAFE_TYPEDEF_EXTRA(Base, name, extra)                                                                                                   \
    class name : public Base                                                                                                                    \
    {                                                                                                                                           \
      public:                                                                                                                                   \
        template<class... Args>                                                                                                                 \
        explicit name(Args... args) : Base(args...)                                                                                             \
        {                                                                                                                                       \
        }                                                                                                                                       \
        const Base &raw() const                                                                                                                 \
        {                                                                                                                                       \
            return *this;                                                                                                                       \
        }                                                                                                                                       \
        extra                                                                                                                                   \
    }

#define nothing
#define SAFE_TYPEDEF(Base, name) SAFE_TYPEDEF_EXTRA(Base, name, nothing)
namespace Qv2ray::base::safetype
{
    // To prevent anonying QJsonObject misuse
    SAFE_TYPEDEF(QJsonObject, INBOUNDSETTING);
    SAFE_TYPEDEF(QJsonObject, OUTBOUNDSETTING);
    SAFE_TYPEDEF(QJsonObject, INBOUND);
    SAFE_TYPEDEF(QJsonObject, OUTBOUND);
    SAFE_TYPEDEF(QJsonObject, CONFIGROOT);
    SAFE_TYPEDEF(QJsonObject, PROXYSETTING);
    //
    SAFE_TYPEDEF(QJsonArray, ROUTERULELIST);
    SAFE_TYPEDEF(QJsonArray, INOUTLIST);
    SAFE_TYPEDEF(QJsonObject, ROUTING);
    SAFE_TYPEDEF(QJsonObject, ROUTERULE);
    SAFE_TYPEDEF(INOUTLIST, OUTBOUNDS);
    SAFE_TYPEDEF(INOUTLIST, INBOUNDS);

    template<typename T>
    struct QvPair
    {
        T value1;
        T value2;
        JSONSTRUCT_REGISTER(QvPair<T>, F(value1, value2))
    };
    template<typename enumKey, typename TValue, typename = typename std::enable_if<std::is_enum<enumKey>::value>::type>
    struct QvEnumMap : QMap<enumKey, TValue>
    {
        static constexpr auto ENUM_JSON_KEY_PREFIX = "$";
        void loadJson(const QJsonValue &json_object)
        {
            QMap<QString, TValue> data;
            JsonStructHelper::___json_struct_load_data(data, json_object);
            this->clear();
            for (QString k_str : data.keys())
            {
                auto k = (enumKey) k_str.remove(ENUM_JSON_KEY_PREFIX).toInt();
                this->insert(k, data[ENUM_JSON_KEY_PREFIX + k_str]);
            }
        }
        [[nodiscard]] static auto fromJson(const QJsonValue &json)
        {
            QvEnumMap t;
            t.loadJson(json);
            return t;
        }
        [[nodiscard]] const QJsonObject toJson() const
        {
            QMap<QString, TValue> data;
            for (const auto &k : this->keys())
            {
                data[ENUM_JSON_KEY_PREFIX + QString::number(k)] = this->value(k);
            }
            return JsonStructHelper::___json_struct_store_data(data).toObject();
        }
    };

} // namespace Qv2ray::base::safetype
