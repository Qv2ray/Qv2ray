#include "w_ConnectionEditWindow.h"
#include "w_MainWindow.h"
#include <QDebug>
#include <QFile>
#include <QIntValidator>
#include <iostream>
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
    //
    LoadGUIContents();
    vmess = VMessOut::ServerObject();
    vmess.users.push_back(VMessOut::ServerObject::UserObject());
    stream = StreamSettingsObject();
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
    auto alias = new_config ? (ui->ipLineEdit->text() + "_" + ui->portLineEdit->text()) : _alias;
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
    emit s_reload_config(!new_config);
}

void ConnectionEditWindow::on_ipLineEdit_textEdited(const QString &arg1)
{
    vmess.address = arg1.toStdString();
    //
    // No thanks.
    //if (ui->httpHostTxt->toPlainText() == "") {
    //    ui->httpHostTxt->setPlainText(arg1);
    //    on_httpHostTxt_textChanged();
    //}
    //
    //if (ui->wsHeadersTxt->toPlainText() == "") {
    //    ui->wsHeadersTxt->setPlainText("Host|" + arg1);
    //    on_wsHeadersTxt_textChanged();
    //}
}

void ConnectionEditWindow::on_portLineEdit_textEdited(const QString &arg1)
{
    if (arg1 != "") {
        vmess.port = stoi(arg1.toStdString());
    }
}

void ConnectionEditWindow::on_idLineEdit_textEdited(const QString &arg1)
{
    if (vmess.users.size() == 0) vmess.users.push_back(VMessOut::ServerObject::UserObject());

    vmess.users.front().id = arg1.toStdString();
}

void ConnectionEditWindow::on_alterLineEdit_textEdited(const QString &arg1)
{
    if (vmess.users.size() == 0) vmess.users.push_back(VMessOut::ServerObject::UserObject());

    vmess.users.front().alterId = stoi(arg1.toStdString());
}

void ConnectionEditWindow::on_securityCombo_currentIndexChanged(const QString &arg1)
{
    if (vmess.users.size() == 0) vmess.users.push_back(VMessOut::ServerObject::UserObject());

    vmess.users.front().security = arg1.toStdString();
}

void ConnectionEditWindow::on_tranportCombo_currentIndexChanged(const QString &arg1)
{
    stream.network = arg1.toStdString();
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

        BLACK(httpHostTxt)
    } catch (...) {
        RED(httpHostTxt)
    }
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

        BLACK(wsHeadersTxt)
    } catch (...) {
        RED(wsHeadersTxt)
    }
}


void ConnectionEditWindow::on_tcpRequestDefBtn_clicked()
{
    ui->tcpRequestTxt->clear();
    ui->tcpRequestTxt->insertPlainText("{\"version\":\"1.1\",\"method\":\"GET\",\"path\":[\"/\"],\"headers\":{\"Host\":[\"www.baidu.com\",\"www.bing.com\"],\"User-Agent\":[\"Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/53.0.2785.143 Safari/537.36\",\"Mozilla/5.0 (iPhone; CPU iPhone OS 10_0_2 like Mac OS X) AppleWebKit/601.1 (KHTML, like Gecko) CriOS/53.0.2785.109 Mobile/14A456 Safari/601.1.46\"],\"Accept-Encoding\":[\"gzip, deflate\"],\"Connection\":[\"keep-alive\"],\"Pragma\":\"no-cache\"}}");
}

void ConnectionEditWindow::on_tcpRespDefBtn_clicked()
{
    ui->tcpRespTxt->clear();
    ui->tcpRespTxt->insertPlainText("{\"version\":\"1.1\",\"status\":\"200\",\"reason\":\"OK\",\"headers\":{\"Content-Type\":[\"application/octet-stream\",\"video/mpeg\"],\"Transfer-Encoding\":[\"chunked\"],\"Connection\":[\"keep-alive\"],\"Pragma\":\"no-cache\"}}");
}

void ConnectionEditWindow::on_tcpRequestTxt_textChanged()
{
    try {
        auto tcpReqObject = StructFromJSONString<TransferSettingObjects::HTTPRequestObject>(ui->tcpRequestTxt->toPlainText());
        stream.tcpSettings.header.request = tcpReqObject;
        BLACK(tcpRequestTxt)
    } catch (...) {
        RED(tcpRequestTxt)
    }
}

void ConnectionEditWindow::on_tcpRespTxt_textChanged()
{
    try {
        auto tcpRspObject = StructFromJSONString<TransferSettingObjects::HTTPResponseObject>(ui->tcpRespTxt->toPlainText());
        stream.tcpSettings.header.response = tcpRspObject;
        BLACK(tcpRespTxt)
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
    stream.quicSettings.security = arg1.toStdString();
}

void ConnectionEditWindow::on_quicKeyTxt_textEdited(const QString &arg1)
{
    stream.quicSettings.key = arg1.toStdString();
}

void ConnectionEditWindow::on_quicHeaderTypeCB_currentIndexChanged(const QString &arg1)
{
    stream.quicSettings.header.type = arg1.toStdString();
}

void ConnectionEditWindow::on_tcpRequestPrettifyBtn_clicked()
{
    try {
        auto tcpReqObject = StructFromJSONString<TransferSettingObjects::HTTPRequestObject>(ui->tcpRequestTxt->toPlainText());
        auto tcpReqObjectStr = StructToJSONString(tcpReqObject);
        ui->tcpRequestTxt->setPlainText(tcpReqObjectStr);
    } catch (...) {
        QvMessageBox(this, tr("#JsonPrettify"), tr("#JsonContainsError"));
    }
}

void ConnectionEditWindow::on_tcpRespPrettifyBtn_clicked()
{
    try {
        auto tcpRspObject = StructFromJSONString<TransferSettingObjects::HTTPResponseObject>(ui->tcpRespTxt->toPlainText());
        auto tcpRspObjectStr = StructToJSONString(tcpRspObject);
        ui->tcpRespTxt->setPlainText(tcpRspObjectStr);
    } catch (...) {
        QvMessageBox(this, tr("#JsonPrettify"), tr("#JsonContainsError"));
    }
}

void ConnectionEditWindow::on_tcpHeaderTypeCB_currentIndexChanged(const QString &arg1)
{
    stream.tcpSettings.header.type = arg1.toStdString();
}

void ConnectionEditWindow::on_wsPathTxt_textEdited(const QString &arg1)
{
    stream.wsSettings.path = arg1.toStdString();
}


void ConnectionEditWindow::on_kcpMTU_valueChanged(int arg1)
{
    stream.kcpSettings.mtu = arg1;
}

void ConnectionEditWindow::on_kcpTTI_valueChanged(int arg1)
{
    stream.kcpSettings.tti  = arg1;
}

void ConnectionEditWindow::on_kcpUploadCapacSB_valueChanged(int arg1)
{
    stream.kcpSettings.uplinkCapacity = arg1;
}

void ConnectionEditWindow::on_kcpCongestionCB_stateChanged(int arg1)
{
    stream.kcpSettings.congestion = arg1 == Qt::Checked;
}

void ConnectionEditWindow::on_kcpDownCapacitySB_valueChanged(int arg1)
{
    stream.kcpSettings.downlinkCapacity = arg1;
}

void ConnectionEditWindow::on_kcpReadBufferSB_valueChanged(int arg1)
{
    stream.kcpSettings.readBufferSize = arg1;
}

void ConnectionEditWindow::on_kcpWriteBufferSB_valueChanged(int arg1)
{
    stream.kcpSettings.writeBufferSize = arg1;
}

void ConnectionEditWindow::on_kcpHeaderType_currentTextChanged(const QString &arg1)
{
    stream.kcpSettings.header.type = arg1.toStdString();
}

void ConnectionEditWindow::on_tranportCombo_currentIndexChanged(int index)
{
    ui->tabWidget->setCurrentIndex(index);
}
