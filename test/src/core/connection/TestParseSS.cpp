#include "libs/QJsonStruct/QJsonIO.hpp"
#include "src/core/connection/Serialization.hpp"
#define CATCH_CONFIG_MAIN
#include "catch.hpp"
using namespace Qv2ray::core::connection::Serialization;

SCENARIO("Test Parse Shadowsocks url", "[ParseSSUrl]")
{

    GIVEN("A shadowsocks server object")
    {
        ShadowSocksServerObject s;
        QString err;
        QString alias = "ssurl1";
        WHEN("the url without padding")
        {
            auto c = ss::Deserialize("ss://YmYtY2ZiOnRlc3RAMTkyLjE2OC4xMDAuMTo4ODg4", &alias, &err);
            s = ShadowSocksServerObject::fromJson(QJsonIO::GetValue(c, "outbounds", 0, "settings", "servers", 0));
            REQUIRE(s.address == "192.168.100.1");
            REQUIRE(s.port == 8888);
            REQUIRE(s.password == "test");
            REQUIRE(s.method == "bf-cfb");
        }
        WHEN("the url with padding")
        {
            auto c = ss::Deserialize("ss://YmYtY2ZiOnRlc3RAMTkyLjE2OC4xLjE6ODM4OA==", &alias, &err);
            s = ShadowSocksServerObject::fromJson(QJsonIO::GetValue(c, "outbounds", 0, "settings", "servers", 0));
            REQUIRE(s.address == "192.168.1.1");
            REQUIRE(s.port == 8388);
            REQUIRE(s.password == "test");
            REQUIRE(s.method == "bf-cfb");
        }
        WHEN("the with remarks")
        {
            auto c = ss::Deserialize(",ss://YmYtY2ZiOnRlc3RAMTkyLjE2OC4xMDAuMTo4ODg4#example-server", &alias, &err);
            s = ShadowSocksServerObject::fromJson(QJsonIO::GetValue(c, "outbounds", 0, "settings", "servers", 0));
            qDebug() << s.address << '\n';
            REQUIRE(s.address == "192.168.100.1");
            REQUIRE(s.port == 8888);
            REQUIRE(s.password == "test");
            REQUIRE(s.method == "bf-cfb");
        }
    }
}
