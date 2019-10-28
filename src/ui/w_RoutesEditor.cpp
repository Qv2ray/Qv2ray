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
    ui(new Ui::RouteEditor)
{
    ui->setupUi(this);
    //
    inbounds = root["inbounds"].toArray();
    outbounds = root["outbounds"].toArray();
    DomainStrategy = root["routing"].toObject()["domainStrategy"].toString();

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

    for (auto route : rules) {
        ui->routesTable->insertRow(ui->routesTable->rowCount());
        // There will be an additional check on the final configuration generation process.
        auto enabledItem = new QTableWidgetItem(tr("Enabled"));
        enabledItem->setCheckState(route.enabled ? Qt::Checked : Qt::Unchecked);
        ui->routesTable->setItem(ui->routesTable->rowCount() - 1, 0, enabledItem);
        ui->routesTable->setItem(ui->routesTable->rowCount() - 1, 1, new QTableWidgetItem(route.inboundTag.size() > 0 ? Stringify(route.inboundTag) : tr("Any")));
        ui->routesTable->setItem(ui->routesTable->rowCount() - 1, 2, new QTableWidgetItem(QString::number(route.domain.size() + route.ip.size()) + " " + tr("Items")));
        ui->routesTable->setItem(ui->routesTable->rowCount() - 1, 3, new QTableWidgetItem(QSTRING(route.outboundTag)));
    }
}

QJsonObject RouteEditor::OpenEditor()
{
    this->exec();
    QJsonArray balancers;

    for (auto item : Balancers) {
        QJsonObject balancerEntry;
        balancerEntry["tag"] = Balancers.key(item);
        balancerEntry["selector"] = QJsonArray::fromStringList(item);
        balancers.append(balancerEntry);
    }

    //
    QJsonObject routing;
    routing["domainStrategy"] = DomainStrategy;
    routing["rules"] = GetRootObject(rules.toStdList());
    routing["balancers"] = balancers;
    //
    root["inbounds"] = inbounds;
    root["outbounds"] = outbounds;
    root["routing"] = routing;
    return root;
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
    auto protocols = QList<string>::fromStdList(CurrentRule.protocol);
    //
    // Set protocol checkboxes.
    ui->routeProtocolHTTPCB->setChecked(false);
    ui->routeProtocolBTCB->setChecked(false);
    ui->routeProtocolTLSCB->setChecked(false);

    if (protocols.contains("http")) {
        ui->routeProtocolHTTPCB->setChecked(true);
    }

    if (protocols.contains("tls")) {
        ui->routeProtocolTLSCB->setChecked(true);
    }

    if (protocols.contains("bittorrent")) {
        ui->routeProtocolBTCB->setChecked(true);
    }

    ui->routePortTxt->setText(QSTRING(CurrentRule.port));
    //
    QString users = std::accumulate(CurrentRule.user.begin(), CurrentRule.user.end(), QString(), [](QString result, const string & str) {
        result.append(QSTRING(str) + NEWLINE);
        return result;
    });
    ui->routeUserTxt->setPlainText(users);
    //

    if (rule.inboundTag.size() == 0) {
        for (int i = 0; i < ui->inboundsList->count(); i++) {
            ui->inboundsList->item(i)->setCheckState(Qt::Checked);
        }
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
                STATUS("OK")
            } else {
                STATUS("Cannot find inbound by a tag, possible currupted files?")
                LOG(MODULE_UI, "An inbound could not be determined by tag.")
                return;
            }
        }
    }
}

void RouteEditor::on_routesTable_cellClicked(int row, int column)
{
    Q_UNUSED(column)

    if (row < 0) {
        return;
    }

    currentRuleIndex = row;
    ShowRuleDetail(CurrentRule);
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
}

void RouteEditor::on_balancerDelBtn_clicked()
{
}

void RouteEditor::on_hostList_textChanged()
{
}

void RouteEditor::on_ipList_textChanged()
{
}

void RouteEditor::on_routePortTxt_textEdited(const QString &arg1)
{
}

void RouteEditor::on_routeUserTxt_textEdited(const QString &arg1)
{
}
