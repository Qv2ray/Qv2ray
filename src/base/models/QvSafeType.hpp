#pragma once
#include "3rdparty/QJsonStruct/QJsonStruct.hpp"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMap>

template<typename placeholder, typename BASETYPE_T>
class SAFETYPE_IMPL : public BASETYPE_T
{
  public:
    template<class... Args>
    explicit SAFETYPE_IMPL(Args... args) : BASETYPE_T(args...){};
    const BASETYPE_T &raw() const
    {
        return *this;
    }
};

#define SAFE_TYPEDEF(BASE, CLASS)                                                                                                                    \
    class __##CLASS##__;                                                                                                                             \
    typedef SAFETYPE_IMPL<__##CLASS##__, BASE> CLASS;

#define nothing
namespace Qv2ray::base::safetype
{
    // To prevent anonying QJsonObject misuse
    SAFE_TYPEDEF(QJsonObject, INBOUNDSETTING);
    SAFE_TYPEDEF(QJsonObject, OUTBOUNDSETTING);
    SAFE_TYPEDEF(QJsonObject, INBOUND);
    SAFE_TYPEDEF(QJsonObject, OUTBOUND);
    SAFE_TYPEDEF(QJsonObject, CONFIGROOT);
    SAFE_TYPEDEF(QJsonObject, ROUTING);
    SAFE_TYPEDEF(QJsonObject, ROUTERULE);
    //
    SAFE_TYPEDEF(QJsonArray, OUTBOUNDS);
    SAFE_TYPEDEF(QJsonArray, INBOUNDS);

    template<typename T1, typename T2 = T1>
    struct QvPair
    {
        T1 value1;
        T2 value2;
        friend bool operator==(const QvPair<T1, T2> &one, const QvPair<T1, T2> &another)
        {
            return another.value1 == one.value1 && another.value2 == one.value2;
        }
        JSONSTRUCT_REGISTER(___qvpair_t, F(value1, value2))

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
            return JsonStructHelper::Serialize(data).toObject();
        }
    };

} // namespace Qv2ray::base::safetype

using namespace Qv2ray::base::safetype;
