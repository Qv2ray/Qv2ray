#include "src/components/latency/RealPing.hpp"
#include "uvw.hpp"
#define CATCH_CONFIG_MAIN
#include "catch.hpp"

int fakeArgc = 0;
char *fakeArgv[]{};

class QvTestApplication
    : public QCoreApplication
    , public Qv2rayApplicationInterface
{
  public:
    explicit QvTestApplication() : QCoreApplication(fakeArgc, fakeArgv), Qv2rayApplicationInterface(){};
    virtual void MessageBoxWarn(QWidget *, const QString &, const QString &, MessageOpt) override{};
    virtual void MessageBoxInfo(QWidget *, const QString &, const QString &, MessageOpt) override{};
    virtual MessageOpt MessageBoxAsk(QWidget *, const QString &, const QString &, const QList<MessageOpt> &) override
    {
        return {};
    };
    virtual void OpenURL(const QString &) override{};
};

SCENARIO("Test RealPing get proxy address", "[RealPing]")
{
    QvTestApplication app;
    GIVEN("A realping object")
    {
        auto loop = uvw::Loop::create();
        LatencyTestHost *p;
        LatencyTestRequest req;
        auto realping = std::make_shared<Qv2ray::components::latency::realping::RealPing>(loop, req, p);
        WHEN("test IPv4 any address")
        {
            GlobalConfig.inboundConfig.listenip = "0.0.0.0";
            GlobalConfig.inboundConfig.useHTTP = true;
            GlobalConfig.inboundConfig.httpSettings.useAuth = true;
            GlobalConfig.inboundConfig.httpSettings.port = 9090;
            GlobalConfig.inboundConfig.httpSettings.account.user = "usr";
            GlobalConfig.inboundConfig.httpSettings.account.pass = "pp";
            REQUIRE(realping->getProxyAddress() == "http://usr:pp@127.0.0.1:9090");
        }
        WHEN("test IPv6 any address")
        {
            GlobalConfig.inboundConfig.listenip = "::";
            GlobalConfig.inboundConfig.useHTTP = false;
            GlobalConfig.inboundConfig.useSocks = true;
            GlobalConfig.inboundConfig.socksSettings.useAuth = true;
            GlobalConfig.inboundConfig.socksSettings.port = 9090;
            GlobalConfig.inboundConfig.socksSettings.account.user = "ausr";
            GlobalConfig.inboundConfig.socksSettings.account.pass = "";
            REQUIRE(realping->getProxyAddress() == "socks5://ausr:@[::1]:9090");
        }
        WHEN("test IPv4 address without usr/pwd")
        {
            GlobalConfig.inboundConfig.listenip = "192.168.1.1";
            GlobalConfig.inboundConfig.useHTTP = true;
            GlobalConfig.inboundConfig.httpSettings.useAuth = false;
            GlobalConfig.inboundConfig.httpSettings.port = 9090;
            GlobalConfig.inboundConfig.httpSettings.account.user = "usr";
            GlobalConfig.inboundConfig.httpSettings.account.pass = "pp";
            REQUIRE(realping->getProxyAddress() == "http://192.168.1.1:9090");
        }
        WHEN("test IPv6 address without usr/pwd")
        {
            GlobalConfig.inboundConfig.listenip = "6:6:6:6:6:6:6:6";
            GlobalConfig.inboundConfig.useHTTP = false;
            GlobalConfig.inboundConfig.useSocks = true;
            GlobalConfig.inboundConfig.socksSettings.useAuth = false;
            GlobalConfig.inboundConfig.socksSettings.port = 9090;
            GlobalConfig.inboundConfig.socksSettings.account.user = "ausr";
            GlobalConfig.inboundConfig.socksSettings.account.pass = "";
            REQUIRE(realping->getProxyAddress() == "socks5://[6:6:6:6:6:6:6:6]:9090");
        }
    }
}
