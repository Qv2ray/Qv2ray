#include "3rdparty/QJsonStruct/QJsonIO.hpp"

#include <QJsonDocument>
#include <QJsonObject>
#define CATCH_CONFIG_MAIN
#include "catch.hpp"

TEST_CASE("QJsonIO Get Simple Value")
{
    const auto obj = QJsonObject{
        { "string", "a quick brown fox jumps over the lazy dog" }, //
        { "integer", -32767 },                                     //
        { "boolean", true },                                       //
        { "decimal", 0.618 },                                      //
    };
    const auto arr = QJsonArray{ obj };

    SECTION("Get Array Object")
    {
        const auto val = QJsonIO::GetValue(arr, 0);
        REQUIRE(val.isObject());
        REQUIRE(val.toObject() == obj);
    }

    SECTION("Get Object String")
    {
        const auto val = QJsonIO::GetValue(obj, "string");
        REQUIRE(val.isString());
        REQUIRE(val.toString() == obj["string"].toString());
    }

    SECTION("Get Object Integer")
    {
        const auto val = QJsonIO::GetValue(obj, "integer");
        REQUIRE(val.toInt() == obj["integer"].toInt());
    }

    SECTION("Get Object Boolean")
    {
        const auto val = QJsonIO::GetValue(obj, "boolean");
        REQUIRE(val.isBool());
        REQUIRE(val.toBool() == obj["boolean"].toBool());
    }

    SECTION("Get Object Decimal")
    {
        const auto val = QJsonIO::GetValue(obj, "decimal");
        REQUIRE(val.isDouble());
        REQUIRE(val.toDouble() == obj["decimal"].toDouble());
    }
}

TEST_CASE("QJsonIO Get Simple Value Using std::tuple")
{
    const auto obj = QJsonObject{
        { "string", "a quick brown fox jumps over the lazy dog" }, //
        { "integer", -32767 },                                     //
        { "boolean", true },                                       //
        { "decimal", 0.618 },                                      //
    };
    const auto arr = QJsonArray{ obj };

    SECTION("Get Array Object")
    {
        const auto val = QJsonIO::GetValue(arr, std::tuple{ 0 });
        REQUIRE(val.isObject());
        REQUIRE(val.toObject() == obj);
    }

    SECTION("Get Object String")
    {
        const auto val = QJsonIO::GetValue(obj, std::tuple{ "string" });
        REQUIRE(val.isString());
        REQUIRE(val.toString() == obj["string"].toString());
    }

    SECTION("Get Object Integer")
    {
        const auto val = QJsonIO::GetValue(obj, std::tuple{ "integer" });
        REQUIRE(val.toInt() == obj["integer"].toInt());
    }

    SECTION("Get Object Boolean")
    {
        const auto val = QJsonIO::GetValue(obj, std::tuple{ "boolean" });
        REQUIRE(val.isBool());
        REQUIRE(val.toBool() == obj["boolean"].toBool());
    }

    SECTION("Get Object Decimal")
    {
        const auto val = QJsonIO::GetValue(obj, std::tuple{ "decimal" });
        REQUIRE(val.isDouble());
        REQUIRE(val.toDouble() == obj["decimal"].toDouble());
    }
}

TEST_CASE("QJsonIO Set Simple Value")
{
    auto obj = QJsonObject{};
    auto arr = QJsonArray{ obj };

    SECTION("Set Object String")
    {
        QJsonIO::SetValue(obj, "qv2ray_test", "string");
        const auto val = QJsonIO::GetValue(obj, "string");
        REQUIRE(val.isString());
        REQUIRE(val.toString().toStdString() == "qv2ray_test");
    }

    SECTION("Set Object Decimal")
    {
        QJsonIO::SetValue(obj, 13.14, "decimal");
        const auto val = QJsonIO::GetValue(obj, "decimal");
        REQUIRE(val.isDouble());
        REQUIRE(val.toDouble() == 13.14);
    }

    SECTION("Set Object Integer")
    {
        QJsonIO::SetValue(obj, 114514, "integer");
        const auto val = QJsonIO::GetValue(obj, "integer");
        REQUIRE(val.toInt() == 114514);
    }

    SECTION("Set Object Boolean")
    {
        QJsonIO::SetValue(obj, true, "boolean");
        const auto val = QJsonIO::GetValue(obj, "boolean");
        REQUIRE(val.isBool());
        REQUIRE(val.toBool() == true);
    }
}

TEST_CASE("QJsonIO Set Path")
{

    SECTION("Path Test 1")
    {
        auto obj = QJsonObject{};
        QJsonIO::SetValue(obj, "vmess", "outbounds", 0, "protocol");
        QJsonIO::SetValue(obj, "0.0.0.0", "outbounds", 0, "sendThrough");
        QJsonIO::SetValue(obj, "114.51.41.191", "outbounds", 0, "settings", "vnext", 0, "address");
        QJsonIO::SetValue(obj, 9810, "outbounds", 0, "settings", "vnext", 0, "port");
        INFO("QJsonIO Result:" << QJsonDocument(obj).toJson().toStdString());

        const auto ans = QJsonObject{
            { "outbounds",
              QJsonArray{ QJsonObject{
                  { "protocol", "vmess" },
                  { "sendThrough", "0.0.0.0" },
                  { "settings", QJsonObject{ { "vnext", QJsonArray{ QJsonObject{ { "address", "114.51.41.191" }, { "port", 9810 } } } } } } } } }
        };
        INFO("QJsonObject Result: " << QJsonDocument(ans).toJson().toStdString())

        REQUIRE(ans == obj);
    }

    SECTION("Path remove test")
    {
        auto obj = QJsonObject{ { "qv2ray", "shit" } };
        QJsonIO::SetValue(obj, QJsonValue(QJsonValue::Undefined), "qv2ray");

        REQUIRE(obj == QJsonObject{});
    }
}
