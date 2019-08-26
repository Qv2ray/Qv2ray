#include "w_RouteEditor.h"
#include "ui_w_RouteEditor.h"

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

        if (hasTag) {
            ui->outboundsList->addItem(out.toObject()["tag"].toString());
        } else {
            ui->outboundsList->addItem(out.toObject()["protocol"].toString());
        }
    }

    foreach (auto in, inbounds) {
        bool hasTag = in.toObject().contains("tag");
        auto inItem = new QListWidgetItem();
        inItem->setCheckState(Qt::Unchecked);

        if (hasTag) {
            inItem->setText(in.toObject()["tag"].toString());
        } else {
            inItem->setText(in.toObject()["protocol"].toString());
        }

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
    ui->outboundTagLabel->setText(outBoundRoot.contains("tag") ? outBoundRoot["tag"].toString() : tr("#NoTag"));
    auto outboundType = outBoundRoot["protocol"].toString();
    ui->outboundTypeLabel->setText(outboundType);

    if (outboundType == "vmess") {
        auto x = StructFromJsonString<VMessServerObject>(JsonToString(outBoundRoot["settings"].toObject()["vnext"].toArray().first().toObject()));
        ui->outboundAddressLabel->setText(QSTRING(x.address));
        ui->outboundPortLabel->setText(QSTRING(to_string(x.port)));
    } else if (outboundType == "shadowsocks") {
        auto x = JsonToString(outBoundRoot["settings"].toObject()["servers"].toArray().first().toObject());
        auto Server = StructFromJsonString<ShadowSocksServer>(x);
        ui->outboundAddressLabel->setText(QSTRING(Server.address));
        ui->outboundPortLabel->setText(QSTRING(to_string(Server.port)));
    } else if (outboundType == "socks") {
        auto x = JsonToString(outBoundRoot["settings"].toObject()["servers"].toArray().first().toObject());
        auto Server = StructFromJsonString<SocksServerObject>(x);
        ui->outboundAddressLabel->setText(QSTRING(Server.address));
        ui->outboundPortLabel->setText(QSTRING(to_string(Server.port)));
    }
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
    Q_UNUSED(column)
    auto outboundTag = ui->routesTable->item(row, 2)->text();
    ui->outboundsList->setCurrentItem(ui->outboundsList->findItems(outboundTag, Qt::MatchExactly).first());
    //
    auto inboundTagList = ui->routesTable->item(row, 0)->text();
    bool isAnyInbounds = inboundTagList == "any";

    if (isAnyInbounds) {
    }
}
