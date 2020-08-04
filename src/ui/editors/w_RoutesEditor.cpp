#include "w_RoutesEditor.hpp"

#include "components/plugins/QvPluginHost.hpp"
#include "core/connection/Generation.hpp"
#include "core/handler/ConfigHandler.hpp"
#include "ui/common/UIBase.hpp"
#include "ui/node/NodeBase.hpp"
#include "ui/node/models/InboundNodeModel.hpp"
#include "ui/node/models/OutboundNodeModel.hpp"
#include "ui/node/models/RuleNodeModel.hpp"
#include "ui/widgets/DnsSettingsWidget.hpp"
#include "ui/widgets/complex/ChainEditorWidget.hpp"
#include "ui/widgets/complex/RoutingEditorWidget.hpp"
#include "ui/windows/w_ImportConfig.hpp"
#include "w_InboundEditor.hpp"
#include "w_JsonEditor.hpp"
#include "w_OutboundEditor.hpp"

#include <nodes/ConnectionStyle>
#include <nodes/FlowScene>
#include <nodes/FlowView>
#include <nodes/FlowViewStyle>
#include <nodes/Node>

#ifdef QT_DEBUG
    #include "nodes/../../src/ConnectionPainter.hpp"
#endif

using namespace QtNodes;
using namespace Qv2ray::ui::nodemodels;

namespace
{
    constexpr auto NODE_TAB_ROUTE_EDITOR = 0;
    constexpr auto NODE_TAB_CHAIN_EDITOR = 1;
} // namespace

#define LOADINGCHECK                                                                                                                            \
    if (isLoading)                                                                                                                              \
        return;
#define LOAD_FLAG_BEGIN isLoading = true;
#define LOAD_FLAG_END isLoading = false;

void RouteEditor::updateColorScheme()
{
    // Setup icons according to the theme settings.
    addInboundBtn->setIcon(QICON_R("add"));
    addOutboundBtn->setIcon(QICON_R("add"));
    //
    const static auto darkConnectionStyle = R"({"ConnectionStyle": {"ConstructionColor": "gray","NormalColor": "black","SelectedColor": "gray",
                                             "SelectedHaloColor": "deepskyblue","HoveredColor": "deepskyblue","LineWidth": 3.0,
                                             "ConstructionLineWidth": 2.0,"PointDiameter": 10.0,"UseDataDefinedColors": true}})";
    const static auto lightNodeStyle = R"({"NodeStyle": {"NormalBoundaryColor": "darkgray","SelectedBoundaryColor": "deepskyblue",
                                        "GradientColor0": "mintcream","GradientColor1": "mintcream","GradientColor2": "mintcream",
                                        "GradientColor3": "mintcream","ShadowColor": [200, 200, 200],"FontColor": [10, 10, 10],
                                        "FontColorFaded": [100, 100, 100],"ConnectionPointColor": "white","PenWidth": 2.0,"HoveredPenWidth": 2.5,
                                        "ConnectionPointDiameter": 10.0,"Opacity": 1.0}})";
    const static auto lightViewStyle =
        R"({"FlowViewStyle": {"BackgroundColor": [255, 255, 240],"FineGridColor": [245, 245, 230],"CoarseGridColor": [235, 235, 220]}})";
    const static auto lightConnectionStyle = R"({"ConnectionStyle": {"ConstructionColor": "gray","NormalColor": "black","SelectedColor": "gray",
                                              "SelectedHaloColor": "deepskyblue","HoveredColor": "deepskyblue","LineWidth": 3.0,"ConstructionLineWidth": 2.0,
                                              "PointDiameter": 10.0,"UseDataDefinedColors": false}})";
    if (GlobalConfig.uiConfig.useDarkTheme)
    {
        QtNodes::NodeStyle::reset();
        QtNodes::FlowViewStyle::reset();
        ConnectionStyle::setConnectionStyle(darkConnectionStyle);
    }
    else
    {
        QtNodes::NodeStyle::setNodeStyle(lightNodeStyle);
        QtNodes::FlowViewStyle::setStyle(lightViewStyle);
        ConnectionStyle::setConnectionStyle(lightConnectionStyle);
    }
}

RouteEditor::RouteEditor(QJsonObject connection, QWidget *parent) : QvDialog(parent), root(connection), original(connection)
{
    setupUi(this);
    QvMessageBusConnect(RouteEditor);
    //
    isLoading = true;
    setWindowFlags(windowFlags() | Qt::WindowMaximizeButtonHint);
    updateColorScheme();
    //
    // Do not change the order.
    nodeDispatcher = std::make_shared<NodeDispatcher>();
    ruleWidget = new RoutingEditorWidget(nodeDispatcher, ruleEditorUIWidget);
    chainWidget = new ChainEditorWidget(nodeDispatcher, chainEditorUIWidget);
    dnsWidget = new DnsSettingsWidget(this);
    nodeDispatcher->InitializeScenes(ruleWidget->getScene(), chainWidget->getScene());
    connect(nodeDispatcher.get(), &NodeDispatcher::OnOutboundCreated, this, &RouteEditor::OnDispatcherOutboundCreated);
    connect(nodeDispatcher.get(), &NodeDispatcher::OnOutboundDeleted, this, &RouteEditor::OnDispatcherOutboundDeleted);
    connect(nodeDispatcher.get(), &NodeDispatcher::OnRuleCreated, this, &RouteEditor::OnDispatcherRuleCreated);
    connect(nodeDispatcher.get(), &NodeDispatcher::OnInboundOutboundNodeHovered, this, &RouteEditor::OnDispatcherInboundOutboundHovered);
    connect(nodeDispatcher.get(), &NodeDispatcher::RequestEditChain, this, &RouteEditor::OnDispatcherEditChainRequested);
    connect(nodeDispatcher.get(), &NodeDispatcher::OnObjectTagChanged, this, &RouteEditor::OnDispatcherObjectTagChanged);
    //
#define SETLAYOUT(parent, child)                                                                                                                \
    {                                                                                                                                           \
        if (!parent->layout())                                                                                                                  \
        {                                                                                                                                       \
            parent->setLayout(new QVBoxLayout());                                                                                               \
        }                                                                                                                                       \
        auto l = parent->layout();                                                                                                              \
        l->addWidget(child);                                                                                                                    \
        l->setContentsMargins(0, 0, 0, 0);                                                                                                      \
        l->setSpacing(0);                                                                                                                       \
    }
    SETLAYOUT(ruleEditorUIWidget, ruleWidget);
    SETLAYOUT(chainEditorUIWidget, chainWidget);
    SETLAYOUT(dnsEditorUIWidget, dnsWidget);
    //
    nodeDispatcher->LoadFullConfig(root);
    dnsWidget->SetDNSObject(DNSObject::fromJson(root["dns"].toObject()));
    //
    domainStrategy = root["routing"].toObject()["domainStrategy"].toString();
    domainStrategyCombo->setCurrentText(domainStrategy);
    //
    // Set default outboung combo text AFTER adding all outbounds.
    defaultOutboundTag = getTag(OUTBOUND(root["outbounds"].toArray().first().toObject()));
    defaultOutboundCombo->setCurrentText(defaultOutboundTag);

    for (const auto &group : ConnectionManager->AllGroups())
    {
        importGroupBtn->addItem(GetDisplayName(group), group.toString());
    }
    isLoading = false;
}

QvMessageBusSlotImpl(RouteEditor)
{
    switch (msg)
    {
        MBShowDefaultImpl;
        MBHideDefaultImpl;
        MBRetranslateDefaultImpl;
        MBUpdateColorSchemeDefaultImpl;
    }
}

void RouteEditor::OnDispatcherInboundOutboundHovered(const QString &tag, const ProtocolSettingsInfoObject &info)
{
    tagLabel->setText(tag);
    hostLabel->setText(info.address);
    portLabel->setNum(info.port);
    protocolLabel->setText(info.protocol);
}

void RouteEditor::OnDispatcherOutboundCreated(std::shared_ptr<OutboundObjectMeta> out, QtNodes::Node &)
{
    if (out->metaType != METAOUTBOUND_BALANCER)
        defaultOutboundCombo->addItem(out->getDisplayName());
}

void RouteEditor::OnDispatcherRuleCreated(std::shared_ptr<RuleObject> rule, QtNodes::Node &)
{
    ruleListWidget->addItem(rule->QV2RAY_RULE_TAG);
}

void RouteEditor::OnDispatcherOutboundDeleted(const OutboundObjectMeta &data)
{
    const auto id = defaultOutboundCombo->findText(data.getDisplayName());
    if (id >= 0)
    {
        defaultOutboundCombo->removeItem(id);
    }
}

void RouteEditor::OnDispatcherObjectTagChanged(ComplexTagNodeMode t, const QString original, const QString current)
{
    Q_UNUSED(original)
    Q_UNUSED(current)
    //
    if (t == NODE_INBOUND)
    {
        // Pass
    }
    else if (t == NODE_RULE)
    {
        for (auto i = 0; i < ruleListWidget->count(); i++)
        {
            if (ruleListWidget->item(i)->text() == original)
                ruleListWidget->item(i)->setText(current);
        }
    }
    else if (t == NODE_OUTBOUND)
    {
        const auto id = defaultOutboundCombo->findText(original);
        if (id >= 0)
            defaultOutboundCombo->setItemText(id, current);
    }
}

void RouteEditor::OnDispatcherEditChainRequested(const QString &)
{
    nodesTab->setCurrentIndex(NODE_TAB_CHAIN_EDITOR);
}

CONFIGROOT RouteEditor::OpenEditor()
{
    auto result = this->exec();
    if (result != QDialog::Accepted)
        return original;

    const auto &[inbounds, rules, outbounds] = nodeDispatcher->GetData();
    //
    // Inbounds
    QJsonArray inboundsJson;
    for (const auto &in : inbounds)
    {
        inboundsJson << in;
    }
    root["inbounds"] = inboundsJson;
    //
    // QJsonArray rules
    QJsonArray rulesJsonArray;
    for (auto i = 0; i < ruleListWidget->count(); i++)
    {
        // Sorted
        const auto ruleTag = ruleListWidget->item(i)->text();
        if (rules.contains(ruleTag))
        {
            const auto &ruleObject = rules[ruleTag];
            auto ruleJson = ruleObject.toJson();
            // Remove some empty fields.
            // if (ruleObject.port.isEmpty())
            //    ruleJson.remove("port");
            // if (ruleObject.network.isEmpty())
            //    ruleJson.remove("network");
            JAUTOREMOVE(ruleJson, "network");
            JAUTOREMOVE(ruleJson, "port");
            JAUTOREMOVE(ruleJson, "outboundTag");
            JAUTOREMOVE(ruleJson, "balancerTag");
            rulesJsonArray << ruleJson;
        }
        else
        {
            LOG(MODULE_UI, "Could not find rule tag: " + ruleTag)
        }
    }
    //
    // QJsonArray balancers
    QJsonArray balancersArray;
    for (const auto &out : outbounds)
    {
        if (out.metaType != METAOUTBOUND_BALANCER)
            continue;
        BalancerObject o;
        o.tag = out.getDisplayName();
        o.selector = out.outboundTags;
        balancersArray << o.toJson();
    }

    QJsonObject routingObject;
    routingObject["domainStrategy"] = domainStrategy;
    routingObject["rules"] = rulesJsonArray;
    routingObject["balancers"] = balancersArray;
    root["routing"] = routingObject;

    //
    // QJsonArray Outbounds
    QJsonArray outboundsArray;

    for (const auto &out : outbounds)
    {
        QJsonObject outboundJsonObject;
        if (out.metaType == METAOUTBOUND_BALANCER)
            continue;
        if (out.metaType == METAOUTBOUND_ORIGINAL)
        {
            outboundJsonObject = out.realOutbound;
        }
        outboundJsonObject[META_OUTBOUND_KEY_NAME] = out.toJson();
        const auto displayName = out.getDisplayName();
        if (displayName == defaultOutboundTag)
            outboundsArray.prepend(outboundJsonObject);
        else
            outboundsArray.append(outboundJsonObject);
    }
    root["outbounds"] = outboundsArray;
    root["dns"] = GenerateDNS(false, dnsWidget->GetDNSObject());
    return root;
}

RouteEditor::~RouteEditor()
{
    nodeDispatcher->LockOperation();
}

void RouteEditor::on_buttonBox_accepted()
{
}

void RouteEditor::on_insertDirectBtn_clicked()
{
    auto freedom = GenerateFreedomOUT("AsIs", "", 0);
    auto tag = "Freedom_" + QSTRN(QTime::currentTime().msecsSinceStartOfDay());
    auto out = GenerateOutboundEntry(tag, "freedom", freedom, {});
    // ADD NODE
    const auto _ = nodeDispatcher->CreateOutbound(make_normal_outbound(out));
    statusLabel->setText(tr("Added DIRECT outbound"));
}

void RouteEditor::on_addDefaultBtn_clicked()
{
    LOADINGCHECK
    // Add default connection from GlobalConfig
    //
    const auto &inboundConfig = GlobalConfig.inboundConfig;
    const static QJsonObject sniffingOff{ { "enabled", false } };
    const static QJsonObject sniffingOn{ { "enabled", true }, { "destOverride", QJsonArray{ "http", "tls" } } };
    //
    if (inboundConfig.useHTTP)
    {
        const auto httpSettings = GenerateHTTPIN(inboundConfig.httpSettings.useAuth, { inboundConfig.httpSettings.account });
        INBOUND httpConfig = GenerateInboundEntry("GlobalConfig-HTTP", "http",     //
                                                  inboundConfig.listenip,          //
                                                  inboundConfig.httpSettings.port, //
                                                  httpSettings,                    //
                                                  inboundConfig.httpSettings.sniffing ? sniffingOn : sniffingOff);
        auto _ = nodeDispatcher->CreateInbound(httpConfig);
    }
    if (inboundConfig.useSocks)
    {
        auto socks = GenerateSocksIN((inboundConfig.socksSettings.useAuth ? "password" : "noauth"), //
                                     { inboundConfig.socksSettings.account },                       //
                                     inboundConfig.socksSettings.enableUDP,                         //
                                     inboundConfig.socksSettings.localIP);
        auto socksConfig = GenerateInboundEntry("GlobalConfig-Socks", "socks",    //
                                                inboundConfig.listenip,           //
                                                inboundConfig.socksSettings.port, //
                                                socks,                            //
                                                (inboundConfig.socksSettings.sniffing ? sniffingOn : sniffingOff));
        auto _ = nodeDispatcher->CreateInbound(socksConfig);
    }

    if (inboundConfig.useTPROXY)
    {
        QList<QString> networks;
#define ts inboundConfig.tProxySettings
        if (ts.hasTCP)
            networks << "tcp";
        if (ts.hasUDP)
            networks << "udp";
        const auto tproxy_network = networks.join(",");
        auto tproxyInSettings = GenerateDokodemoIN("", 0, tproxy_network, 0, true, 0);
        //
        const static QJsonObject tproxy_sniff{ { "enabled", true }, { "destOverride", QJsonArray{ "http", "tls" } } };
        const QJsonObject tproxy_streamSettings{ { "sockopt", QJsonObject{ { "tproxy", ts.mode } } } };
        {
            auto tProxyIn = GenerateInboundEntry("tProxy IPv4", "dokodemo-door", ts.tProxyIP, ts.port, tproxyInSettings);
            tProxyIn.insert("sniffing", tproxy_sniff);
            tProxyIn.insert("streamSettings", tproxy_streamSettings);
            auto _ = nodeDispatcher->CreateInbound(tProxyIn);
        }
        if (!ts.tProxyV6IP.isEmpty())
        {
            auto tProxyV6In = GenerateInboundEntry("tProxy IPv6", "dokodemo-door", ts.tProxyV6IP, ts.port, tproxyInSettings);
            tProxyV6In.insert("sniffing", tproxy_sniff);
            tProxyV6In.insert("streamSettings", tproxy_streamSettings);
            auto _ = nodeDispatcher->CreateInbound(tProxyV6In);
        }
#undef ts
    }
}

void RouteEditor::on_insertBlackBtn_clicked()
{
    LOADINGCHECK
    auto blackHole = GenerateBlackHoleOUT(false);
    auto tag = "BlackHole-" + QSTRN(QTime::currentTime().msecsSinceStartOfDay());
    auto outbound = GenerateOutboundEntry(tag, "blackhole", blackHole, {});
    const auto _ = nodeDispatcher->CreateOutbound(make_normal_outbound(outbound));
}

void RouteEditor::on_addInboundBtn_clicked()
{
    LOADINGCHECK
    InboundEditor w(INBOUND(), this);
    auto _result = w.OpenEditor();

    if (w.result() == QDialog::Accepted)
    {
        auto _ = nodeDispatcher->CreateInbound(_result);
    }
}

void RouteEditor::on_addOutboundBtn_clicked()
{
    LOADINGCHECK
    OutboundEditor w(OUTBOUND(), this);
    auto _result = w.OpenEditor();

    if (w.result() == QDialog::Accepted)
    {
        auto _ = nodeDispatcher->CreateOutbound(make_normal_outbound(_result));
    }
}

void RouteEditor::on_domainStrategyCombo_currentIndexChanged(const QString &arg1)
{
    LOADINGCHECK
    domainStrategy = arg1;
}

void RouteEditor::on_defaultOutboundCombo_currentTextChanged(const QString &arg1)
{
    LOADINGCHECK
    if (defaultOutboundTag != arg1)
    {
        LOG(MODULE_UI, "Default outbound changed: " + arg1)
        defaultOutboundTag = arg1;
    }
}

void RouteEditor::on_importExistingBtn_clicked()
{
    const auto connId = ConnectionId{ importConnBtn->currentData(Qt::UserRole).toString() };
    const auto root = ConnectionManager->GetConnectionRoot(connId);
    auto outbound = OUTBOUND(root["outbounds"].toArray()[0].toObject());
    outbound["tag"] = GetDisplayName(connId);
    auto _ = nodeDispatcher->CreateOutbound(make_normal_outbound(outbound));
}

void RouteEditor::on_importGroupBtn_currentIndexChanged(int)
{
    const auto group = GroupId{ importGroupBtn->currentData(Qt::UserRole).toString() };
    importConnBtn->clear();
    for (const auto &connId : ConnectionManager->Connections(group))
    {
        importConnBtn->addItem(GetDisplayName(connId), connId.toString());
    }
}

void RouteEditor::on_addBalancerBtn_clicked()
{
    auto _ = nodeDispatcher->CreateOutbound(make_balancer_outbound({}, "Balancer"));
}

void RouteEditor::on_addChainBtn_clicked()
{
    auto _ = nodeDispatcher->CreateOutbound(make_chained_outbound({}, "Chained Outbound"));
}

void RouteEditor::on_debugPainterCB_clicked(bool checked)
{
#ifdef QT_DEBUG
    QtNodes::ConnectionPainter::IsDebuggingEnabled = checked;
    ruleWidget->getScene()->update();
    chainWidget->getScene()->update();
#endif
}

void RouteEditor::on_linkExistingBtn_clicked()
{
    const auto cid = ConnectionId{ importConnBtn->currentData(Qt::UserRole).toString() };
    auto _ = nodeDispatcher->CreateOutbound(make_external_outbound(cid, GetDisplayName(cid)));
}

void RouteEditor::on_importOutboundBtn_clicked()
{
    LOADINGCHECK
    ImportConfigWindow w(this);
    // True here for not keep the inbounds.
    auto configs = w.SelectConnection(true);

    for (auto i = 0; i < configs.count(); i++)
    {
        auto conf = configs.values()[i];
        auto name = configs.key(conf, "");

        if (name.isEmpty())
            continue;

        // conf is rootObject, needs to unwrap it.
        auto confList = conf["outbounds"].toArray();

        for (int i = 0; i < confList.count(); i++)
        {
            auto _ = nodeDispatcher->CreateOutbound(make_normal_outbound(OUTBOUND(confList[i].toObject())));
        }
    }
}
