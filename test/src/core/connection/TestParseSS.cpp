#include "libs/QJsonStruct/QJsonIO.hpp"
#include "src/core/connection/Serialization.hpp"

#include <QTest>

class ParseSSUrl : public QObject
{
    Q_OBJECT
  private slots:
    void t1()
    {
        using namespace Qv2ray::core::connection::Serialization;
        QString err;
        QString alias = "ssurl1";
        auto c = ss::Deserialize("ss://YmYtY2ZiOnRlc3RAMTkyLjE2OC4xMDAuMTo4ODg4", &alias, &err);
        auto s = ShadowSocksServerObject::fromJson(QJsonIO::GetValue(c, "outbounds", 0, "settings", "servers", 0));
        QVERIFY(s.address == "192.168.100.1");
        QVERIFY(s.port == 8888);
        QVERIFY(s.password == "test");
        QVERIFY(s.method == "bf-cfb");
    }
    void t2()
    {
        using namespace Qv2ray::core::connection::Serialization;
        QString err;
        QString alias = "ssurl1";
        auto c = ss::Deserialize("ss://YmYtY2ZiOnRlc3RAMTkyLjE2OC4xLjE6ODM4OA==", &alias, &err);
        auto s = ShadowSocksServerObject::fromJson(QJsonIO::GetValue(c, "outbounds", 0, "settings", "servers", 0));
        QVERIFY(s.address == "192.168.1.1");
        QVERIFY(s.port == 8388);
        QVERIFY(s.password == "test");
        QVERIFY(s.method == "bf-cfb");
    }
    void t3()
    {
        using namespace Qv2ray::core::connection::Serialization;
        QString err;
        QString alias = "ssurl1";
        auto c = ss::Deserialize(",ss://YmYtY2ZiOnRlc3RAMTkyLjE2OC4xMDAuMTo4ODg4#example-server",&alias, &err);
        auto s = ShadowSocksServerObject::fromJson(QJsonIO::GetValue(c, "outbounds", 0, "settings", "servers", 0));
        qDebug()<<s.address<<'\n';
        QVERIFY(s.address == "192.168.100.1");
        QVERIFY(s.port == 8888);
        QVERIFY(s.password == "test");
        QVERIFY(s.method == "bf-cfb");
    }
};

QTEST_MAIN(ParseSSUrl)
#include "TestParseSS.moc"
