#include "w_RoutesEditor.h"
#include "QvCoreConfigOperations.h"
#include "ui_w_RoutesEditor.h"
#include "w_OutboundEditor.h"
#include "w_JsonEditor.h"
#include "w_InboundEditor.h"

#define CurrentRule this->rules[this->currentRuleIndex]
#define STATUS(msg) ui->statusLabel->setText(tr(msg));

RouteEditor::RouteEditor(QJsonObject connection, QWidget *parent) :
    QDialog(parent),
    root(connection),
    original(connection),
    ui(new Ui::RouteEditor)
{
    ui->setupUi(this);
    //
    inbounds = root["inbounds"].toArray();
    outbounds = root["outbounds"].toArray();
    DomainStrategy = root["routing"].toObject()["domainStrategy"].toString();

    // Applying Balancers.
    for (auto _balancer :  root["routing"].toObject()["balancers"].toArray()) {
        auto __balancer = _balancer.toObject();
        Balancers[__balancer["tag"].toString()] = QList<QString>();

        for (auto _ : __balancer["selector"].toArray()) {
            Balancers[__balancer["tag"].toString()].append(_.toString());
        }
    }

    rules = QList<RuleObject>::fromStdList(StructFromJsonString<list<RuleObject>>(JsonToString(root["routing"].toObject()["rules"].toArray())));
    //
    ui->outboundsList->clear();
    ui->inboundsList->clear();

    for (auto out : outbounds) {
        bool hasTag = out.toObject().contains("tag");
        //
        auto protocol = out.toObject()["protocol"].toString();
        auto tag = hasTag ? out.toObject()["tag"].toString() : tr("No Tag");
        //
        ui->outboundsList->addItem(tag + " (" + protocol + ")");
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
        ui->inboundsList->addItem(inItem);
    }

    ui->routesTable->clearContents();

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
        ui->routesTable->insertRow(ui->routesTable->rowCount());
        //
        // WARNING There should be an additional check on the final configuration generation process.
        auto enabledItem = new QTableWidgetItem(tr("Enabled"));
        enabledItem->setCheckState(rule.QV2RAY_RULE_ENABLED ? Qt::Checked : Qt::Unchecked);
        ui->routesTable->setItem(ui->routesTable->rowCount() - 1, 0, enabledItem);
        //
        ui->routesTable->setItem(ui->routesTable->rowCount() - 1, 1, new QTableWidgetItem(rule.inboundTag.size() > 0 ? Stringify(rule.inboundTag) : tr("Any")));
        ui->routesTable->setItem(ui->routesTable->rowCount() - 1, 2, new QTableWidgetItem(QString::number(rule.domain.size() + rule.ip.size()) + " " + tr("Items")));
        ui->routesTable->setItem(ui->routesTable->rowCount() - 1, 3, new QTableWidgetItem(QSTRING(rule.outboundTag)));
#undef rule
    }

    if (rules.size() > 0) {
        ui->routesTable->setCurrentItem(ui->routesTable->item(0, 0));
        currentRuleIndex = 0;
        ShowRuleDetail(CurrentRule);
    } else {
        ui->delRouteBtn->setEnabled(false);

        if (ui->inboundsList->count() > 0) ui->inboundsList->setCurrentRow(0);

        if (ui->outboundsList->count() > 0) ui->outboundsList->setCurrentRow(0);
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
                balancerEntry["tag"] = key;
                balancerEntry["selector"] = QJsonArray::fromStringList(item);
                balancers.append(balancerEntry);
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
    delete ui;
}

void RouteEditor::on_buttonBox_accepted()
{
}

void RouteEditor::on_outboundsList_currentRowChanged(int currentRow)
{
    auto outBoundRoot = outbounds[currentRow].toObject();
    auto outboundType = outBoundRoot["protocol"].toString();
    ui->outboundTagLabel->setText(outBoundRoot.contains("tag") ? outBoundRoot["tag"].toString() : tr("No Tag"));
    //
    ui->outboundTypeLabel->setText(outboundType);
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

    ui->outboundAddressLabel->setText(QSTRING(serverAddress));
    ui->outboundPortLabel->setText(QSTRING(serverPort));
}

void RouteEditor::on_inboundsList_currentRowChanged(int currentRow)
{
    auto inBoundRoot = inbounds[currentRow].toObject();
    //
    ui->inboundTagLabel->setText(inBoundRoot.contains("tag") ? inBoundRoot["tag"].toString() : tr("No Tag"));
    ui->inboundTypeLabel->setText(inBoundRoot["protocol"].toString());
    ui->inboundAddressLabel->setText(inBoundRoot["listen"].toString());
    ui->inboundPortLabel->setText(inBoundRoot["port"].toVariant().toString());
}

void RouteEditor::ShowRuleDetail(RuleObject rule)
{
    for (int i = 0; i < ui->inboundsList->count(); ++i) {
        ui->inboundsList->item(i)->setCheckState(Qt::Unchecked);
    }

    auto outboundTag = QSTRING(rule.outboundTag);
    int index = FindIndexByTag(outbounds, &outboundTag);
    ui->outboundsList->setCurrentRow(index);
    //
    auto network = QSTRING(rule.network).toLower();
    ui->netUDPRB->setChecked(network.contains("udp"));
    ui->netTCPRB->setChecked(network.contains("tcp"));
    ui->netBothRB->setChecked(network.contains("tcp") && network.contains("udp"));
    //
    // Set protocol checkboxes.
    auto protocols = QList<string>::fromStdList(CurrentRule.protocol);
    ui->routeProtocolHTTPCB->setChecked(protocols.contains("http"));
    ui->routeProtocolTLSCB->setChecked(protocols.contains("tls"));
    ui->routeProtocolBTCB->setChecked(protocols.contains("bittorrent"));
    //
    // Port
    ui->routePortTxt->setText(QSTRING(CurrentRule.port));
    //
    // Users
    QString users = Stringify(CurrentRule.user, NEWLINE);
    ui->routeUserTxt->setPlainText(users);
    //
    // Incoming Sources
    QString sources = Stringify(CurrentRule.source, NEWLINE);
    ui->sourceIPList->setPlainText(sources);
    //
    // Domains
    QString domains = Stringify(CurrentRule.domain, NEWLINE);
    ui->hostList->setPlainText(domains);
    //
    // Outcoming IPs
    QString ips = Stringify(CurrentRule.ip, NEWLINE);
    ui->ipList->setPlainText(ips);
    //
    // Set Balancer
    ui->balancerSelectionCombo->clear();
    ui->balancerSelectionCombo->addItems(Balancers[QSTRING(CurrentRule.balancerTag)]);
    ui->balancerList->clear();
    ui->balancerList->addItems(Balancers[QSTRING(CurrentRule.balancerTag)]);

    if (rule.inboundTag.size() == 0) {
        for (int i = 0; i < ui->inboundsList->count(); i++) {
            ui->inboundsList->item(i)->setCheckState(Qt::Checked);
        }

        ui->inboundsList->setCurrentRow(0);
    } else {
        for (auto inboundTag : rule.inboundTag) {
            auto inTag = QSTRING(inboundTag);
            int _index = FindIndexByTag(inbounds, &inTag);

            // FIXED if an inbound is missing (index out of range)
            if (_index >= 0) {
                if (ui->inboundsList->count() <= _index) {
                    QvMessageBox(this, tr("Route Editor"), tr("Cannot find an inbound by tag: ") + tr("Index Out Of Range"));
                    LOG(MODULE_UI, "FATAL: An inbound could not be found.")
                    return;
                }

                ui->inboundsList->item(_index)->setCheckState(Qt::Checked);
                ui->inboundsList->setCurrentRow(_index);
                STATUS("OK")
            } else {
                STATUS("Cannot find inbound by a tag, possible currupted files?")
                LOG(MODULE_UI, "An inbound could not be determined by tag.")
                return;
            }
        }
    }
}

void RouteEditor::on_editOutboundBtn_clicked()
{
    QJsonObject result;
    int row = ui->outboundsList->currentRow();

    if (row < 0) {
        STATUS("No row selected.")
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
        STATUS("Opening JSON editor")
        result = w->OpenEditor();
        delete w;
    } else {
        OutboundEditor *w = new OutboundEditor(currentOutbound, this);
        STATUS("Opening default outbound editor.")
        result = w->OpenEditor();
        delete w;
    }

    outbounds[row] = result;
    on_outboundsList_currentRowChanged(row);
    STATUS("OK")
}

void RouteEditor::on_insertDirectBtn_clicked()
{
    auto freedom = GenerateFreedomOUT("as-is", "", 0);
    auto tag = "Freedom_" + QString::number(QTime::currentTime().msecsSinceStartOfDay());
    auto out = GenerateOutboundEntry("freedom", freedom, QJsonObject(), QJsonObject(), "0.0.0.0", tag);
    this->outbounds.append(out);
    ui->outboundsList->addItem(tag);
    STATUS("Added DIRECT outbound")
}

void RouteEditor::on_editInboundBtn_clicked()
{
    QJsonObject result;
    int row = ui->inboundsList->currentRow();

    if (row < 0) return;

    auto currentInbound = inbounds[row].toObject();
    auto protocol =  currentInbound["protocol"].toString();

    if (protocol != "http" && protocol != "mtproto" && protocol != "socks" && protocol != "dokodemo-door") {
        QvMessageBox(this, tr("Cannot Edit"), tr("Currently, this type of outbound is not supported by the editor.") + "\r\n" +
                     tr("We will launch Json Editor instead."));
        STATUS("Opening JSON editor")
        JsonEditor *w = new JsonEditor(currentInbound, this);
        result = w->OpenEditor();
        delete w;
    } else {
        InboundEditor *w = new InboundEditor(currentInbound, this);
        STATUS("Opening default inbound editor")
        result = w->OpenEditor();
        delete w;
    }

    inbounds[row] = result;
    on_inboundsList_currentRowChanged(row);
    STATUS("OK")
}

void RouteEditor::on_routeProtocolHTTPCB_stateChanged(int arg1)
{
    QList<string> protocols;

    if (arg1 == Qt::Checked) protocols << "http";

    if (ui->routeProtocolTLSCB->isChecked()) protocols << "tls";

    if (ui->routeProtocolBTCB->isChecked()) protocols << "bittorrent";

    CurrentRule.protocol = protocols.toStdList();
    STATUS("Protocol list changed.")
}

void RouteEditor::on_routeProtocolTLSCB_stateChanged(int arg1)
{
    QList<string> protocols;

    if (arg1 == Qt::Checked) protocols << "tls";

    if (ui->routeProtocolHTTPCB->isChecked()) protocols << "http";

    if (ui->routeProtocolBTCB->isChecked()) protocols << "bittorrent";

    CurrentRule.protocol = protocols.toStdList();
    STATUS("Protocol list changed.")
}

void RouteEditor::on_routeProtocolBTCB_stateChanged(int arg1)
{
    QList<string> protocols;

    if (arg1 == Qt::Checked) protocols << "bittorrent";

    if (ui->routeProtocolHTTPCB->isChecked()) protocols << "http";

    if (ui->routeProtocolTLSCB->isChecked()) protocols << "tls";

    CurrentRule.protocol = protocols.toStdList();
    STATUS("Protocol list changed.")
}
void RouteEditor::on_balabcerAddBtn_clicked()
{
    if (!ui->balancerSelectionCombo->currentText().isEmpty()) {
        this->Balancers[QSTRING(CurrentRule.balancerTag)].append(ui->balancerSelectionCombo->currentText());
    }

    ui->balancerList->addItem(ui->balancerSelectionCombo->currentText());
    ui->balancerSelectionCombo->setEditText("");
    STATUS("OK")
}

void RouteEditor::on_balancerDelBtn_clicked()
{
    if (ui->balancerList->currentRow() < 0) {
        return;
    }

    Balancers[QSTRING(CurrentRule.balancerTag)].removeAt(ui->balancerList->currentRow());
    ui->balancerList->takeItem(ui->balancerList->currentRow());
    STATUS("Removed a balancer entry.")
}

void RouteEditor::on_hostList_textChanged()
{
    CurrentRule.domain = SplitLinesStdString(ui->hostList->toPlainText()).toStdList();
}

void RouteEditor::on_ipList_textChanged()
{
    CurrentRule.ip = SplitLinesStdString(ui->ipList->toPlainText()).toStdList();
}

void RouteEditor::on_routePortTxt_textEdited(const QString &arg1)
{
    CurrentRule.port = arg1.toStdString();
}

void RouteEditor::on_routeUserTxt_textEdited(const QString &arg1)
{
    CurrentRule.user = SplitLinesStdString(arg1).toStdList();
}

void RouteEditor::on_routesTable_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn)
{
    Q_UNUSED(currentColumn)
    Q_UNUSED(previousColumn)
    Q_UNUSED(previousRow)

    if (currentRow < 0) {
        return;
    }

    currentRuleIndex = currentRow;
    ShowRuleDetail(CurrentRule);
}

void RouteEditor::on_addRouteBtn_clicked()
{
    // Add Route
    RuleObject rule;
    //

    if (QSTRING(rule.balancerTag).isEmpty()) {
        // Default balancer tag.
        auto bTag = GenerateRandomString();
        rule.balancerTag = bTag.toStdString();
        Balancers[bTag] = QStringList();
    }

    //
    ui->routesTable->insertRow(ui->routesTable->rowCount());
    // WARNING There will be an additional check on the final configuration generation process.
    auto enabledItem = new QTableWidgetItem(tr("Enabled"));
    enabledItem->setCheckState(rule.QV2RAY_RULE_ENABLED ? Qt::Checked : Qt::Unchecked);
    ui->routesTable->setItem(ui->routesTable->rowCount() - 1, 0, enabledItem);
    ui->routesTable->setItem(ui->routesTable->rowCount() - 1, 1, new QTableWidgetItem(rule.inboundTag.size() > 0 ? Stringify(rule.inboundTag) : tr("Any")));
    ui->routesTable->setItem(ui->routesTable->rowCount() - 1, 2, new QTableWidgetItem(QString::number(rule.domain.size() + rule.ip.size()) + " " + tr("Items")));
    ui->routesTable->setItem(ui->routesTable->rowCount() - 1, 3, new QTableWidgetItem(QSTRING(rule.outboundTag)));
    rules.append(rule);
    currentRuleIndex = ui->routesTable->rowCount() - 1;
    ui->routesTable->setCurrentCell(currentRuleIndex, 0);
    ShowRuleDetail(CurrentRule);
    ui->delRouteBtn->setEnabled(true);
}

void RouteEditor::on_changeIOBtn_clicked()
{
    QString outbound = "";

    if (ui->outboundsList->currentRow() < 0) {
        // Don't return as someone may use the outboundTag
        //
        //QvMessageBox(this, tr("Changing route inbound/outbound"), tr("Please select an outbound from the list."));
        //return;
        QvMessageBox(this, tr("Changing route inbound/outbound"),
                     tr("You didn't select an outbound.") + NEWLINE +
                     tr("Banlancer will be used."));
    } else {
        outbound = outbounds[ui->outboundsList->currentRow()].toObject()["tag"].toString();
    }

    QList<string> new_inbounds;
    QList<string> new_inbounds_name;

    for (int i = 0; i < ui->inboundsList->count(); i++) {
        auto _item = ui->inboundsList->item(i);

        if (_item->checkState() == Qt::Checked) {
            // WARN there are possiblilties that someone may forget to set the tag.
            new_inbounds.append(inbounds[i].toObject()["tag"].toString().toStdString());
            new_inbounds_name.append(_item->text().toStdString());
        }
    }

    if (new_inbounds.size() == 0) {
        // TODO what to do?
    }

    if (new_inbounds.contains("")) {
        // Empty tag.
        auto result1 = QvMessageBoxAsk(this, tr("Changing route inbound/outbound"), tr("One or more inbound config(s) have no tag configured, do you still want to continue?"));

        if (result1 != QMessageBox::Yes) {
            return;
        }
    }

    auto result = QvMessageBoxAsk(this, tr("Changing route inbound/outbound"),
                                  tr("Are you sure to change the inbound/outbound of currently selected route?")  + NEWLINE +
                                  tr("Current inbound/outbound combinations:") + NEWLINE + NEWLINE + tr("Inbounds: ") + NEWLINE +
                                  Stringify(new_inbounds_name.toStdList(), NEWLINE) + NEWLINE + tr("Outbound: ") + outbound);

    if (result != QMessageBox::Yes) {
        STATUS("Canceled changing inbound/outbound combination.")
        return;
    }

    CurrentRule.inboundTag = new_inbounds.toStdList();
    CurrentRule.outboundTag = outbound.toStdString();
    STATUS("OK")
}

void RouteEditor::on_routesTable_cellChanged(int row, int column)
{
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

    rules[row].QV2RAY_RULE_ENABLED = ui->routesTable->item(row, column)->checkState() == Qt::Checked;
    STATUS((rules[row].QV2RAY_RULE_ENABLED ? "Enabled a route" : "Disabled a route"))
}

void RouteEditor::on_netBothRB_clicked()
{
    CurrentRule.network = "tcp,udp";
}

void RouteEditor::on_netUDPRB_clicked()
{
    CurrentRule.network = "udp";
}

void RouteEditor::on_netTCPRB_clicked()
{
    CurrentRule.network = "tcp";
}

void RouteEditor::on_routeUserTxt_textChanged()
{
    CurrentRule.user = SplitLinesStdString(ui->routeUserTxt->toPlainText()).toStdList();
}

void RouteEditor::on_sourceIPList_textChanged()
{
    CurrentRule.source = SplitLinesStdString(ui->sourceIPList->toPlainText()).toStdList();
}
