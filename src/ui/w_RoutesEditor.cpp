#include "w_RoutesEditor.hpp"
#include "QvCoreConfigOperations.hpp"
#include "w_OutboundEditor.hpp"
#include "w_JsonEditor.hpp"
#include "w_InboundEditor.hpp"
#include "w_ImportConfig.hpp"

static bool isLoading = false;
#define CurrentRule this->rules[this->currentRuleIndex]
#define LOADINGCHECK if(isLoading) return;

RouteEditor::RouteEditor(QJsonObject connection, QWidget *parent) :
    QDialog(parent),
    root(connection),
    original(connection)
{
    setupUi(this);
    isLoading = true;
    addInboundBtn->setIcon(QICON_R("add.png"));
    addOutboundBtn->setIcon(QICON_R("add.png"));
    editInboundBtn->setIcon(QICON_R("edit.png"));
    editOutboundBtn->setIcon(QICON_R("edit.png"));
    delInboundBtn->setIcon(QICON_R("delete.png"));
    delOutboundBtn->setIcon(QICON_R("delete.png"));
    addRouteBtn->setIcon(QICON_R("add.png"));
    delRouteBtn->setIcon(QICON_R("delete.png"));
    balabcerAddBtn->setIcon(QICON_R("add.png"));
    balancerDelBtn->setIcon(QICON_R("delete.png"));
    //
    inbounds = root["inbounds"].toArray();
    outbounds = root["outbounds"].toArray();
    DomainStrategy = root["routing"].toObject()["domainStrategy"].toString();

    // Applying Balancers.
    for (auto _balancer : root["routing"].toObject()["balancers"].toArray()) {
        auto __balancer = _balancer.toObject();
        Balancers[__balancer["tag"].toString()] = QList<QString>();

        for (auto _ : __balancer["selector"].toArray()) {
            Balancers[__balancer["tag"].toString()].append(_.toString());
        }
    }

    rules = QList<RuleObject>::fromStdList(StructFromJsonString<list<RuleObject>>(JsonToString(root["routing"].toObject()["rules"].toArray())));
    //
    outboundsList->clear();
    inboundsList->clear();

    for (auto out : outbounds) {
        bool hasTag = out.toObject().contains("tag");
        //
        auto protocol = out.toObject()["protocol"].toString();
        auto tag = hasTag ? out.toObject()["tag"].toString() : tr("No Tag");
        //
        outboundsList->addItem(tag + " (" + protocol + ")");
    }

    for (auto in : inbounds) {
        bool hasTag = in.toObject().contains("tag");
        //
        auto tag = hasTag ? in.toObject()["tag"].toString() : tr("No Tag");
        auto protocol = in.toObject()["protocol"].toString();
        auto port = in.toObject()["port"].toVariant().toString();
        //
        auto inItem = new QListWidgetItem(tag + " (" + protocol + ": " + port  + ")");
        inItem->setCheckState(Qt::Unchecked);
        inboundsList->addItem(inItem);
    }

    routesTable->clearContents();

    for (int i = 0; i < rules.size(); i++) {
#define rule rules[i]
        // Set up balancers.

        if (QSTRING(rule.balancerTag).isEmpty()) {
            // Default balancer tag.
            auto bTag = GenerateRandomString();
            rule.balancerTag = bTag.toStdString();
            Balancers[bTag] = QStringList();
        }

        //
        routesTable->insertRow(routesTable->rowCount());
        //
        // WARNING There should be an additional check on the final configuration generation process.
        auto enabledItem = new QTableWidgetItem(tr("Enabled"));
        enabledItem->setCheckState(rule.QV2RAY_RULE_ENABLED ? Qt::Checked : Qt::Unchecked);
        routesTable->setItem(routesTable->rowCount() - 1, 0, enabledItem);
        //
        routesTable->setItem(routesTable->rowCount() - 1, 1, new QTableWidgetItem(rule.inboundTag.size() > 0 ? Stringify(rule.inboundTag) : tr("Any")));
        routesTable->setItem(routesTable->rowCount() - 1, 2, new QTableWidgetItem(QString::number(rule.domain.size() + rule.ip.size()) + " " + tr("Items")));
        routesTable->setItem(routesTable->rowCount() - 1, 3, new QTableWidgetItem(QSTRING(rule.outboundTag)));
#undef rule
    }

    isLoading = false;

    if (rules.size() > 0) {
        routesTable->setCurrentItem(routesTable->item(0, 0));
        currentRuleIndex = 0;
        ShowRuleDetail(CurrentRule);
    } else {
        delRouteBtn->setEnabled(false);

        if (inboundsList->count() > 0) inboundsList->setCurrentRow(0);

        if (outboundsList->count() > 0) outboundsList->setCurrentRow(0);
    }
}

QJsonObject RouteEditor::OpenEditor()
{
    auto result = this->exec();

    if (result == QDialog::Accepted) {
        QJsonArray balancers;

        for (auto item : Balancers) {
            QJsonObject balancerEntry;
            auto key = Balancers.key(item);

            if (!key.isEmpty()) {
                DEBUG(MODULE_UI, "Processing balancer: "  + key.toStdString())

                if (item.size() == 0) {
                    item.append("QV2RAY_EMPTY_SELECTORS");
                    LOG(MODULE_CONFIG, "Adding a default selector list to a balancer with empty selectors.")
                }

                balancerEntry["tag"] = key;
                balancerEntry["selector"] = QJsonArray::fromStringList(item);
                balancers.append(balancerEntry);
            } else {
                LOG(MODULE_UI, "Balancer with an empty key detected! Ignored.")
            }
        }

        //
        QJsonArray rulesArray;

        for (auto _rule : rules) {
            rulesArray.append(GetRootObject(_rule));
        }

        QJsonObject routing;
        routing["domainStrategy"] = DomainStrategy;
        routing["rules"] = rulesArray;
        routing["balancers"] = balancers;
        //
        root["inbounds"] = inbounds;
        root["outbounds"] = outbounds;
        root["routing"] = routing;
        return root;
    } else {
        return original;
    }
}

RouteEditor::~RouteEditor()
{
}

void RouteEditor::on_buttonBox_accepted()
{
}

void RouteEditor::on_outboundsList_currentRowChanged(int currentRow)
{
    LOADINGCHECK
    auto outBoundRoot = outbounds[currentRow].toObject();
    auto outboundType = outBoundRoot["protocol"].toString();
    outboundTagLabel->setText(outBoundRoot.contains("tag") ? outBoundRoot["tag"].toString() : tr("No Tag"));
    //
    outboundTypeLabel->setText(outboundType);
    string serverAddress = "N/A";
    string serverPort = "N/A";

    if (outboundType == "vmess") {
        auto x = StructFromJsonString<VMessServerObject>(JsonToString(outBoundRoot["settings"].toObject()["vnext"].toArray().first().toObject()));
        serverAddress = x.address;
        serverPort = to_string(x.port);
    } else if (outboundType == "shadowsocks") {
        auto x = JsonToString(outBoundRoot["settings"].toObject()["servers"].toArray().first().toObject());
        auto Server = StructFromJsonString<ShadowSocksServerObject>(x);
        serverAddress = Server.address;
        serverPort = to_string(Server.port);
    } else if (outboundType == "socks") {
        auto x = JsonToString(outBoundRoot["settings"].toObject()["servers"].toArray().first().toObject());
        auto Server = StructFromJsonString<SocksServerObject>(x);
        serverAddress = Server.address;
        serverPort = to_string(Server.port);
    }

    outboundAddressLabel->setText(QSTRING(serverAddress));
    outboundPortLabel->setText(QSTRING(serverPort));
}

void RouteEditor::on_inboundsList_currentRowChanged(int currentRow)
{
    LOADINGCHECK
    auto inBoundRoot = inbounds[currentRow].toObject();
    //
    inboundTagLabel->setText(inBoundRoot.contains("tag") ? inBoundRoot["tag"].toString() : tr("No Tag"));
    inboundTypeLabel->setText(inBoundRoot["protocol"].toString());
    inboundAddressLabel->setText(inBoundRoot["listen"].toString());
    inboundPortLabel->setText(inBoundRoot["port"].toVariant().toString());
}

void RouteEditor::ShowRuleDetail(RuleObject rule)
{
    LOADINGCHECK
    balancerSelectionCombo->clear();
    routeOutboundSelector->clear();

    // BUG added the wrong items, should be outbound list.
    for (auto out : outbounds) {
        routeOutboundSelector->addItem(out.toObject()["tag"].toString());
        balancerSelectionCombo->addItem(out.toObject()["tag"].toString());
    }

    //
    // Balancers combo and balancer list.
    enableBalancerCB->setChecked(rule.QV2RAY_RULE_USE_BALANCER);

    if (!QSTRING(rule.balancerTag).isEmpty()) {
        balancerList->clear();
        balancerList->addItems(Balancers[QSTRING(CurrentRule.balancerTag)]);
    }

    if (!QSTRING(rule.outboundTag).isEmpty()) {
        // Find outbound index by tag.
        auto tag = QSTRING(rule.outboundTag);
        auto index = FindIndexByTag(outbounds, &tag);
        routeOutboundSelector->setCurrentIndex(index);
        //
        // Default balancer tag.
        // Don't add anything, we just show the configuration.
        //auto bTag = GenerateRandomString();
        //rule.balancerTag = bTag.toStdString();
        //Balancers[bTag] = QStringList();
        //LOG(MODULE_UI, "Adding a balancer with tag: " + bTag.toStdString())
    }

    //
    // Process inbound list
    isLoading = true;

    for (int i = 0; i < inboundsList->count(); ++i) {
        inboundsList->item(i)->setCheckState(Qt::Unchecked);
    }

    isLoading = false;
    on_inboundsList_itemChanged(nullptr);
    //
    auto outboundTag = QSTRING(rule.outboundTag);
    int index = FindIndexByTag(outbounds, &outboundTag);
    outboundsList->setCurrentRow(index);
    //
    // Networks
    auto network = QSTRING(rule.network).toLower();
    netUDPRB->setChecked(network.contains("udp"));
    netTCPRB->setChecked(network.contains("tcp"));
    netBothRB->setChecked(network.contains("tcp") && network.contains("udp"));
    //
    // Set protocol checkboxes.
    auto protocols = QList<string>::fromStdList(CurrentRule.protocol);
    routeProtocolHTTPCB->setChecked(protocols.contains("http"));
    routeProtocolTLSCB->setChecked(protocols.contains("tls"));
    routeProtocolBTCB->setChecked(protocols.contains("bittorrent"));
    //
    // Port
    routePortTxt->setText(QSTRING(CurrentRule.port));
    //
    // Users
    QString users = Stringify(CurrentRule.user, NEWLINE);
    routeUserTxt->setPlainText(users);
    //
    // Incoming Sources
    QString sources = Stringify(CurrentRule.source, NEWLINE);
    sourceIPList->setPlainText(sources);
    //
    // Domains
    QString domains = Stringify(CurrentRule.domain, NEWLINE);
    hostList->setPlainText(domains);
    //
    // Outcoming IPs
    QString ips = Stringify(CurrentRule.ip, NEWLINE);
    ipList->setPlainText(ips);

    //
    // Inbound Tags
    if (rule.inboundTag.size() == 0) {
        isLoading = true;

        for (int i = 0; i < inboundsList->count(); i++) {
            inboundsList->item(i)->setCheckState(Qt::Checked);
        }

        inboundsList->setCurrentRow(0);
        isLoading = false;
        on_inboundsList_itemChanged(inboundsList->item(0));
    } else {
        for (auto inboundTag : rule.inboundTag) {
            auto inTag = QSTRING(inboundTag);

            if (!inTag.isEmpty()) {
                // forget about the "" issue.
                int _index = FindIndexByTag(inbounds, &inTag);

                // FIXED if an inbound is missing (index out of range)
                if (_index >= 0) {
                    if (inboundsList->count() <= _index) {
                        QvMessageBox(this, tr("Route Editor"), tr("Cannot find an inbound by tag: ") + tr("Index Out Of Range"));
                        LOG(MODULE_UI, "FATAL: An inbound could not be found.")
                        return;
                    }

                    inboundsList->item(_index)->setCheckState(Qt::Checked);
                    inboundsList->setCurrentRow(_index);
                    statusLabel->setText(tr("OK"));
                } else {
                    statusLabel->setText(tr("Cannot find inbound by a tag, possible currupted files?"));
                    LOG(MODULE_UI, "An inbound could not be determined by tag.")
                    return;
                }
            }
        }
    }
}

void RouteEditor::on_editOutboundBtn_clicked()
{
    QJsonObject result;
    int row = outboundsList->currentRow();

    if (row < 0) {
        statusLabel->setText(tr("No row selected."));
        LOG(MODULE_UI, "No outbound row selected.")
        return;
    }

    auto currentOutbound = outbounds[row].toObject();
    auto protocol =  currentOutbound["protocol"].toString();

    if (protocol != "vmess" && protocol != "shadowsocks" && protocol != "socks") {
        QvMessageBox(this, tr("Cannot Edit"),
                     tr("This outbound entry is not supported by the GUI editor.") + "\r\n" +
                     tr("We will launch Json Editor instead."));
        JsonEditor *w = new JsonEditor(currentOutbound, this);
        statusLabel->setText(tr("Opening JSON editor"));
        result = w->OpenEditor();
        delete w;
    } else {
        OutboundEditor *w = new OutboundEditor(currentOutbound, this);
        statusLabel->setText(tr("Opening default outbound editor."));
        result = w->OpenEditor();
        delete w;
    }

    outbounds[row] = result;
    on_outboundsList_currentRowChanged(row);
    statusLabel->setText(tr("OK"));
}

void RouteEditor::on_insertDirectBtn_clicked()
{
    auto freedom = GenerateFreedomOUT("as-is", "", 0);
    auto tag = "Freedom_" + QString::number(QTime::currentTime().msecsSinceStartOfDay());
    auto out = GenerateOutboundEntry("freedom", freedom, QJsonObject(), QJsonObject(), "0.0.0.0", tag);
    this->outbounds.append(out);
    outboundsList->addItem(tag);
    statusLabel->setText(tr("Added DIRECT outbound"));
}

void RouteEditor::on_editInboundBtn_clicked()
{
    QJsonObject result;
    int row = inboundsList->currentRow();

    if (row < 0) return;

    auto currentInbound = inbounds[row].toObject();
    auto protocol =  currentInbound["protocol"].toString();

    if (protocol != "http" && protocol != "mtproto" && protocol != "socks" && protocol != "dokodemo-door") {
        QvMessageBox(this, tr("Cannot Edit"), tr("Currently, this type of outbound is not supported by the editor.") + "\r\n" +
                     tr("We will launch Json Editor instead."));
        statusLabel->setText(tr("Opening JSON editor"));
        JsonEditor *w = new JsonEditor(currentInbound, this);
        result = w->OpenEditor();
        delete w;
    } else {
        InboundEditor *w = new InboundEditor(currentInbound, this);
        statusLabel->setText(tr("Opening default inbound editor"));
        result = w->OpenEditor();
        delete w;
    }

    inbounds[row] = result;
    on_inboundsList_currentRowChanged(row);
    statusLabel->setText(tr("OK"));
}

void RouteEditor::on_routeProtocolHTTPCB_stateChanged(int arg1)
{
    LOADINGCHECK
    QList<string> protocols;

    if (arg1 == Qt::Checked) protocols << "http";

    if (routeProtocolTLSCB->isChecked()) protocols << "tls";

    if (routeProtocolBTCB->isChecked()) protocols << "bittorrent";

    CurrentRule.protocol = protocols.toStdList();
    statusLabel->setText(tr("Protocol list changed."));
}

void RouteEditor::on_routeProtocolTLSCB_stateChanged(int arg1)
{
    LOADINGCHECK
    QList<string> protocols;

    if (arg1 == Qt::Checked) protocols << "tls";

    if (routeProtocolHTTPCB->isChecked()) protocols << "http";

    if (routeProtocolBTCB->isChecked()) protocols << "bittorrent";

    CurrentRule.protocol = protocols.toStdList();
    statusLabel->setText(tr("Protocol list changed."));
}

void RouteEditor::on_routeProtocolBTCB_stateChanged(int arg1)
{
    LOADINGCHECK
    QList<string> protocols;

    if (arg1 == Qt::Checked) protocols << "bittorrent";

    if (routeProtocolHTTPCB->isChecked()) protocols << "http";

    if (routeProtocolTLSCB->isChecked()) protocols << "tls";

    CurrentRule.protocol = protocols.toStdList();
    statusLabel->setText(tr("Protocol list changed."));
}

void RouteEditor::on_balabcerAddBtn_clicked()
{
    LOADINGCHECK

    if (!balancerSelectionCombo->currentText().isEmpty()) {
        this->Balancers[QSTRING(CurrentRule.balancerTag)].append(balancerSelectionCombo->currentText());
    }

    auto balancerTx = balancerSelectionCombo->currentText();

    if (!balancerTx.isEmpty()) {
        balancerList->addItem(balancerTx);
        balancerSelectionCombo->setEditText("");
        statusLabel->setText(tr("OK"));
    } else {
        statusLabel->setText(tr("Balacer is empty, not processing."));
    }
}

void RouteEditor::on_balancerDelBtn_clicked()
{
    LOADINGCHECK

    if (balancerList->currentRow() < 0) {
        return;
    }

    Balancers[QSTRING(CurrentRule.balancerTag)].removeAt(balancerList->currentRow());
    balancerList->takeItem(balancerList->currentRow());
    statusLabel->setText(tr("Removed a balancer entry."));
}

void RouteEditor::on_hostList_textChanged()
{
    LOADINGCHECK
    CurrentRule.domain = SplitLinesStdString(hostList->toPlainText()).toStdList();
}

void RouteEditor::on_ipList_textChanged()
{
    LOADINGCHECK
    CurrentRule.ip = SplitLinesStdString(ipList->toPlainText()).toStdList();
}

void RouteEditor::on_routePortTxt_textEdited(const QString &arg1)
{
    LOADINGCHECK
    CurrentRule.port = arg1.toStdString();
}

void RouteEditor::on_routeUserTxt_textEdited(const QString &arg1)
{
    LOADINGCHECK
    CurrentRule.user = SplitLinesStdString(arg1).toStdList();
}

void RouteEditor::on_routesTable_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn)
{
    LOADINGCHECK
    Q_UNUSED(currentColumn)
    Q_UNUSED(previousColumn)
    Q_UNUSED(previousRow)

    if (currentRow < 0 || currentRow >= rules.size()) {
        DEBUG(MODULE_UI, "Out of range: " + to_string(currentRow))
        return;
    }

    currentRuleIndex = currentRow;
    ShowRuleDetail(CurrentRule);
}

void RouteEditor::on_addRouteBtn_clicked()
{
    LOADINGCHECK
    // Add Route
    RuleObject rule;
    //
    rule.QV2RAY_RULE_ENABLED = true;
    rule.QV2RAY_RULE_USE_BALANCER = false;
    // Default balancer tag.
    auto bTag = GenerateRandomString();
    rule.balancerTag = bTag.toStdString();
    Balancers[bTag] = QStringList();
    //
    routesTable->insertRow(routesTable->rowCount());
    // There will be an additional check on the final configuration generation process.
    auto enabledItem = new QTableWidgetItem(tr("Enabled"));
    enabledItem->setCheckState(rule.QV2RAY_RULE_ENABLED ? Qt::Checked : Qt::Unchecked);
    //
    routesTable->setItem(routesTable->rowCount() - 1, 0, enabledItem);
    routesTable->setItem(routesTable->rowCount() - 1, 1, new QTableWidgetItem(rule.inboundTag.size() > 0 ? Stringify(rule.inboundTag) : tr("Any")));
    routesTable->setItem(routesTable->rowCount() - 1, 2, new QTableWidgetItem(QString::number(rule.domain.size() + rule.ip.size()) + " " + tr("Items")));
    routesTable->setItem(routesTable->rowCount() - 1, 3, new QTableWidgetItem(QSTRING(rule.outboundTag)));
    rules.append(rule);
    //
    currentRuleIndex = routesTable->rowCount() - 1;
    routesTable->setCurrentCell(currentRuleIndex, 0);
    ShowRuleDetail(CurrentRule);
    delRouteBtn->setEnabled(true);
}

void RouteEditor::on_routesTable_cellChanged(int row, int column)
{
    LOADINGCHECK

    if (column != 0) {
        // Impossible
        return;
    }

    if (row < 0) {
        return;
    }

    if (rules.size() <= row) {
        LOG(MODULE_UI, "INFO: This message is possibly caused by adding a new route.")
        LOG(MODULE_UI, "INFO: ... But may indicate to other bugs if you didn't do that.")
        return;
    }

    rules[row].QV2RAY_RULE_ENABLED = routesTable->item(row, column)->checkState() == Qt::Checked;
    statusLabel->setText(tr((rules[row].QV2RAY_RULE_ENABLED ? "Enabled a route" : "Disabled a route")));
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
    CurrentRule.user = SplitLinesStdString(routeUserTxt->toPlainText()).toStdList();
}

void RouteEditor::on_sourceIPList_textChanged()
{
    LOADINGCHECK
    CurrentRule.source = SplitLinesStdString(sourceIPList->toPlainText()).toStdList();
}

void RouteEditor::on_enableBalancerCB_stateChanged(int arg1)
{
    LOADINGCHECK
    CurrentRule.QV2RAY_RULE_USE_BALANCER = arg1 == Qt::Checked;
    stackedWidget->setCurrentIndex(arg1 == Qt::Checked ? 1 : 0);
}


void RouteEditor::on_routeOutboundSelector_currentIndexChanged(int index)
{
    LOADINGCHECK
    CurrentRule.outboundTag = outbounds[index].toObject()["tag"].toString().toStdString();
}

void RouteEditor::on_inboundsList_itemChanged(QListWidgetItem *item)
{
    LOADINGCHECK
    Q_UNUSED(item)
    QList<string> new_inbounds;

    for (int i = 0; i < inboundsList->count(); i++) {
        auto _item = inboundsList->item(i);

        if (_item->checkState() == Qt::Checked) {
            // WARN there are possiblilties that someone may forget to set the tag.
            new_inbounds.append(inbounds[i].toObject()["tag"].toString().toStdString());
        }
    }

    if (new_inbounds.size() == 0) {
        // TODO what to do?
        LOG(MODULE_UI, "WARN: Inbound size = 0")
    }

    if (new_inbounds.contains("")) {
        // Empty tag.
        auto result1 = QvMessageBoxAsk(this, tr("Changing route inbound/outbound"), tr("One or more inbound config(s) have no tag configured, which will be ignored, do you still want to continue?"));
        new_inbounds.removeAll("");

        if (result1 != QMessageBox::Yes) {
            return;
        }
    }

    CurrentRule.inboundTag = new_inbounds.toStdList();
    statusLabel->setText(tr("OK"));
}

void RouteEditor::on_delRouteBtn_clicked()
{
    LOADINGCHECK

    if (routesTable->currentRow() >= 0) {
        auto index = routesTable->currentRow();
        auto rule = rules[index];
        rules.removeAt(index);
        Q_UNUSED(rule)
        routesTable->removeRow(index);

        // Show current row.;
        if (routesTable->rowCount() > 0) {
            currentRuleIndex = 0;
            routesTable->setCurrentCell(currentRuleIndex, 0);
            ShowRuleDetail(CurrentRule);
        }
    }
}

void RouteEditor::on_addDefaultBtn_clicked()
{
    LOADINGCHECK
    // Add default connection from GlobalConfig
    auto conf = GetGlobalConfig();
    //
    auto _in = conf.inboundConfig;
    //
    auto _in_httpConf = GenerateHTTPIN(QList<AccountObject>() << _in.httpAccount);
    auto _in_socksConf = GenerateSocksIN((_in.socks_useAuth ? "password" : "noauth"),
                                         QList<AccountObject>() << _in.socksAccount,
                                         _in.socksUDP, QSTRING(_in.socksLocalIP));
    //
    auto _in_HTTP = GenerateInboundEntry(QSTRING(_in.listenip), _in.http_port, "http", _in_httpConf, "HTTP_gConf");
    auto _in_SOCKS = GenerateInboundEntry(QSTRING(_in.listenip), _in.socks_port, "socks", _in_socksConf, "SOCKS_gConf");
    //
    inbounds.append(_in_HTTP);
    inboundsList->addItem("HTTP Global Config");
    inboundsList->item(inboundsList->count() - 1)->setCheckState(Qt::Unchecked);
    //
    inbounds.append(_in_SOCKS);
    inboundsList->addItem("SOCKS Global Config");
    inboundsList->item(inboundsList->count() - 1)->setCheckState(Qt::Unchecked);
}

void RouteEditor::on_insertBlackBtn_clicked()
{
    LOADINGCHECK
    auto blackHole = GenerateBlackHoleOUT(false);
    auto tag = "blackhole_" + QString::number(QTime::currentTime().msecsSinceStartOfDay());
    auto _blackHoleOutbound = GenerateOutboundEntry("blackhole", blackHole, QJsonObject(), QJsonObject(), "0.0.0.0", tag);
    outbounds.append(_blackHoleOutbound);
    outboundsList->addItem(tag);
}

void RouteEditor::on_delOutboundBtn_clicked()
{
    LOADINGCHECK

    if (outboundsList->currentRow() < 0) {
        return;
    }

    auto index = outboundsList->currentRow();
    outbounds.removeAt(index);
    outboundsList->takeItem(index);
}

void RouteEditor::on_delInboundBtn_clicked()
{
    LOADINGCHECK

    if (inboundsList->currentRow() < 0) {
        return;
    }

    auto index = inboundsList->currentRow();
    inbounds.removeAt(index);
    inboundsList->takeItem(index);
}

void RouteEditor::on_addInboundBtn_clicked()
{
    LOADINGCHECK
    InboundEditor w(QJsonObject(), this);
    auto _result = w.OpenEditor();
    inbounds.append(_result);
    inboundsList->addItem(tr("New Inbound"));
}

void RouteEditor::on_addOutboundBtn_clicked()
{
    LOADINGCHECK
    ImportConfigWindow *w = new ImportConfigWindow(this);
    // True here for not keep the inbounds.
    auto configs = w->OpenImport(true);

    for (auto conf : configs) {
        auto name = configs.key(conf, "");

        if (name.isEmpty())
            continue;

        // conf is rootObject, needs to unwrap it.
        auto confList = conf["outbounds"].toArray();

        for (int i = 0; i < confList.count(); i++) {
            outbounds.append(confList[i]);
            outboundsList->addItem(name + "_" + QString::number(i));
        }
    }
}
