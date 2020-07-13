#include "w_RoutesEditor.hpp"

#include "components/plugins/QvPluginHost.hpp"
#include "core/CoreUtils.hpp"
#include "core/connection/ConnectionIO.hpp"
#include "core/connection/Generation.hpp"
#include "core/handler/ConfigHandler.hpp"
#include "ui/common/UIBase.hpp"
#include "ui/models/InboundNodeModel.hpp"
#include "ui/models/OutboundNodeModel.hpp"
#include "ui/models/RuleNodeModel.hpp"
#include "ui/windows/w_ImportConfig.hpp"
#include "w_InboundEditor.hpp"
#include "w_JsonEditor.hpp"
#include "w_OutboundEditor.hpp"

#include <nodes/internal/FlowScene.hpp>
#include <nodes/internal/FlowView.hpp>
#include <nodes/internal/FlowViewStyle.hpp>
#include <nodes/internal/NodeStyle.hpp>

using QtNodes::FlowView;
using namespace Qv2ray::ui::nodemodels;

#define CurrentRule this->rules[this->currentRuleTag]
#define LOADINGCHECK                                                                                                                            \
    if (isLoading)                                                                                                                              \
        return;
#define GetFirstNodeData(_node, name)                                                                                                           \
    (static_cast<name##Data *>(static_cast<Qv##name##Model *>((nodeScene->node(_node))->nodeDataModel())->outData(0).get()))

#define CHECKEMPTYRULES                                                                                                                         \
    if (this->rules.isEmpty())                                                                                                                  \
    {                                                                                                                                           \
        LOG(MODULE_UI, "No rules currently, we add one.")                                                                                       \
        AddNewRule();                                                                                                                           \
    }

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

    QVBoxLayout *l = new QVBoxLayout(nodeGraphWidget);
    nodeScene = new FlowScene(nodeGraphWidget);
    connect(nodeScene, &FlowScene::nodeClicked, this, &RouteEditor::onNodeClicked);
    connect(nodeScene, &FlowScene::connectionCreated, this, &RouteEditor::onConnectionCreated);
    connect(nodeScene, &FlowScene::connectionDeleted, this, &RouteEditor::onConnectionDeleted);
    auto view = new FlowView(nodeScene);
    view->scaleDown();
    l->addWidget(view);
    l->setContentsMargins(0, 0, 0, 0);
    l->setSpacing(0);
}

RouteEditor::RouteEditor(QJsonObject connection, QWidget *parent) : QvDialog(parent), root(connection), original(connection)
{
    QvMessageBusConnect(RouteEditor);
    setupUi(this);
    isLoading = true;
    setWindowFlags(windowFlags() | Qt::WindowMaximizeButtonHint);
    //
    SetupNodeWidget();
    //
    // Setup icons according to the theme settings.
    addInboundBtn->setIcon(QICON_R("add.png"));
    addOutboundBtn->setIcon(QICON_R("add.png"));
    editBtn->setIcon(QICON_R("edit.png"));
    addRouteBtn->setIcon(QICON_R("add.png"));
    delBtn->setIcon(QICON_R("delete.png"));
    balancerAddBtn->setIcon(QICON_R("add.png"));
    balancerDelBtn->setIcon(QICON_R("delete.png"));
    //
    domainStrategy = root["routing"].toObject()["domainStrategy"].toString();
    domainStrategyCombo->setCurrentText(domainStrategy);

    // Show connections in the node graph
    for (const auto &in : root["inbounds"].toArray())
    {
        AddInbound(INBOUND(in.toObject()));
    }

    for (const auto &out : root["outbounds"].toArray())
    {
        AddOutbound(OUTBOUND(out.toObject()));
    }

    for (const auto &item : root["routing"].toObject()["rules"].toArray())
    {
        AddRule(RuleObject::fromJson(item.toObject()));
    }

    // Set default outboung combo text AFTER adding all outbounds.
    defaultOutbound = getTag(OUTBOUND(root["outbounds"].toArray().first().toObject()));
    defaultOutboundCombo->setCurrentText(defaultOutbound);

    // Find and add balancers.
    for (auto _balancer : root["routing"].toObject()["balancers"].toArray())
    {
        auto _balancerObject = _balancer.toObject();
        if (!_balancerObject["tag"].toString().isEmpty())
        {
            balancers.insert(_balancerObject["tag"].toString(), _balancerObject["selector"].toVariant().toStringList());
        }
    }

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

void RouteEditor::onNodeClicked(Node &n)
{
    LOADINGCHECK

    if (isExiting)
        return;

    auto isOut = outboundNodes.values().contains(n.id());
    auto isIn = inboundNodes.values().contains(n.id());
    auto isRule = ruleNodes.values().contains(n.id());

    if (isRule)
    {
        // It's a rule object
        currentRuleTag = GetFirstNodeData(n.id(), RuleNode)->GetRuleTag();
        DEBUG(MODULE_GRAPH, "Selecting rule: " + currentRuleTag)
        ShowCurrentRuleDetail();
        toolBox->setCurrentIndex(1);
    }
    else if (isOut || isIn)
    {
        // It's an inbound or an outbound.
        QString alias;
        QString host;
        QString port;
        QString protocol;

        if (isOut)
        {
            alias = GetFirstNodeData(n.id(), OutboundNode)->GetOutbound();
            QJsonObject _root = outbounds[alias].raw();
            int _port;
            GetOutboundInfo(OUTBOUND(_root), &host, &_port, &protocol);
            port = QString::number(_port);
        }
        else
        {
            alias = GetFirstNodeData(n.id(), InboundNode)->GetInbound();
            QJsonObject _root = inbounds[alias].raw();
            host = _root["listen"].toString();
            protocol = _root["protocol"].toString();
            // Port could be a string, or an integer.
            port = _root["port"].toVariant().toString();
        }

        tagLabel->setText(alias);
        protocolLabel->setText(protocol);
        portLabel->setText(port);
        hostLabel->setText(host);
    }
    else
    {
        LOG(MODULE_GRAPH, "Selected an unknown node, RARE.")
    }
}

void RouteEditor::onConnectionCreated(QtNodes::Connection const &c)
{
    LOADINGCHECK

    if (isExiting)
        return;

    // Connection Established
    auto const &sourceNode = c.getNode(PortType::Out);
    auto const &targetNode = c.getNode(PortType::In);

    if (inboundNodes.values().contains(sourceNode->id()) && ruleNodes.values().contains(targetNode->id()))
    {
        // It's a inbound-rule connection
        onNodeClicked(*sourceNode);
        onNodeClicked(*targetNode);
        LOG(MODULE_GRAPH, "Inbound-rule new connection.")
        // Get all connected inbounds to this rule node.
        // QStringList has an helper to let us remove duplicates, see below.
        QSet<QString> _inbounds;
        //
        // Workaround for removing a connection within the loop.
        QList<std::shared_ptr<QtNodes::Connection>> connectionsTobeRemoved;
        for (auto &&[_, conn] : nodeScene->connections())
        {
            const auto &inNode = conn->getNode(PortType::In);
            const auto &outNode = conn->getNode(PortType::Out);
            // If a connection is not current Id, but with same IN/OUT nodes.
            // It is a "duplicated" connection.
            if (inNode->id() == targetNode->id() && outNode->id() == sourceNode->id() && conn->id() != c.id())
            {
                connectionsTobeRemoved << (conn);
            }
            // Append all inbounds
            if (inNode->id() == targetNode->id())
            {
                _inbounds.insert(GetFirstNodeData(outNode->id(), InboundNode)->GetInbound());
            }
        }
        for (const auto &connRemoved : connectionsTobeRemoved)
        {
            nodeScene->deleteConnection(*connRemoved);
        }

        CurrentRule.inboundTag = _inbounds.values();
    }
    else if (ruleNodes.values().contains(sourceNode->id()) && outboundNodes.values().contains(targetNode->id()))
    {
        // It's a rule-outbound connection
        onNodeClicked(*sourceNode);
        onNodeClicked(*targetNode);
        CurrentRule.outboundTag = GetFirstNodeData(targetNode->id(), OutboundNode)->GetOutbound();
        // Connecting to an outbound will disable the balancer feature.
        CurrentRule.QV2RAY_RULE_USE_BALANCER = false;
        // Update balancer settings.
        ShowCurrentRuleDetail();
        LOG(MODULE_GRAPH, "Updated outbound: " + CurrentRule.outboundTag)
    }
    else
    {
        // It's an impossible connection
        LOG(MODULE_GRAPH, "Unrecognized connection, RARE.")
    }
}

void RouteEditor::onConnectionDeleted(QtNodes::Connection const &c)
{
    LOADINGCHECK

    if (isExiting)
        return;

    // Connection Deleted
    const auto &source = c.getNode(PortType::Out);
    const auto &target = c.getNode(PortType::In);

    if (inboundNodes.values().contains(source->id()) && ruleNodes.values().contains(target->id()))
    {
        // It's a inbound-rule connection
        onNodeClicked(*source);
        onNodeClicked(*target);
        currentRuleTag = GetFirstNodeData(target->id(), RuleNode)->GetRuleTag();
        auto _inboundTag = GetFirstNodeData(source->id(), InboundNode)->GetInbound();
        LOG(MODULE_UI, "Removing inbound: " + _inboundTag + " from rule: " + currentRuleTag)
        CurrentRule.inboundTag.removeAll(_inboundTag);
    }
    else if (ruleNodes.values().contains(source->id()) && outboundNodes.values().contains(target->id()))
    {
        // It's a rule-outbound connection
        onNodeClicked(*source);
        onNodeClicked(*target);
        currentRuleTag = GetFirstNodeData(source->id(), RuleNode)->GetRuleTag();
        auto _outboundTag = GetFirstNodeData(target->id(), OutboundNode)->GetOutbound();

        if (!CurrentRule.QV2RAY_RULE_USE_BALANCER && CurrentRule.outboundTag == _outboundTag)
        {
            CurrentRule.outboundTag.clear();
        }

        LOG(MODULE_GRAPH, "Removing an outbound: " + _outboundTag)
    }
    else
    {
        // It's an impossible connection
        LOG(MODULE_GRAPH, "Selected an unknown node, RARE.")
    }
}

CONFIGROOT RouteEditor::OpenEditor()
{
    auto result = this->exec();

    if (rules.isEmpty())
    {
        // Prevent empty rule list causing mis-detection of config type to
        // simple.
        on_addRouteBtn_clicked();
    }

    // If clicking OK
    if (result == QDialog::Accepted)
    {
        QJsonArray rulesArray;
        QJsonArray _balancers;

        // Append rules by order
        for (auto i = 0; i < ruleListWidget->count(); i++)
        {
            auto _rule = rules[ruleListWidget->item(i)->text()];
            auto ruleJsonObject = _rule.toJson();

            // Process balancer for a rule
            if (_rule.QV2RAY_RULE_USE_BALANCER)
            {
                // Do not use outbound tag.
                ruleJsonObject.remove("outboundTag");

                // Find balancer list
                if (!balancers.contains(_rule.balancerTag))
                {
                    LOG(MODULE_UI, "Cannot find a balancer for tag: " + _rule.balancerTag)
                }
                else
                {
                    auto _balancerList = balancers[_rule.balancerTag];
                    QJsonObject balancerEntry;
                    balancerEntry["tag"] = _rule.balancerTag;
                    balancerEntry["selector"] = QJsonArray::fromStringList(_balancerList);
                    _balancers.append(balancerEntry);
                }
            }

            // Remove some empty fields.
            if (_rule.port.isEmpty())
            {
                ruleJsonObject.remove("port");
            }

            if (_rule.network.isEmpty())
            {
                ruleJsonObject.remove("network");
            }

            rulesArray.append(ruleJsonObject);
        }

        QJsonObject routing;
        routing["domainStrategy"] = domainStrategy;
        routing["rules"] = rulesArray;
        routing["balancers"] = _balancers;
        //
        QJsonArray _inbounds;
        QJsonArray _outbounds;

        // Convert our internal data format to QJsonArray
        for (auto x : inbounds)
        {
            if (x.isEmpty())
                continue;

            _inbounds.append(x.raw());
        }

        for (auto x : outbounds)
        {
            if (x.isEmpty())
                continue;

            if (getTag(x) == defaultOutbound)
            {
                LOG(MODULE_CONNECTION, "Pushing default outbound to the front.")
                // Put the default outbound to the first.
                _outbounds.push_front(x.raw());
            }
            else
            {
                _outbounds.push_back(x.raw());
            }
        }

        root["inbounds"] = _inbounds;
        root["outbounds"] = _outbounds;
        root["routing"] = routing;
        return root;
    }
    else
    {
        return original;
    }
}

RouteEditor::~RouteEditor()
{
    // Double prevent events to be processed while closing the Editor.
    isLoading = true;
    disconnect(nodeScene, &FlowScene::connectionDeleted, this, &RouteEditor::onConnectionDeleted);
    disconnect(nodeScene, &FlowScene::connectionCreated, this, &RouteEditor::onConnectionCreated);
    disconnect(nodeScene, &FlowScene::nodeClicked, this, &RouteEditor::onNodeClicked);
}
void RouteEditor::on_buttonBox_accepted()
{
}

void RouteEditor::ShowCurrentRuleDetail()
{
    LOADINGCHECK

    if (currentRuleTag.isEmpty())
    {
        LOG(MODULE_UI, "WARNING, trying to access a non-exist rule entry. return.")
        return;
    }

    if (!rules.contains(currentRuleTag))
    {
        QvMessageBoxWarn(this, tr("Show rule details"), tr("A rule cannot be found: ") + currentRuleTag);
        LOG(MODULE_UI, "WARNING, trying to access a non-exist rule entry. return.")
        return;
    }

    // Switch to the detailed page.
    ruleEnableCB->setEnabled(true);
    ruleEnableCB->setChecked(CurrentRule.QV2RAY_RULE_ENABLED);
    routeEditGroupBox->setEnabled(true);
    ruleTagLineEdit->setEnabled(true);
    ruleRenameBtn->setEnabled(true);
    routeRuleGroupBox->setEnabled(true);
    LOAD_FLAG_BEGIN
    ruleTagLineEdit->setText(CurrentRule.QV2RAY_RULE_TAG);
    balancerSelectionCombo->clear();

    for (auto out : outbounds)
    {
        balancerSelectionCombo->addItem(getTag(OUTBOUND(out)));
    }

    //
    // Balancers combo and balancer list.
    enableBalancerCB->setChecked(CurrentRule.QV2RAY_RULE_USE_BALANCER);
    balancersWidget->setEnabled(CurrentRule.QV2RAY_RULE_USE_BALANCER);

    if (!CurrentRule.balancerTag.isEmpty())
    {
        balancerList->clear();
        balancerList->addItems(balancers[CurrentRule.balancerTag]);
    }

    isLoading = false;
    // Networks
    auto network = CurrentRule.network.toLower();
    bool isBoth = (network.contains("tcp") && network.contains("udp")) || network.isEmpty();
    netUDPRB->setChecked(network.contains("udp"));
    netTCPRB->setChecked(network.contains("tcp"));
    netBothRB->setChecked(isBoth);
    //
    // Set protocol checkboxes.
    auto protocol = CurrentRule.protocol;
    routeProtocolHTTPCB->setChecked(protocol.contains("http"));
    routeProtocolTLSCB->setChecked(protocol.contains("tls"));
    routeProtocolBTCB->setChecked(protocol.contains("bittorrent"));
    //
    // Port
    routePortTxt->setText(CurrentRule.port);
    //
    // Users
    QString users = CurrentRule.user.join(NEWLINE);
    routeUserTxt->setPlainText(users);
    //
    // Incoming Sources
    QString sources = CurrentRule.source.join(NEWLINE);
    sourceIPList->setPlainText(sources);
    //
    // Domains
    QString domains = CurrentRule.domain.join(NEWLINE);
    hostList->setPlainText(domains);
    //
    // Outcoming IPs
    QString ips = CurrentRule.ip.join(NEWLINE);
    ipList->setPlainText(ips);
    LOAD_FLAG_END
}

void RouteEditor::on_insertDirectBtn_clicked()
{
    auto freedom = GenerateFreedomOUT("AsIs", "", 0);
    auto tag = "Freedom_" + QSTRN(QTime::currentTime().msecsSinceStartOfDay());
    auto out = GenerateOutboundEntry("freedom", freedom, QJsonObject(), QJsonObject(), "0.0.0.0", tag);
    // ADD NODE
    AddOutbound(out);
    statusLabel->setText(tr("Added DIRECT outbound"));
}

void RouteEditor::on_routeProtocolHTTPCB_stateChanged(int arg1)
{
    LOADINGCHECK
    QStringList protocols;

    if (arg1 == Qt::Checked)
        protocols.push_back("http");

    if (routeProtocolTLSCB->isChecked())
        protocols.push_back("tls");

    if (routeProtocolBTCB->isChecked())
        protocols.push_back("bittorrent");

    CurrentRule.protocol = protocols;
    statusLabel->setText(tr("Protocol list changed: ") + protocols.join(";"));
}
void RouteEditor::on_routeProtocolTLSCB_stateChanged(int arg1)
{
    LOADINGCHECK
    QStringList protocols;

    if (arg1 == Qt::Checked)
        protocols.push_back("tls");

    if (routeProtocolHTTPCB->isChecked())
        protocols.push_back("http");

    if (routeProtocolBTCB->isChecked())
        protocols.push_back("bittorrent");

    CurrentRule.protocol = protocols;
    statusLabel->setText(tr("Protocol list changed: ") + protocols.join(";"));
}
void RouteEditor::on_routeProtocolBTCB_stateChanged(int arg1)
{
    LOADINGCHECK
    QStringList protocols;

    if (arg1 == Qt::Checked)
        protocols.push_back("bittorrent");

    if (routeProtocolHTTPCB->isChecked())
        protocols.push_back("http");

    if (routeProtocolTLSCB->isChecked())
        protocols.push_back("tls");

    CurrentRule.protocol = protocols;
    statusLabel->setText(tr("Protocol list changed: ") + protocols.join(";"));
}
void RouteEditor::on_balancerAddBtn_clicked()
{
    LOADINGCHECK
    auto balancerTx = balancerSelectionCombo->currentText();

    if (!balancerTx.isEmpty())
    {
        this->balancers[CurrentRule.balancerTag].append(balancerSelectionCombo->currentText());
        balancerList->addItem(balancerTx);
        balancerSelectionCombo->setEditText("");
        statusLabel->setText(tr("OK"));
    }
    else
    {
        statusLabel->setText(tr("Balancer is empty, not processing."));
    }
}
void RouteEditor::on_balancerDelBtn_clicked()
{
    LOADINGCHECK

    if (balancerList->currentRow() < 0)
    {
        return;
    }

    balancers[CurrentRule.balancerTag].removeAt(balancerList->currentRow());
    balancerList->takeItem(balancerList->currentRow());
    statusLabel->setText(tr("Removed a balancer entry."));
}
void RouteEditor::on_hostList_textChanged()
{
    LOADINGCHECK
    CurrentRule.domain = SplitLines(hostList->toPlainText());
}
void RouteEditor::on_ipList_textChanged()
{
    LOADINGCHECK
    CurrentRule.ip = SplitLines(ipList->toPlainText());
}
void RouteEditor::on_routePortTxt_textEdited(const QString &arg1)
{
    LOADINGCHECK
    CurrentRule.port = arg1;
}
void RouteEditor::on_routeUserTxt_textEdited(const QString &arg1)
{
    LOADINGCHECK
    CurrentRule.user = SplitLines(arg1);
}
void RouteEditor::on_addRouteBtn_clicked()
{
    auto ruleName = AddNewRule();
    Q_UNUSED(ruleName)
}
void RouteEditor::on_netBothRB_clicked()
{
    LOADINGCHECK
    CurrentRule.network = "tcp,udp";
}
void RouteEditor::on_netUDPRB_clicked()
{
    LOADINGCHECK
    CurrentRule.network = "udp";
}
void RouteEditor::on_netTCPRB_clicked()
{
    LOADINGCHECK
    CurrentRule.network = "tcp";
}
void RouteEditor::on_routeUserTxt_textChanged()
{
    LOADINGCHECK
    CurrentRule.user = SplitLines(routeUserTxt->toPlainText());
}
void RouteEditor::on_sourceIPList_textChanged()
{
    LOADINGCHECK
    CurrentRule.source = SplitLines(sourceIPList->toPlainText());
}
void RouteEditor::on_enableBalancerCB_stateChanged(int arg1)
{
    LOADINGCHECK
    auto useBalancer = arg1 == Qt::Checked;
    CurrentRule.QV2RAY_RULE_USE_BALANCER = useBalancer;
    balancersWidget->setEnabled(useBalancer);

    if (CurrentRule.balancerTag.isEmpty())
    {
        LOG(MODULE_UI, "Creating a new balancer tag.")
        CurrentRule.balancerTag = GenerateRandomString(6);
        balancers[CurrentRule.balancerTag] = QStringList();
    }

    DEBUG(MODULE_UI, "Balancer: " + CurrentRule.balancerTag)

    if (useBalancer)
    {
        LOG(MODULE_UI, "A rule has been set to use balancer, disconnect it to any outbound.")
        auto ruleNode = ruleNodes[currentRuleTag];
        for (auto &&[_, conn] : nodeScene->connections())
        {
            if (conn->getNode(PortType::Out)->id() == ruleNode)
            {
                nodeScene->deleteConnection(*conn);
                // Since there should be only one connection from this rule node.
                break;
            }
        }
    }
    else
    {
        QvMessageBoxWarn(this, tr("Route Editor"), tr("To make this rule ready to use, you need to connect it to an outbound node."));
    }
}
void RouteEditor::on_addDefaultBtn_clicked()
{
    LOADINGCHECK
    // Add default connection from GlobalConfig
    //
    auto _Inconfig = GlobalConfig.inboundConfig;
    QJsonObject sniffingOff{ { "enabled", false } };
    QJsonObject sniffingOn{ { "enabled", true }, { "destOverride", QJsonArray{ "http", "tls" } } };
    //
    if (_Inconfig.useHTTP)
    {
        INBOUND _in_HTTP;
        _in_HTTP.insert("listen", _Inconfig.listenip);
        _in_HTTP.insert("port", _Inconfig.httpSettings.port);
        _in_HTTP.insert("protocol", "http");
        _in_HTTP.insert("tag", "http_gConf");
        if (!_Inconfig.httpSettings.sniffing)
        {
            _in_HTTP.insert("sniffing", sniffingOff);
        }
        else
        {
            _in_HTTP.insert("sniffing", sniffingOn);
        }

        if (_Inconfig.httpSettings.useAuth)
        {
            auto httpInSettings = GenerateHTTPIN(QList<AccountObject>() << _Inconfig.httpSettings.account);
            _in_HTTP.insert("settings", httpInSettings);
        }

        AddInbound(_in_HTTP);
    }
    if (_Inconfig.useSocks)
    {
        auto _in_socksConf = GenerateSocksIN((_Inconfig.socksSettings.useAuth ? "password" : "noauth"), //
                                             QList<AccountObject>() << _Inconfig.socksSettings.account, //
                                             _Inconfig.socksSettings.enableUDP,                         //
                                             _Inconfig.socksSettings.localIP);
        auto _in_SOCKS = GenerateInboundEntry(_Inconfig.listenip, _Inconfig.socksSettings.port, "socks", _in_socksConf, "SOCKS_gConf");
        if (!_Inconfig.socksSettings.sniffing)
        {
            _in_SOCKS.insert("sniffing", sniffingOff);
        }
        else
        {
            _in_SOCKS.insert("sniffing", sniffingOn);
        }
        AddInbound(_in_SOCKS);
    }

    if (_Inconfig.useTPROXY)
    {
        QList<QString> networks;
#define _ts_ _Inconfig.tProxySettings
        if (_ts_.hasTCP)
            networks << "tcp";
        if (_ts_.hasUDP)
            networks << "udp";
        const auto tproxy_network = networks.join(",");
        auto tproxyInSettings = GenerateDokodemoIN("", 0, tproxy_network, 0, true, 0);
        //
        QJsonObject tproxy_sniff{ { "enabled", true }, { "destOverride", QJsonArray{ "http", "tls" } } };
        QJsonObject tproxy_streamSettings{ { "sockopt", QJsonObject{ { "tproxy", _ts_.mode } } } };

        auto _in_TPROXY = GenerateInboundEntry(_ts_.tProxyIP, _ts_.port, "dokodemo-door", tproxyInSettings, "TPROXY_gConf");
        _in_TPROXY.insert("sniffing", tproxy_sniff);
        _in_TPROXY.insert("streamSettings", tproxy_streamSettings);
        AddInbound(_in_TPROXY);

        if (!_ts_.tProxyV6IP.isEmpty())
        {
            auto _in_TPROXY = GenerateInboundEntry(_ts_.tProxyV6IP, _ts_.port, "dokodemo-door", tproxyInSettings, "TPROXY_gConf_V6");
            _in_TPROXY.insert("sniffing", tproxy_sniff);
            _in_TPROXY.insert("streamSettings", tproxy_streamSettings);
            AddInbound(_in_TPROXY);
        }
#undef _ts_
    }

    CHECKEMPTYRULES
}
void RouteEditor::on_insertBlackBtn_clicked()
{
    LOADINGCHECK
    auto blackHole = GenerateBlackHoleOUT(false);
    auto tag = "blackhole_" + QSTRN(QTime::currentTime().msecsSinceStartOfDay());
    auto _blackHoleOutbound = GenerateOutboundEntry("blackhole", blackHole, QJsonObject(), QJsonObject(), "0.0.0.0", tag);
    AddOutbound(_blackHoleOutbound);
}
void RouteEditor::on_addInboundBtn_clicked()
{
    LOADINGCHECK
    InboundEditor w(INBOUND(), this);
    auto _result = w.OpenEditor();

    if (w.result() == QDialog::Accepted)
    {
        AddInbound(_result);
    }

    CHECKEMPTYRULES
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
            AddOutbound(OUTBOUND(confList[i].toObject()));
        }
    }

    CHECKEMPTYRULES
}
void RouteEditor::on_ruleEnableCB_stateChanged(int arg1)
{
    bool _isEnabled = arg1 == Qt::Checked;
    CurrentRule.QV2RAY_RULE_ENABLED = _isEnabled;
    routeEditGroupBox->setEnabled(_isEnabled);
    routeRuleGroupBox->setEnabled(_isEnabled);
}
void RouteEditor::on_delBtn_clicked()
{
    if (nodeScene->selectedNodes().empty())
    {
        QvMessageBoxWarn(this, tr("Remove Items"), tr("Please select a node from the graph to continue."));
        return;
    }

    auto firstNode = nodeScene->selectedNodes()[0];
    auto isInbound = inboundNodes.values().contains(firstNode->id());
    auto isOutbound = outboundNodes.values().contains(firstNode->id());
    auto isRule = ruleNodes.values().contains(firstNode->id());

    // Get the tag first, and call inbounds/outbounds/rules container variable
    // remove() Remove the node last since some events may trigger. Then remove
    // the node container.
    if (isInbound)
    {
        currentInboundOutboundTag = GetFirstNodeData(firstNode->id(), InboundNode)->GetInbound();
        nodeScene->removeNode(*nodeScene->node(inboundNodes[currentInboundOutboundTag]));
        inboundNodes.remove(currentInboundOutboundTag);

        // Remove corresponded inbound tags from the rules.
        for (auto k : rules.keys())
        {
            auto v = rules[k];
            v.inboundTag.removeAll(currentInboundOutboundTag);
            rules[k] = v;
        }

        inbounds.remove(currentInboundOutboundTag);
    }
    else if (isOutbound)
    {
        currentInboundOutboundTag = GetFirstNodeData(firstNode->id(), OutboundNode)->GetOutbound();
        outbounds.remove(currentInboundOutboundTag);
        ResolveDefaultOutboundTag(currentInboundOutboundTag, "");

        // Remove corresponded outbound tags from the rules.
        for (auto k : rules.keys())
        {
            auto v = rules[k];

            if (v.outboundTag == currentInboundOutboundTag)
                v.outboundTag.clear();

            rules[k] = v;
        }

        nodeScene->removeNode(*nodeScene->node(outboundNodes[currentInboundOutboundTag]));
        outboundNodes.remove(currentInboundOutboundTag);
    }
    else if (isRule)
    {
        ruleEnableCB->setEnabled(false);
        ruleTagLineEdit->setEnabled(false);
        ruleRenameBtn->setEnabled(false);
        auto RuleTag = GetFirstNodeData(firstNode->id(), RuleNode)->GetRuleTag();
        currentRuleTag.clear();
        routeRuleGroupBox->setEnabled(false);
        routeEditGroupBox->setEnabled(false);
        rules.remove(RuleTag);
        nodeScene->removeNode(*nodeScene->node(ruleNodes[RuleTag]));
        ruleNodes.remove(RuleTag);
        //
        // Remove item from the rule order list widget.
        ruleListWidget->takeItem(ruleListWidget->row(ruleListWidget->findItems(RuleTag, Qt::MatchExactly).first()));
        CHECKEMPTYRULES
        // currentRuleTag = rules.firstKey();
        // ShowCurrentRuleDetail();
    }
    else
    {
        LOG(MODULE_UI, "Unknown node selected.")
        QvMessageBoxWarn(this, tr("Error"), tr("Qv2ray entered an unknown state."));
    }
}
void RouteEditor::on_editBtn_clicked()
{
    if (nodeScene->selectedNodes().empty())
    {
        QvMessageBoxWarn(this, tr("Edit Inbound/Outbound"), tr("Please select a node from the graph to continue."));
        return;
    }

    const auto firstNode = nodeScene->selectedNodes().at(0);
    const auto &isInbound = inboundNodes.values().contains(firstNode->id());
    const auto &isOutbound = outboundNodes.values().contains(firstNode->id());

    if (isInbound)
    {
        currentInboundOutboundTag = GetFirstNodeData(firstNode->id(), InboundNode)->GetInbound();

        if (!inbounds.contains(currentInboundOutboundTag))
        {
            QvMessageBoxWarn(this, tr("Edit Inbound"), tr("No inbound tag found: ") + currentInboundOutboundTag);
            return;
        }

        auto _in = inbounds[currentInboundOutboundTag];
        INBOUND _result;
        auto protocol = _in["protocol"].toString();
        int _code;

        if (protocol != "http" && protocol != "mtproto" && protocol != "socks" && protocol != "dokodemo-door")
        {
            QvMessageBoxWarn(this, tr("Cannot Edit"),
                             tr("Currently, this type of outbound is not supported by the editor.") + "\r\n" +
                                 tr("We will launch Json Editor instead."));
            statusLabel->setText(tr("Opening JSON editor"));
            JsonEditor w(_in, this);
            _result = INBOUND(w.OpenEditor());
            _code = w.result();
        }
        else
        {
            InboundEditor w(_in, this);
            statusLabel->setText(tr("Opening default inbound editor"));
            _result = w.OpenEditor();
            _code = w.result();
        }

        statusLabel->setText(tr("OK"));

        if (_code == QDialog::Accepted)
        {
            bool isTagChanged = getTag(_in) != getTag(_result);

            if (isTagChanged)
            {
                auto newTag = getTag(_result);
                RenameItemTag(RENAME_INBOUND, getTag(_in), &newTag);
            }

            DEBUG(MODULE_UI, "Removed old tag: " + getTag(_in))
            inbounds.remove(getTag(_in));
            DEBUG(MODULE_UI, "Adding new tag: " + getTag(_result))
            inbounds.insert(getTag(_result), _result);
        }
    }
    else if (isOutbound)
    {
        currentInboundOutboundTag = GetFirstNodeData(firstNode->id(), OutboundNode)->GetOutbound();

        if (!outbounds.contains(currentInboundOutboundTag))
        {
            QvMessageBoxWarn(this, tr("Edit Inbound"), tr("No inbound tag found: ") + currentInboundOutboundTag);
            return;
        }

        OUTBOUND _result;
        auto _out = outbounds.value(currentInboundOutboundTag);
        auto protocol = _out["protocol"].toString().toLower();
        int _code;

        bool guisupport = true;
        if (protocol != "vmess" && protocol != "shadowsocks" && protocol != "socks" && protocol != "http")
        {
            guisupport = false;
            auto pluginEditorWidgetsInfo = PluginHost->GetOutboundEditorWidgets();
            for (const auto &plugin : pluginEditorWidgetsInfo)
            {
                for (const auto &_d : plugin->OutboundCapabilities())
                {
                    guisupport = guisupport || protocol == _d.protocol;
                }
            }
        }

        if (!guisupport)
        {
            QvMessageBoxWarn(this, tr("Unsupported Outbound Type"),
                             tr("This outbound entry is not supported by the GUI editor.") + NEWLINE +
                                 tr("We will launch Json Editor instead."));
            JsonEditor w(_out, this);
            statusLabel->setText(tr("Opening JSON editor"));
            _result = OUTBOUND(w.OpenEditor());
            _code = w.result();
        }
        else
        {
            OutboundEditor w(_out, this);
            statusLabel->setText(tr("Opening default outbound editor."));
            _result = w.OpenEditor();
            _code = w.result();
        }

        if (_code == QDialog::Accepted)
        {
            bool isTagChanged = getTag(_out) != getTag(_result);

            if (isTagChanged)
            {
                auto newTag = getTag(_result);
                DEBUG(MODULE_UI, "Outbound tag is changed: " + newTag)
                RenameItemTag(RENAME_OUTBOUND, getTag(_out), &newTag);
            }

            DEBUG(MODULE_UI, "Adding new tag: " + getTag(_result))
            outbounds.insert(getTag(_result), _result);
            statusLabel->setText(tr("OK"));
        }
    }
    else
    {
        LOG(MODULE_UI, "Cannot apply 'edit' operation to non-inbound and non-outbound")
    }
}

void RouteEditor::on_domainStrategyCombo_currentIndexChanged(const QString &arg1)
{
    LOADINGCHECK
    domainStrategy = arg1;
}

void RouteEditor::on_ruleRenameBtn_clicked()
{
    auto newTag = ruleTagLineEdit->text();

    if (newTag.isEmpty())
    {
        LOG(MODULE_UI, "Tag is empty, this is ILLEGAL!")
        QvMessageBoxWarn(this, tr("Renaming a tag"), tr("New tag is empty, please try another."));
    }
    else if (newTag == CurrentRule.QV2RAY_RULE_TAG)
    {
        LOG(MODULE_UI, "No tag changed, returning.")
        QvMessageBoxInfo(this, tr("Renaming a tag"), tr("New tag is the same as the original one."));
    }
    else if (rules.contains(newTag))
    {
        LOG(MODULE_UI, "Tag duplicate detected.")
        QvMessageBoxWarn(this, tr("Renaming a tag"), tr("Duplicate rule tag detected, please try another."));
    }
    else
    {
        RenameItemTag(RENAME_RULE, CurrentRule.QV2RAY_RULE_TAG, &newTag);
    }
}

void RouteEditor::on_defaultOutboundCombo_currentTextChanged(const QString &arg1)
{
    LOADINGCHECK
    defaultOutbound = arg1;
}

void RouteEditor::on_importExistingBtn_clicked()
{
    const auto connId = ConnectionId{ importConnBtn->currentData(Qt::UserRole).toString() };
    const auto root = ConnectionManager->GetConnectionRoot(connId);
    auto outbound = root["outbounds"].toArray()[0].toObject();
    outbound["tag"] = GetDisplayName(connId);
    AddOutbound(OUTBOUND{ outbound });
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
