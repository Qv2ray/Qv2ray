#include "libs/QJsonStruct/QJsonIO.hpp"
#include "src/core/connection/Serialization.hpp"

#include <QTest>

class ParseVmessUrl : public QObject
{
  Q_OBJECT
  private slots:
    void t1()
    {
        QVERIFY(true);
    }
};

QTEST_MAIN(ParseVmessUrl)
#include "TestParseVmess.moc"
