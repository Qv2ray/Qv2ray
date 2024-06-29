#include "3rdparty/QJsonStruct/QJsonIO.hpp"
#include "Common.hpp"
#include "src/core/connection/Serialization.hpp"
#define CATCH_CONFIG_MAIN
#include "catch.hpp"
using namespace Qv2ray::core::connection::serialization;

SCENARIO("Test Parse trojan url", "[ParseTrojanUrl]")
{
    QvTestApplication app;
    GIVEN("A trojan server object")
    {
        TrojanServerObject s;
        QString err;
        WHEN("simple url")
        {
            QString alias;
            auto c = trojan::Deserialize("trojan://passω0rd@255.255.255.1:8192#evil", &alias, &err);
            s = TrojanServerObject::fromJson(QJsonIO::GetValue(c, "outbounds", 0, "settings", "servers", 0));
            REQUIRE(s.address.toStdString() == "255.255.255.1");
            REQUIRE(s.port == 8192);
            REQUIRE(s.password.toStdString() == "passω0rd");
        }
        WHEN("password containing special characters")
        {
            QString alias;
            auto c = trojan::Deserialize("trojan://pAss:ω0rd@4.4.4.4:443#%E6%81%B6%E9%AD%94", &alias, &err);
            s = TrojanServerObject::fromJson(QJsonIO::GetValue(c, "outbounds", 0, "settings", "servers", 0));
            REQUIRE(s.address.toStdString() == "4.4.4.4");
            REQUIRE(s.port == 443);
            REQUIRE(s.password.toStdString() == "pAss:ω0rd");
            REQUIRE(alias == "恶魔");
        }
    }
}
