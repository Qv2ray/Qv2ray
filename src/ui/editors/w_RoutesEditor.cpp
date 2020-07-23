#include "w_RoutesEditor.hpp"

#include "components/plugins/QvPluginHost.hpp"
#include "core/connection/Generation.hpp"
#include "core/handler/ConfigHandler.hpp"
#include "ui/common/UIBase.hpp"
#include "ui/node/models/InboundNodeModel.hpp"
#include "ui/node/models/OutboundNodeModel.hpp"
#include "ui/node/models/RuleNodeModel.hpp"
#include "ui/windows/w_ImportConfig.hpp"
#include "w_InboundEditor.hpp"
#include "w_JsonEditor.hpp"
#include "w_OutboundEditor.hpp"

#include <nodes/internal/FlowScene.hpp>
#include <nodes/internal/FlowView.hpp>
#include <nodes/internal/FlowViewStyle.hpp>
#include <nodes/internal/NodeStyle.hpp>

#ifdef QT_DEBUG
    #include "nodes/../../src/ConnectionPainter.hpp"
#endif

using QtNodes::FlowView;
using namespace Qv2ray::ui::nodemodels;

constexpr auto GRAPH_GLOBAL_OFFSET_X = -380;
constexpr auto GRAPH_GLOBAL_OFFSET_Y = -350;

//#define CurrentRule this->rules[this->currentRuleTag]
#define LOADINGCHECK                                                                                                                            \
    if (isLoading)                                                                                                                              \
        return;
#define LOAD_FLAG_BEGIN isLoading = true;
#define LOAD_FLAG_END isLoading = false;

void RouteEditor::SetupNodeWidget()
{
    if (GlobalConfig.uiConfig.useDarkTheme)
    {
        ConnectionStyle::setConnectionStyle(
            R"({"ConnectionStyle": {"ConstructionColor": "gray","NormalColor": "black","SelectedColor": "gray",
                                            "SelectedHaloColor": "deepskyblue","HoveredColor": "deepskyblue","LineWidth": 3.0,
                                            "ConstructionLineWidth": 2.0,"PointDiameter": 10.0,"UseDataDefinedColors": true}})");
    }
    else
    {
        QtNodes::NodeStyle::setNodeStyle(
            R"({"NodeStyle": {"NormalBoundaryColor": "darkgray","SelectedBoundaryColor": "deepskyblue",
                                            "GradientColor0": "mintcream","GradientColor1": "mintcream","GradientColor2": "mintcream",
                                            "GradientColor3": "mintcream","ShadowColor": [200, 200, 200],"FontColor": [10, 10, 10],
                                            "FontColorFaded": [100, 100, 100],"ConnectionPointColor": "white","PenWidth": 2.0,"HoveredPenWidth": 2.5,
                                            "ConnectionPointDiameter": 10.0,"Opacity": 1.0}})");
        QtNodes::FlowViewStyle::setStyle(
            R"({"FlowViewStyle": {"BackgroundColor": [255, 255, 240],"FineGridColor": [245, 245, 230],"CoarseGridColor": [235, 235, 220]}})");
        ConnectionStyle::setConnectionStyle(
            R"({"ConnectionStyle": {"ConstructionColor": "gray","NormalColor": "black","SelectedColor": "gray",
                                            "SelectedHaloColor": "deepskyblue","HoveredColor": "deepskyblue","LineWidth": 3.0,"ConstructionLineWidth": 2.0,
                                            "PointDiameter": 10.0,"UseDataDefinedColors": false}})");
    }
    nodeScene = new FlowScene(this);
    flowView = new FlowView(nodeScene, nodeGraphWidget);
    flowView->scaleDown();
    if (!nodeGraphWidget->layout())
    {
        // The QWidget will take ownership of layout.
        nodeGraphWidget->setLayout(new QVBoxLayout());
    }
    auto l = nodeGraphWidget->layout();
    l->addWidget(flowView);
    l->setContentsMargins(0, 0, 0, 0);
    l->setSpacing(0);
}

RouteEditor::RouteEditor(QJsonObject connection, QWidget *parent) : QvDialog(parent), root(connection), original(connection)
{
    setupUi(this);
    QvMessageBusConnect(RouteEditor);
    //
    isLoading = true;
    setWindowFlags(windowFlags() | Qt::WindowMaximizeButtonHint);
    SetupNodeWidget();
    updateColorScheme();
    //
    nodeDispatcher = std::make_shared<NodeDispatcher>(nodeScene);
    connect(nodeDispatcher.get(), &NodeDispatcher::OnInboundCreated, this, &RouteEditor::OnDispatcherInboundCreated);
    connect(nodeDispatcher.get(), &NodeDispatcher::OnOutboundCreated, this, &RouteEditor::OnDispatcherOutboundCreated);
    connect(nodeDispatcher.get(), &NodeDispatcher::OnRuleCreated, this, &RouteEditor::OnDispatcherRuleCreated);
    connect(nodeDispatcher.get(), &NodeDispatcher::OnInboundOutboundNodeHovered, this, &RouteEditor::OnDispatcherInboundOutboundHovered);
    //
    domainStrategy = root["routing"].toObject()["domainStrategy"].toString();
    domainStrategyCombo->setCurrentText(domainStrategy);
    //
    nodeDispatcher->LoadFullConfig(root);
    //// Set default outboung combo text AFTER adding all outbounds.
    const auto defaultOutbound = getTag(OUTBOUND(root["outbounds"].toArray().first().toObject()));
    defaultOutboundCombo->setCurrentText(defaultOutbound);
    //
    ////    // Find and add balancers.
    ////    for (auto _balancer : root["routing"].toObject()["balancers"].toArray())
    ////    {
    ////        auto _balancerObject = _balancer.toObject();
    ////        if (!_balancerObject["tag"].toString().isEmpty())
    ////        {
    ////            balancers.insert(_balancerObject["tag"].toString(), _balancerObject["selector"].toVariant().toStringList());
    ////        }
    ////    }
    //
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

void RouteEditor::OnDispatcherInboundCreated(std::shared_ptr<INBOUND>, QtNodes::Node &node)
{
    QPoint pos{ 0 + GRAPH_GLOBAL_OFFSET_X, nodeDispatcher->InboundsCount() * 130 + GRAPH_GLOBAL_OFFSET_Y };
    nodeScene->setNodePosition(node, pos);
}

void RouteEditor::OnDispatcherOutboundCreated(std::shared_ptr<OutboundObjectMeta> out, QtNodes::Node &node)
{
    QPoint pos = nodeGraphWidget->pos();
    pos.setX(pos.x() + 850 + GRAPH_GLOBAL_OFFSET_X);
    pos.setY(pos.y() + nodeDispatcher->OutboundsCount() * 120 + GRAPH_GLOBAL_OFFSET_Y);
    nodeScene->setNodePosition(node, pos);
    defaultOutboundCombo->addItem(out->getTag());
}

void RouteEditor::OnDispatcherRuleCreated(std::shared_ptr<RuleObject> rule, QtNodes::Node &node)
{
    auto pos = nodeGraphWidget->pos();
    pos.setX(pos.x() + 350 + GRAPH_GLOBAL_OFFSET_X);
    pos.setY(pos.y() + nodeDispatcher->RulesCount() * 120 + GRAPH_GLOBAL_OFFSET_Y);
    nodeScene->setNodePosition(node, pos);
    ruleListWidget->addItem(rule->QV2RAY_RULE_TAG);
}

void RouteEditor::OnDispatcherOutboundDeleted(const OutboundObjectMeta &data)
{
    const auto id = defaultOutboundCombo->findText(data.getTag());
    if (id > 0)
    {
        defaultOutboundCombo->removeItem(id);
    }
}

CONFIGROOT RouteEditor::OpenEditor()
{
    auto result = this->exec();

    auto x = nodeDispatcher->GetFullConfig();
    //    if (rules.isEmpty())
    //    {
    //        // Prevent empty rule list causing mis-detection of config type to
    //        // simple.
    //        on_addRouteBtn_clicked();
    //    }

    // // If clicking OK
    // if (result == QDialog::Accepted)
    // {
    // QJsonArray rulesArray;
    // QJsonArray _balancers;
    //
    //// Append rules by order
    // for (auto i = 0; i < ruleListWidget->count(); i++)
    //{
    //    auto _rule = rules[i];
    //    auto ruleJsonObject = _rule.toJson();
    //
    //    // Process balancer for a rule
    //    if (_rule.QV2RAY_RULE_USE_BALANCER)
    //    {
    //        // Do not use outbound tag.
    //        ruleJsonObject.remove("outboundTag");
    //
    //        //                // Find balancer list
    //        //                if (!balancers.contains(_rule.balancerTag))
    //        //                {
    //        //                    LOG(MODULE_UI, "Cannot find a balancer for tag: " + _rule.balancerTag)
    //        //                }
    //        //                else
    //        //                {
    //        //                    auto _balancerList = balancers[_rule.balancerTag];
    //        //                    QJsonObject balancerEntry;
    //        //                    balancerEntry["tag"] = _rule.balancerTag;
    //        //                    balancerEntry["selector"] = QJsonArray::fromStringList(_balancerList);
    //        //                    _balancers.append(balancerEntry);
    //        //                }
    //    }
    //
    //    // Remove some empty fields.
    //    if (_rule.port.isEmpty())
    //    {
    //        ruleJsonObject.remove("port");
    //    }
    //
    //    if (_rule.network.isEmpty())
    //    {
    //        ruleJsonObject.remove("network");
    //    }
    //
    //  rulesArray.append(ruleJsonObject);
    //}

    // QJsonObject routing;
    // routing["domainStrategy"] = domainStrategy;
    // routing["rules"] = rulesArray;
    // routing["balancers"] = _balancers;
    // //
    // QJsonArray _inbounds;
    // QJsonArray _outbounds;
    //
    // // Convert our internal data format to QJsonArray
    // for (auto x : inbounds)
    // {
    //     if (x.isEmpty())
    //         continue;
    //
    //     _inbounds.append(x.raw());
    // }
    //
    // for (auto x : outbounds)
    // {
    //     if (x.isEmpty())
    //         continue;
    //
    //     if (getTag(x) == defaultOutbound)
    //     {
    //         LOG(MODULE_CONNECTION, "Pushing default outbound to the front.")
    //         // Put the default outbound to the first.
    //         _outbounds.push_front(x.raw());
    //     }
    //     else
    //     {
    //         _outbounds.push_back(x.raw());
    //     }
    // }
    //
    // root["inbounds"] = _inbounds;
    // root["outbounds"] = _outbounds;
    // root["routing"] = routing;
    //    return root;
    //}
    // else
    //{
    return original;
    //}
}

RouteEditor::~RouteEditor()
{
    nodeDispatcher.reset();
}
void RouteEditor::on_buttonBox_accepted()
{
}

void RouteEditor::on_insertDirectBtn_clicked()
{
    auto freedom = GenerateFreedomOUT("AsIs", "", 0);
    auto tag = "Freedom_" + QSTRN(QTime::currentTime().msecsSinceStartOfDay());
    auto out = GenerateOutboundEntry("freedom", freedom, {}, {}, "0.0.0.0", tag);
    // ADD NODE
    const auto _ = nodeDispatcher->CreateOutbound(make_outbound(out));
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
        auto http = GenerateHTTPIN(QList<AccountObject>() << inboundConfig.httpSettings.account);
        INBOUND httpConfig = GenerateInboundEntry(inboundConfig.listenip, inboundConfig.httpSettings.port, "http", http, "GlobalConfig-HTTP",
                                                  inboundConfig.httpSettings.sniffing ? sniffingOn : sniffingOff);
        auto _ = nodeDispatcher->CreateInbound(httpConfig);
    }
    if (inboundConfig.useSocks)
    {
        auto socks = GenerateSocksIN((inboundConfig.socksSettings.useAuth ? "password" : "noauth"), //
                                     QList<AccountObject>() << inboundConfig.socksSettings.account, //
                                     inboundConfig.socksSettings.enableUDP,                         //
                                     inboundConfig.socksSettings.localIP);
        auto socksConfig = GenerateInboundEntry(inboundConfig.listenip,           //
                                                inboundConfig.socksSettings.port, //
                                                "socks",                          //
                                                socks,                            //
                                                "GlobalConfig-Socks",             //
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
        //
        const static QJsonObject tproxy_sniff{ { "enabled", true }, { "destOverride", QJsonArray{ "http", "tls" } } };
        const QJsonObject tproxy_streamSettings{ { "sockopt", QJsonObject{ { "tproxy", ts.mode } } } };

        auto tProxyIn = GenerateInboundEntry(ts.tProxyIP, ts.port, "dokodemo-door", tproxyInSettings, "TPROXY_gConf");
        tProxyIn.insert("sniffing", tproxy_sniff);
        tProxyIn.insert("streamSettings", tproxy_streamSettings);
        auto _ = nodeDispatcher->CreateInbound(tProxyIn);

        if (!ts.tProxyV6IP.isEmpty())
        {
            auto tProxyV6In = GenerateInboundEntry(ts.tProxyV6IP, ts.port, "dokodemo-door", tproxyInSettings, "TPROXY_gConf_V6");
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
    auto outbound = GenerateOutboundEntry("blackhole", blackHole, {}, {}, "0.0.0.0", tag);
    const auto _ = nodeDispatcher->CreateOutbound(make_outbound(outbound));
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
            auto _ = nodeDispatcher->CreateOutbound(make_outbound(OUTBOUND(confList[i].toObject())));
        }
    }
}

void RouteEditor::on_delBtn_clicked()
{
    if (nodeScene->selectedNodes().empty())
    {
        QvMessageBoxWarn(this, tr("Remove Items"), tr("Please select a node from the graph to continue."));
        return;
    }

    const auto selecteNodes = nodeScene->selectedNodes();
    if (selecteNodes.empty())
    {
        QvMessageBoxWarn(this, tr("Deleting a node"), tr("You need to select a node first"));
        return;
    }
    nodeScene->removeNode(*selecteNodes.front());
}

void RouteEditor::on_addRouteBtn_clicked()
{
    const auto _ = nodeDispatcher->CreateRule({});
}

void RouteEditor::on_domainStrategyCombo_currentIndexChanged(const QString &arg1)
{
    LOADINGCHECK
    domainStrategy = arg1;
}

void RouteEditor::on_defaultOutboundCombo_currentTextChanged(const QString &arg1)
{
    LOADINGCHECK
    // defaultOutbound = arg1;
}

void RouteEditor::on_importExistingBtn_clicked()
{
    const auto connId = ConnectionId{ importConnBtn->currentData(Qt::UserRole).toString() };
    const auto root = ConnectionManager->GetConnectionRoot(connId);
    auto outbound = OUTBOUND(root["outbounds"].toArray()[0].toObject());
    outbound["tag"] = GetDisplayName(connId);
    auto _ = nodeDispatcher->CreateOutbound(make_outbound(outbound));
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
    auto _ = nodeDispatcher->CreateOutbound(make_outbound(BalancerId{ GenerateRandomString() }, "Balancer"));
}

void RouteEditor::on_addChainBtn_clicked()
{
    auto _ = nodeDispatcher->CreateOutbound(make_outbound(ChainId{ GenerateRandomString() }, "Chained Outbound"));
}

void RouteEditor::on_debugPainterCB_clicked(bool checked)
{
#ifdef QT_DEBUG
    QtNodes::ConnectionPainter::IsDebuggingEnabled = checked;
    nodeScene->update();
    flowView->repaint();
#endif
}

void RouteEditor::on_linkExistingBtn_clicked()
{
    const auto cid = ConnectionId{ importConnBtn->currentData(Qt::UserRole).toString() };
    auto _ = nodeDispatcher->CreateOutbound(make_outbound(cid, GetDisplayName(cid)));
}
