#include "3rdparty/QJsonStruct/QJsonIO.hpp"
#include "Common.hpp"
#include "src/core/connection/Serialization.hpp"
#define CATCH_CONFIG_MAIN
#include "catch.hpp"

TEST_CASE("Test VLESS URL Parsing")
{
    QvTestApplication app;
    QString alias;
    QString errMessage;

    SECTION("VLESSTCPXTLSSplice")
    {
        const static auto url = "vless://b0dd64e4-0fbd-4038-9139-d1f32a68a0dc@qv2ray.net:3279?security=xtls&flow=rprx-xtls-splice#VLESSTCPXTLSSplice";

        const auto result = vless::Deserialize(url, &alias, &errMessage);

        INFO("Parsed: " << QJsonDocument(result).toJson().toStdString());
        REQUIRE(errMessage.isEmpty());
        REQUIRE(alias.toStdString() == "VLESSTCPXTLSSplice");
    }
}
