#include "w_ConnectionEditWindow.h"
#include "w_MainWindow.h"
#include <QDebug>
#include <QFile>
#include <QIntValidator>
#include <iostream>

#define RED(obj)   \
    auto p = ui->obj->palette(); \
    p.setColor(QPalette::Text, Qt::red); \
    ui->obj->setPalette(p);

#define WHITE(obj)   \
    auto p = ui->obj->palette(); \
    p.setColor(QPalette::Text, Qt::black); \
    ui->obj->setPalette(p);

ConnectionEditWindow::ConnectionEditWindow(QWidget *parent)
    : QDialog(parent),
      _alias(),
      stream(),
      vmess(),
      ui(new Ui::ConnectionEditWindow)
{
    ui->setupUi(this);
    ui->portLineEdit->setValidator(new QIntValidator());
    ui->alterLineEdit->setValidator(new QIntValidator());
}

ConnectionEditWindow::ConnectionEditWindow(QJsonObject editRootObject, QString alias, QWidget *parent)
    : ConnectionEditWindow(parent)
{
    _alias = alias;
    original = editRootObject;
    auto outBoundRoot = original["outbounds"].toArray().first().toObject();
    // TEST
    vmess = StructFromJSONString<VMessOut>(JSONToString(outBoundRoot["settings"].toObject())).vnext.front();
    stream = StructFromJSONString<StreamSettingsObject>(JSONToString(outBoundRoot["streamSettings"].toObject()));
    LoadGUIContents();
}


ConnectionEditWindow::~ConnectionEditWindow()
{
    delete ui;
}

void ConnectionEditWindow::LoadGUIContents()
{
    ui->ipLineEdit->setText(QString::fromStdString(vmess.address));
    ui->portLineEdit->setText(QString::number(vmess.port));
    ui->idLineEdit->setText(QString::fromStdString(vmess.users.front().id));
    ui->alterLineEdit->setText(QString::number(vmess.users.front().alterId));
    ui->securityCombo->setCurrentText(QString::fromStdString(vmess.users.front().security));
    ui->tranportCombo->setCurrentText(QString::fromStdString(stream.network));
    ui->tlsCB->setChecked(stream.security == "tls");
    // TCP
    ui->tcpHeaderTypeCB->setCurrentText(QString::fromStdString(stream.tcpSettings.header.type));
    ui->tcpRequestTxt->setPlainText(StructToJSONString(stream.tcpSettings.header.request));
    ui->tcpRespTxt->setPlainText(StructToJSONString(stream.tcpSettings.header.response));
    // HTTP
    QString allHosts;

    foreach (auto host, stream.httpSettings.host) {
        allHosts = allHosts + QString::fromStdString(host) + "\r\n";
    }

    ui->httpHostTxt->setPlainText(allHosts);
    ui->httpPathTxt->setText(QString::fromStdString(stream.httpSettings.path));
    // WS
    ui->wsPathTxt->setText(QString::fromStdString(stream.wsSettings.path));
    QString wsHeaders;

    foreach (auto _, stream.wsSettings.headers) {
        wsHeaders = wsHeaders + QString::fromStdString(_.first + "|" + _.second) + "\r\n";
    }

    ui->wsHeadersTxt->setPlainText(wsHeaders);
    // mKCP
    ui->kcpMTU->setValue(stream.kcpSettings.mtu);
    ui->kcpTTI->setValue(stream.kcpSettings.tti);
    ui->kcpHeaderType->setCurrentText(QString::fromStdString(stream.kcpSettings.header.type));
    ui->kcpCongestionCB->setChecked(stream.kcpSettings.congestion);
    ui->kcpReadBufferSB->setValue(stream.kcpSettings.readBufferSize);
    ui->kcpUploadCapacSB->setValue(stream.kcpSettings.uplinkCapacity);
    ui->kcpDownCapacitySB->setValue(stream.kcpSettings.downlinkCapacity);
    ui->kcpWriteBufferSB->setValue(stream.kcpSettings.writeBufferSize);
    // DS
    ui->dsPathTxt->setText(QString::fromStdString(stream.dsSettings.path));
    // QUIC
    ui->quicKeyTxt->setText(QString::fromStdString(stream.quicSettings.key));
    ui->quicSecurityCB->setCurrentText(QString::fromStdString(stream.quicSettings.security));
    ui->quicHeaderTypeCB->setCurrentText(QString::fromStdString(stream.quicSettings.header.type));
    // SOCKOPT
    ui->tProxyCB->setCurrentText(QString::fromStdString(stream.sockopt.tproxy));
    ui->tcpFastOpenCB->setChecked(stream.sockopt.tcpFastOpen);
    ui->soMarkSpinBox->setValue(stream.sockopt.mark);
}

void ConnectionEditWindow::on_buttonBox_accepted()
{
    bool new_config = _alias == "";
    auto alias = new_config ? ui->ipLineEdit->text() : _alias;
    //
    auto outbound = GenerateConnectionJson();
    QJsonArray outbounds;
    outbounds.append(outbound);

    if (original.contains("outbounds")) {
        original.remove("outbounds");
    }

    original.insert("outbounds", outbounds);
    SaveConnectionConfig(original, &alias);
    auto globalConf = GetGlobalConfig();

    if (new_config) {
        // New config...
        globalConf.configs.push_back(alias.toStdString());
    }

    SetGlobalConfig(globalConf);
    emit s_reload_config();
}

void ConnectionEditWindow::on_ipLineEdit_textEdited(const QString &arg1)
{
    vmess.address = arg1.toStdString();
}

void ConnectionEditWindow::on_portLineEdit_textEdited(const QString &arg1)
{
    if (arg1 != "") {
        vmess.port = stoi(arg1.toStdString());
    }
}

void ConnectionEditWindow::on_idLineEdit_textEdited(const QString &arg1)
{
    LOG("BUGFUL!")
    vmess.users.front().id = arg1.toStdString();
}

void ConnectionEditWindow::on_alterLineEdit_textEdited(const QString &arg1)
{
    LOG("BUGFUL!")
    vmess.users.front().id = arg1.toStdString();
}

void ConnectionEditWindow::on_securityCombo_currentIndexChanged(const QString &arg1)
{
    LOG("BUGFUL!")
    vmess.users.front().security = arg1.toStdString();
}

void ConnectionEditWindow::on_tranportCombo_currentIndexChanged(const QString &arg1)
{
    stream.network = arg1.toStdString();
}

void ConnectionEditWindow::on_comboBox_currentIndexChanged(const QString &arg1)
{
    stream.tcpSettings.header.type = arg1.toStdString();
}

void ConnectionEditWindow::on_httpPathTxt_textEdited(const QString &arg1)
{
    stream.httpSettings.path = arg1.toStdString();
}

void ConnectionEditWindow::on_httpHostTxt_textChanged()
{
    try {
        QStringList hosts = ui->httpHostTxt->toPlainText().replace("\n", "").split("\r");
        stream.httpSettings.host.clear();

        foreach (auto host, hosts) {
            stream.httpSettings.host.push_back(host.toStdString());
        }

        WHITE(httpHostTxt)
    } catch (...) {
        RED(httpHostTxt)
    }
}

void ConnectionEditWindow::on_lineEdit_textEdited(const QString &arg1)
{
    stream.wsSettings.path = arg1.toStdString();
}

void ConnectionEditWindow::on_wsHeadersTxt_textChanged()
{
    try {
        QStringList headers = ui->wsHeadersTxt->toPlainText().replace("\n", "").split("\r");
        stream.wsSettings.headers.clear();

        foreach (auto header, headers) {
            auto content = header.split("|");

            if (content.length() < 2) throw "fast fail.";

            stream.wsSettings.headers.insert(make_pair(content[0].toStdString(), content[1].toStdString()));
        }

        WHITE(wsHeadersTxt)
    } catch (...) {
        RED(wsHeadersTxt)
    }
}

void ConnectionEditWindow::on_spinBox_valueChanged(int arg1)
{
    stream.kcpSettings.mtu = arg1;
}

void ConnectionEditWindow::on_tcpRequestDefBtn_clicked()
{
    ui->tcpRequestTxt->clear();
    ui->tcpRequestTxt->insertPlainText("{ \"version\" : \"TODO....\"");
}

void ConnectionEditWindow::on_tcpRespDefBtn_clicked()
{
    ui->tcpRespTxt->clear();
    ui->tcpRespTxt->insertPlainText("{ \"version\" : \"TODO....\"");
}

void ConnectionEditWindow::on_tcpRequestTxt_textChanged()
{
    try {
        auto httpReqObject = StructFromJSONString<_TransferSettingObjects::TRANSFERObjectsInternal::HTTPRequestObject>(ui->tcpRequestTxt->toPlainText());
        stream.tcpSettings.header.request = httpReqObject;
        WHITE(tcpRequestTxt)
    } catch (...) {
        RED(tcpRequestTxt)
    }
}

void ConnectionEditWindow::on_tcpRespTxt_textChanged()
{
    try {
        auto httpRspObject = StructFromJSONString<_TransferSettingObjects::TRANSFERObjectsInternal::HTTPResponseObject>(ui->tcpRespTxt->toPlainText());
        stream.tcpSettings.header.response = httpRspObject;
        WHITE(tcpRespTxt)
    } catch (...) {
        RED(tcpRespTxt)
    }
}

void ConnectionEditWindow::on_genJsonBtn_clicked()
{
    auto json = GenerateConnectionJson();
    ui->finalJson->setText(JSONToString(json));
}

QJsonObject ConnectionEditWindow::GenerateConnectionJson()
{
    // VMess is a server, however we need an array { "vnext": [] }
    QJsonArray vnext;
    vnext.append(JSONFromString(StructToJSONString(vmess)));
    QJsonObject settings;
    settings.insert("vnext", vnext);
    auto mux = JSONFromString(StructToJSONString(GetGlobalConfig().mux));
    auto streaming = JSONFromString(StructToJSONString(stream));
    auto root = GenerateOutboundEntry("vmess", settings, streaming, mux, "0.0.0.0", OUTBOUND_TAG_PROXY);
    return root;
}

void ConnectionEditWindow::on_tlsCB_stateChanged(int arg1)
{
    stream.security = arg1 == Qt::Checked ? "tls" : "none";
}

void ConnectionEditWindow::on_soMarkSpinBox_valueChanged(int arg1)
{
    stream.sockopt.mark = arg1;
}

void ConnectionEditWindow::on_tcpFastOpenCB_stateChanged(int arg1)
{
    stream.sockopt.tcpFastOpen = arg1 == Qt::Checked;
}

void ConnectionEditWindow::on_tProxyCB_currentIndexChanged(const QString &arg1)
{
    stream.sockopt.tproxy = arg1.toStdString();
}

void ConnectionEditWindow::on_quicSecurityCB_currentTextChanged(const QString &arg1)
{
    stream.quicSettings.header.type = arg1.toStdString();
}

void ConnectionEditWindow::on_quicKeyTxt_textEdited(const QString &arg1)
{
    stream.quicSettings.key = arg1.toStdString();
}

void ConnectionEditWindow::on_quicHeaderTypeCB_currentIndexChanged(const QString &arg1)
{
    stream.quicSettings.header.type = arg1.toStdString();
}
