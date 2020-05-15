#include <QTest>
#include "src/core/connection/Serialization.hpp"
#include "libs/QJsonStruct/QJsonIO.hpp"

class ParseSSUrl: public QObject {
    Q_OBJECT
private slots:
    void t1() { 
        using namespace Qv2ray::core::connection::Serialization;
        QString err;
        QString alias="ssurl1";
        auto c=ss::Deserialize("ss://YmYtY2ZiOnRlc3RAMTkyLjE2OC4xMDAuMTo4ODg4", &alias, &err);
        auto s=ShadowSocksServerObject::fromJson(c["outbounds"].toArray().first().toObject()["settings"].toObject()["servers"].toArray().first().toObject());
        QVERIFY(s.address == "192.168.100.1");
        QVERIFY(s.port == 8888);
        QVERIFY(s.password=="test");
        QVERIFY(s.method=="bf-cfb");
    }
};

QTEST_MAIN(ParseSSUrl)
#include "TestParseSS.moc"
