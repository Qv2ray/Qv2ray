#include "w_RouteEditor.h"
#include "QvCoreConfigOperations.h"
#include "ui_w_RouteEditor.h"
#include "w_ConnectionEditWindow.h"

RouteEditor::RouteEditor(QJsonObject connection, const QString alias, QWidget *parent) :
    QDialog(parent),
    root(connection),
    rootAlias(alias),
    ui(new Ui::RouteEditor)
{
    inbounds = root["inbounds"].toArray();
    outbounds = root["outbounds"].toArray();
    routes = StructFromJsonString<RoutingObject>(JsonToString(root["routing"].toObject()));
    ui->setupUi(this);
    ui->outboundsList->clear();

    foreach (auto out, outbounds) {
        bool hasTag = out.toObject().contains("tag");
        //
        auto protocol = out.toObject()["protocol"].toString();
        auto tag = hasTag ? out.toObject()["tag"].toString() : tr("NoTag");
        //
        ui->outboundsList->addItem(tag + " (" + protocol + ")");
    }

    foreach (auto in, inbounds) {
        bool hasTag = in.toObject().contains("tag");
        //
        auto tag = hasTag ?  in.toObject()["tag"].toString() : tr("NoTag");
        auto protocol = in.toObject()["protocol"].toString();
        auto port = in.toObject()["port"].toVariant().toString();
        //
        auto inItem = new QListWidgetItem(tag + " (" + protocol + ": " + port  + ")");
        inItem->setCheckState(Qt::Unchecked);
        ui->inboundsList->addItem(inItem);
    }

    foreach (auto route, routes.rules) {
        ui->routesTable->insertRow(ui->routesTable->rowCount());
        ui->routesTable->setItem(ui->routesTable->rowCount() - 1, 0,
                                 new QTableWidgetItem(route.inboundTag.size() > 0 ? Stringify(route.inboundTag) : "Any"));
        //
        auto tmplist = QList<string>::fromStdList(route.ip);
        tmplist.append(QList<string>::fromStdList(route.domain));
        //
        ui->routesTable->setItem(ui->routesTable->rowCount() - 1, 1,
                                 new QTableWidgetItem(Stringify(tmplist.toStdList())));
        ui->routesTable->setItem(ui->routesTable->rowCount() - 1, 2,
                                 new QTableWidgetItem(QSTRING(route.outboundTag)));
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
    LOG(MODULE_UI, "Outbound selected: " + to_string(currentRow))
    auto outBoundRoot = outbounds[currentRow].toObject();
    //
    auto outboundType = outBoundRoot["protocol"].toString();
    ui->outboundTagLabel->setText(outBoundRoot.contains("tag") ? outBoundRoot["tag"].toString() : tr("NoTag"));
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
        auto Server = StructFromJsonString<ShadowSocksServer>(x);
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
    LOG(MODULE_UI, "Inbound selected: " + to_string(currentRow))
    auto inBoundRoot = inbounds[currentRow].toObject();
    ui->inboundTagLabel->setText(inBoundRoot.contains("tag") ? inBoundRoot["tag"].toString() : tr("#NoTag"));
    ui->inboundTypeLabel->setText(inBoundRoot["protocol"].toString());
    ui->inboundAddressLabel->setText(inBoundRoot["listen"].toString());
    ui->inboundPortLabel->setText(inBoundRoot["port"].toVariant().toString());
}

void RouteEditor::on_routesTable_cellClicked(int row, int column)
{
    for (int i = 0; i < ui->inboundsList->count(); ++i) {
        ui->inboundsList->item(i)->setCheckState(Qt::Unchecked);
    }

    Q_UNUSED(column)
    auto outboundTag = ui->routesTable->item(row, 2)->text();
    int index = FindIndexByTag(outbounds, &outboundTag);
    ui->outboundsList->setCurrentRow(index);
    //
    auto inboundTagList = ui->routesTable->item(row, 0)->text();
    bool isAnyInbounds = inboundTagList == "Any";

    if (isAnyInbounds) {
        for (int i = 0; i < ui->inboundsList->count(); ++i) {
            ui->inboundsList->item(i)->setCheckState(Qt::Checked);
        }
    } else {
        auto rulesList = QList<RuleObject>::fromStdList(routes.rules);

        foreach (auto inboundTag, rulesList[row].inboundTag) {
            auto inTag = QSTRING(inboundTag);
            int index = FindIndexByTag(inbounds, &inTag);
            ui->inboundsList->item(index)->setCheckState(Qt::Checked);
        }
    }
}

void RouteEditor::on_editOutboundBtn_clicked()
{
    auto currentOutbound = outbounds[ui->outboundsList->currentRow()].toObject();
    ConnectionEditWindow *w = new ConnectionEditWindow(currentOutbound, nullptr, this);
    w->exec();
    auto result = w->Result;
    LOG(MODULE_UI, "NOT FINISHED YET")
    delete w;
}
