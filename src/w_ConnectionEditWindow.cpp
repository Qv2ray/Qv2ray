#include <QDebug>
#include <QFile>
#include <QIntValidator>
#include <iostream>

#include "w_ConnectionEditWindow.h"
#include "w_MainWindow.h"

#define GEN_JSON ConnectionEditWindow::on_genJsonBtn_clicked();

ConnectionEditWindow::ConnectionEditWindow(QWidget *parent)
    : QDialog(parent),
      _alias(),
      ui(new Ui::ConnectionEditWindow),
      stream(),
      vmess(),
      shadowsocks()
{
    ui->setupUi(this);
    ui->portLineEdit->setValidator(new QIntValidator());
    ui->alterLineEdit->setValidator(new QIntValidator());
    shadowsocks = ShadowSocksServerObject();
    vmess = VMessServerObject();
    vmess.users.push_back(VMessServerObject::UserObject());
    stream = StreamSettingsObject();
    ReLoad_GUI_JSON_ModelContent();
    GEN_JSON
}

ConnectionEditWindow::ConnectionEditWindow(QJsonObject editRootObject, QString alias, QWidget *parent)
    : ConnectionEditWindow(parent)
{
    _alias = alias;
    original = editRootObject;
    auto outBoundRoot = original["outbounds"].toArray().first().toObject();
    OutboundType = outBoundRoot["protocol"].toString();

    if (OutboundType == "vmess") {
        vmess = StructFromJSONString<VMessServerObject>(JSONToString(outBoundRoot["settings"].toObject()["vnext"].toArray().first().toObject()));
        stream = StructFromJSONString<StreamSettingsObject>(JSONToString(outBoundRoot["streamSettings"].toObject()));
        shadowsocks.port = vmess.port;
        shadowsocks.address = vmess.address;
    } else if (OutboundType == "shadowsocks") {
        shadowsocks = StructFromJSONString<ShadowSocksServerObject>(JSONToString(outBoundRoot["settings"].toObject()["servers"].toArray().first().toObject()));
        vmess.address = shadowsocks.address;
        vmess.port = shadowsocks.port;
    }

    ReLoad_GUI_JSON_ModelContent();
    GEN_JSON
}


ConnectionEditWindow::~ConnectionEditWindow()
{
    delete ui;
}

void ConnectionEditWindow::ReLoad_GUI_JSON_ModelContent()
{
    if (OutboundType == "vmess") {
        ui->outBoundTypeCombo->setCurrentIndex(0);
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
    } else if (OutboundType == "shadowsocks") {
        ui->outBoundTypeCombo->setCurrentIndex(1);
        // ShadowSocks Configs
        ui->ipLineEdit->setText(QString::fromStdString(shadowsocks.address));
        ui->portLineEdit->setText(QString::number(shadowsocks.port));
        ui->ss_emailTxt->setText(QString::fromStdString(shadowsocks.email));
        ui->ss_levelSpin->setValue(shadowsocks.level);
        ui->ss_otaCheckBox->setChecked(shadowsocks.ota);
        ui->ss_passwordTxt->setText(QString::fromStdString(shadowsocks.password));
        ui->ss_encryptionMethod->setCurrentText(QString::fromStdString(shadowsocks.method));
    }
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
    shadowsocks.address = arg1.toStdString();
    GEN_JSON
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
        shadowsocks.port = stoi(arg1.toStdString());
        GEN_JSON
    }
}

void ConnectionEditWindow::on_idLineEdit_textEdited(const QString &arg1)
{
    if (vmess.users.size() == 0) vmess.users.push_back(VMessServerObject::UserObject());

    vmess.users.front().id = arg1.toStdString();
    GEN_JSON
}

void ConnectionEditWindow::on_alterLineEdit_textEdited(const QString &arg1)
{
    if (vmess.users.size() == 0) vmess.users.push_back(VMessServerObject::UserObject());

    vmess.users.front().alterId = stoi(arg1.toStdString());
    GEN_JSON
}

void ConnectionEditWindow::on_securityCombo_currentIndexChanged(const QString &arg1)
{
    if (vmess.users.size() == 0) vmess.users.push_back(VMessServerObject::UserObject());

    vmess.users.front().security = arg1.toStdString();
    GEN_JSON
}

void ConnectionEditWindow::on_tranportCombo_currentIndexChanged(const QString &arg1)
{
    stream.network = arg1.toStdString();
    GEN_JSON
}

void ConnectionEditWindow::on_httpPathTxt_textEdited(const QString &arg1)
{
    stream.httpSettings.path = arg1.toStdString();
    GEN_JSON
}

void ConnectionEditWindow::on_httpHostTxt_textChanged()
{
    try {
        QStringList hosts = ui->httpHostTxt->toPlainText().replace("\r", "").split("\n");
        stream.httpSettings.host.clear();

        foreach (auto host, hosts) {
            if (host.trimmed() != "")
                stream.httpSettings.host.push_back(host.trimmed().toStdString());
        }

        GEN_JSON
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

        GEN_JSON
        BLACK(wsHeadersTxt)
    } catch (...) {
        RED(wsHeadersTxt)
    }
}


void ConnectionEditWindow::on_tcpRequestDefBtn_clicked()
{
    ui->tcpRequestTxt->clear();
    ui->tcpRequestTxt->insertPlainText("{\"version\":\"1.1\",\"method\":\"GET\",\"path\":[\"/\"],\"headers\":"
                                       "{\"Host\":[\"www.baidu.com\",\"www.bing.com\"],\"User-Agent\":"
                                       "[\"Mozilla/5.0 (Windows NT 10.0; WOW64) "
                                       "AppleWebKit/537.36 (KHTML, like Gecko) Chrome/53.0.2785.143 Safari/537.36\","
                                       "\"Mozilla/5.0 (iPhone; CPU iPhone OS 10_0_2 like Mac OS X) "
                                       "AppleWebKit/601.1 (KHTML, like Gecko) CriOS/53.0.2785.109 Mobile/14A456 "
                                       "Safari/601.1.46\"],\"Accept-Encoding\":[\"gzip, deflate\"],"
                                       "\"Connection\":[\"keep-alive\"],\"Pragma\":\"no-cache\"}}");
    GEN_JSON
}

void ConnectionEditWindow::on_tcpRespDefBtn_clicked()
{
    ui->tcpRespTxt->clear();
    ui->tcpRespTxt->insertPlainText("{\"version\":\"1.1\",\"status\":\"200\",\"reason\":\"OK\",\"headers\":{\"Content-Type\":[\"application/octet-stream\",\"video/mpeg\"],\"Transfer-Encoding\":[\"chunked\"],\"Connection\":[\"keep-alive\"],\"Pragma\":\"no-cache\"}}");
    GEN_JSON
}

void ConnectionEditWindow::on_tcpRequestTxt_textChanged()
{
    try {
        auto tcpReqObject = StructFromJSONString<TransferSettingObjects::HTTPRequestObject>(ui->tcpRequestTxt->toPlainText());
        stream.tcpSettings.header.request = tcpReqObject;
        GEN_JSON
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
        GEN_JSON
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
    // VMess is only a ServerObject, and we need an array { "vnext": [] }
    QJsonObject settings;
    auto mux = JSONFromString(StructToJSONString(GetGlobalConfig().mux));
    auto streaming = JSONFromString(StructToJSONString(stream));

    if (OutboundType == "vmess") {
        QJsonArray vnext;
        vnext.append(GetRootObject(vmess));
        settings.insert("vnext", vnext);
    } else if (OutboundType == "shadowsocks") {
        streaming = QJsonObject();
        QJsonArray servers;
        servers.append(GetRootObject(shadowsocks));
        settings["servers"] = servers;
    }

    auto root = GenerateOutboundEntry(OutboundType, settings, streaming, mux, "0.0.0.0", OUTBOUND_TAG_PROXY);
    return root;
}

void ConnectionEditWindow::on_tlsCB_stateChanged(int arg1)
{
    stream.security = arg1 == Qt::Checked ? "tls" : "none";
    GEN_JSON
}
void ConnectionEditWindow::on_soMarkSpinBox_valueChanged(int arg1)
{
    stream.sockopt.mark = arg1;
    GEN_JSON
}
void ConnectionEditWindow::on_tcpFastOpenCB_stateChanged(int arg1)
{
    stream.sockopt.tcpFastOpen = arg1 == Qt::Checked;
    GEN_JSON
}
void ConnectionEditWindow::on_tProxyCB_currentIndexChanged(const QString &arg1)
{
    stream.sockopt.tproxy = arg1.toStdString();
    GEN_JSON
}
void ConnectionEditWindow::on_quicSecurityCB_currentTextChanged(const QString &arg1)
{
    stream.quicSettings.security = arg1.toStdString();
    GEN_JSON
}
void ConnectionEditWindow::on_quicKeyTxt_textEdited(const QString &arg1)
{
    stream.quicSettings.key = arg1.toStdString();
    GEN_JSON
}
void ConnectionEditWindow::on_quicHeaderTypeCB_currentIndexChanged(const QString &arg1)
{
    stream.quicSettings.header.type = arg1.toStdString();
    GEN_JSON
}
void ConnectionEditWindow::on_tcpRequestPrettifyBtn_clicked()
{
    try {
        auto tcpReqObject = StructFromJSONString<TransferSettingObjects::HTTPRequestObject>(ui->tcpRequestTxt->toPlainText());
        auto tcpReqObjectStr = StructToJSONString(tcpReqObject);
        ui->tcpRequestTxt->setPlainText(tcpReqObjectStr);
        GEN_JSON
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
        GEN_JSON
    } catch (...) {
        QvMessageBox(this, tr("#JsonPrettify"), tr("#JsonContainsError"));
    }
}
void ConnectionEditWindow::on_tcpHeaderTypeCB_currentIndexChanged(const QString &arg1)
{
    stream.tcpSettings.header.type = arg1.toStdString();
    GEN_JSON
}
void ConnectionEditWindow::on_wsPathTxt_textEdited(const QString &arg1)
{
    stream.wsSettings.path = arg1.toStdString();
    GEN_JSON
}
void ConnectionEditWindow::on_kcpMTU_valueChanged(int arg1)
{
    stream.kcpSettings.mtu = arg1;
    GEN_JSON
}
void ConnectionEditWindow::on_kcpTTI_valueChanged(int arg1)
{
    stream.kcpSettings.tti  = arg1;
    GEN_JSON
}
void ConnectionEditWindow::on_kcpUploadCapacSB_valueChanged(int arg1)
{
    stream.kcpSettings.uplinkCapacity = arg1;
    GEN_JSON
}
void ConnectionEditWindow::on_kcpCongestionCB_stateChanged(int arg1)
{
    stream.kcpSettings.congestion = arg1 == Qt::Checked;
    GEN_JSON
}
void ConnectionEditWindow::on_kcpDownCapacitySB_valueChanged(int arg1)
{
    stream.kcpSettings.downlinkCapacity = arg1;
    GEN_JSON
}
void ConnectionEditWindow::on_kcpReadBufferSB_valueChanged(int arg1)
{
    stream.kcpSettings.readBufferSize = arg1;
    GEN_JSON
}
void ConnectionEditWindow::on_kcpWriteBufferSB_valueChanged(int arg1)
{
    stream.kcpSettings.writeBufferSize = arg1;
    GEN_JSON
}
void ConnectionEditWindow::on_kcpHeaderType_currentTextChanged(const QString &arg1)
{
    stream.kcpSettings.header.type = arg1.toStdString();
    GEN_JSON
}
void ConnectionEditWindow::on_tranportCombo_currentIndexChanged(int index)
{
    ui->v2rayStackView->setCurrentIndex(index);
    GEN_JSON
}
void ConnectionEditWindow::on_dsPathTxt_textEdited(const QString &arg1)
{
    stream.dsSettings.path = arg1.toStdString();
    GEN_JSON
}
void ConnectionEditWindow::on_finalJson_textChanged()
{
    //try {
    //    auto jsonRoot = JSONFromString(ui->finalJson->toPlainText());
    //    vmess = StructFromJSONString<VMessServerObject>(JSONToString(jsonRoot["settings"].toObject()));
    //    stream = StructFromJSONString<StreamSettingsObject>(JSONToString(jsonRoot["streamSettings"].toObject()));
    //    BLACK(finalJson)
    //    rootJsonCursorPos = ui->finalJson->textCursor().position();
    //    ReLoad_GUI_JSON_ModelContent();
    //    ui->finalJson->textCursor().setPosition(rootJsonCursorPos);
    //} catch (...) {
    //    RED(finalJson)
    //}
}
void ConnectionEditWindow::on_outBoundTypeCombo_currentIndexChanged(int index)
{
    ui->outboundTypeStackView->setCurrentIndex(index);
    OutboundType = ui->outBoundTypeCombo->currentText().toLower();
    GEN_JSON
}

void ConnectionEditWindow::on_ss_emailTxt_textEdited(const QString &arg1)
{
    shadowsocks.email = arg1.toStdString();
    GEN_JSON
}

void ConnectionEditWindow::on_ss_passwordTxt_textEdited(const QString &arg1)
{
    shadowsocks.password = arg1.toStdString();
    GEN_JSON
}

void ConnectionEditWindow::on_ss_encryptionMethod_currentIndexChanged(const QString &arg1)
{
    shadowsocks.method = arg1.toStdString();
    GEN_JSON
}

void ConnectionEditWindow::on_ss_levelSpin_valueChanged(int arg1)
{
    shadowsocks.level = arg1;
    GEN_JSON
}

void ConnectionEditWindow::on_ss_otaCheckBox_stateChanged(int arg1)
{
    shadowsocks.ota = arg1 == Qt::Checked;
    GEN_JSON
}
