#include "3rdparty/QJsonStruct/QJsonIO.hpp"
#include "Common.hpp"
#include "src/core/connection/Serialization.hpp"
#define CATCH_CONFIG_MAIN
#include "catch.hpp"

SCENARIO("Test Parse VMess V2 url", "[ParseVMessV2]")
{
    QvTestApplication app;
    GIVEN("vmess+tcp")
    {
        QString _;
        const QString address = "42.255.255.254";
        const int alterId = 4;
        const QString uuid = "59f34e8c-f310-49b0-b240-11663e365601";
        const QString network = "tcp";
        const int port = 11451;
        const QString comment = "日本 VIP节点5 - 10Mbps带宽 苏州-日本 IPLC-CEN专线 游戏加速用 30倍流量比例 原生日本IP落地";

        WHEN("parse Qv2ray 2.5.0 generated uri")
        {
            const QString vmessString = "vmess://eyJhZGQiOiI0Mi4yNTUuMjU1LjI1NCIsImFpZCI6NCwiaWQiOiI1OWYzNGU4Yy1mMzEw"
                                        "LTQ5YjAtYjI0MC0xMTY2M2UzNjU2MDEiLCJuZXQiOiJ0Y3AiLCJwb3J0IjoxMTQ1MSwicHMiOiLm"
                                        "l6XmnKwgVklQ6IqC54K5NSAtIDEwTWJwc+W4puWuvSDoi4/lt54t5pel5pysIElQTEMtQ0VO5LiT"
                                        "57q/IOa4uOaIj+WKoOmAn+eUqCAzMOWAjea1gemHj+avlOS+iyDljp/nlJ/ml6XmnKxJUOiQveWc"
                                        "sCIsInRscyI6Im5vbmUiLCJ0eXBlIjoibm9uZSIsInYiOjJ9Cg==";

            QString commentParsed;
            const auto result = vmess::Deserialize(vmessString, &commentParsed, &_);
            INFO("Raw VMess: " << vmessString.toStdString());
            INFO("Parsed JSON: " << QJsonDocument(result).toJson().toStdString());

            const auto networkParsed = QJsonIO::GetValue(result, "outbounds", 0, "streamSettings", "network").toString();
            const auto addressParsed = QJsonIO::GetValue(result, "outbounds", 0, "settings", "vnext", 0, "address").toString();
            const auto portParsed = QJsonIO::GetValue(result, "outbounds", 0, "settings", "vnext", 0, "port").toInt();
            const auto idParsed = QJsonIO::GetValue(result, "outbounds", 0, "settings", "vnext", 0, "users", 0, "id").toString();
            const auto alterIdParsed = QJsonIO::GetValue(result, "outbounds", 0, "settings", "vnext", 0, "users", 0, "alterId").toInt();

            REQUIRE(commentParsed.toStdString() == comment.toStdString());
            REQUIRE(addressParsed.toStdString() == address.toStdString());
            REQUIRE(portParsed == port);
            REQUIRE(idParsed.toStdString() == uuid.toStdString());
            REQUIRE(alterIdParsed == alterId);
            REQUIRE(networkParsed.toStdString() == "");
        }
    }
}

SCENARIO("Test Parse VMess V1 url", "[ParseVMessV1]")
{
    QvTestApplication app;
    GIVEN("vmess+ws")
    {
        QString _;
        const QString address = "motherfucker.net";
        const int alterId = 0;
        const QString path = "/yaboviss";
        const QString network = "ws";
        const QString uuid = "40980939-f6bd-4b17-ad26-c2aed2f1b3fc";
        const QString comment = "good bye vmess v1";
        const int port = 8003;

        WHEN("parse all stringified vmess v1")
        {
            const QString vmessString = "vmess://eyJwcyI6Imdvb2QgYnllIHZtZXNzIHYxIiwiYWRkIjoibW90aGVyZnVja2VyLm5ldCIs"
                                        "InBvcnQiOiI4MDAzIiwiaWQiOiI0MDk4MDkzOS1mNmJkLTRiMTctYWQyNi1jMmFlZDJmMWIzZmMi"
                                        "LCJhaWQiOiIwIiwibmV0Ijoid3MiLCJ0eXBlIjoibm9uZSIsImhvc3QiOiIveWFib3Zpc3MiLCJ0"
                                        "bHMiOiIifQo=";

            QString commentParsed;
            const auto result = vmess::Deserialize(vmessString, &commentParsed, &_);
            INFO("Raw VMess: " << vmessString.toStdString());
            INFO("Parsed JSON: " << QJsonDocument(result).toJson().toStdString());

            const auto networkParsed = QJsonIO::GetValue(result, "outbounds", 0, "streamSettings", "network").toString();
            const auto addressParsed = QJsonIO::GetValue(result, "outbounds", 0, "settings", "vnext", 0, "address").toString();
            const auto portParsed = QJsonIO::GetValue(result, "outbounds", 0, "settings", "vnext", 0, "port").toInt();
            const auto idParsed = QJsonIO::GetValue(result, "outbounds", 0, "settings", "vnext", 0, "users", 0, "id").toString();
            const auto alterIdParsed = QJsonIO::GetValue(result, "outbounds", 0, "settings", "vnext", 0, "users", 0, "alterId").toInt();
            const auto typeParsed = QJsonIO::GetValue(result, "outbounds", 0, "streamSettings", "tcpSettings", "header", "type").toString();
            const auto tlsParsed = QJsonIO::GetValue(result, "outbounds", 0, "streamSettings", "security").toString();

            REQUIRE(commentParsed.toStdString() == comment.toStdString());
            REQUIRE(addressParsed.toStdString() == address.toStdString());
            REQUIRE(portParsed == port);
            REQUIRE(idParsed.toStdString() == uuid.toStdString());
            REQUIRE(alterIdParsed == alterId);
            REQUIRE(networkParsed.toStdString() == network.toStdString());
        }
    }
}
