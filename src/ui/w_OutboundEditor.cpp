#include <QDebug>
#include <QFile>
#include <QIntValidator>
#include <iostream>

#include "w_OutboundEditor.hpp"
#include "w_MainWindow.hpp"
#include "w_JsonEditor.hpp"

OutboundEditor::OutboundEditor(QWidget *parent)
    : QDialog(parent),
      Tag(""),
      Mux(),
      ui(new Ui::OutboundEditor),
      stream(),
      vmess(),
      shadowsocks()
{
    ui->setupUi(this);
    ui->portLineEdit->setValidator(new QIntValidator());
    ui->alterLineEdit->setValidator(new QIntValidator());
    shadowsocks = ShadowSocksServerObject();
    socks = SocksServerObject();
    socks.users.push_back(SocksServerObject::UserObject());
    vmess = VMessServerObject();
    vmess.users.push_back(VMessServerObject::UserObject());
    stream = StreamSettingsObject();
    OutboundType = "vmess";
    Tag = OUTBOUND_TAG_PROXY;
    ReLoad_GUI_JSON_ModelContent();
    Result = GenerateConnectionJson();
}

OutboundEditor::OutboundEditor(QJsonObject outboundEntry, QWidget *parent)
    : OutboundEditor(parent)
{
    Original = outboundEntry;
    Tag = outboundEntry["tag"].toString();
    ui->tagTxt->setText(Tag);
    OutboundType = outboundEntry["protocol"].toString();
    Mux = outboundEntry["mux"].toObject();

    if (OutboundType == "vmess") {
        vmess = StructFromJsonString<VMessServerObject>(JsonToString(outboundEntry["settings"].toObject()["vnext"].toArray().first().toObject()));
        stream = StructFromJsonString<StreamSettingsObject>(JsonToString(outboundEntry["streamSettings"].toObject()));
        shadowsocks.port = vmess.port;
        shadowsocks.address = vmess.address;
        socks.address = vmess.address;
        socks.port = vmess.port;
    } else if (OutboundType == "shadowsocks") {
        shadowsocks = StructFromJsonString<ShadowSocksServerObject>(JsonToString(outboundEntry["settings"].toObject()["servers"].toArray().first().toObject()));
        vmess.address = shadowsocks.address;
        vmess.port = shadowsocks.port;
        socks.address = shadowsocks.address;
        socks.port = shadowsocks.port;
    } else if (OutboundType == "socks") {
        socks = StructFromJsonString<SocksServerObject>(JsonToString(outboundEntry["settings"].toObject()["servers"].toArray().first().toObject()));
        vmess.address = socks.address;
        vmess.port = socks.port;
        shadowsocks.address = socks.address;
        shadowsocks.port = socks.port;
    }

    ReLoad_GUI_JSON_ModelContent();
    Result = GenerateConnectionJson();
}


OutboundEditor::~OutboundEditor()
{
    delete ui;
}

QJsonObject OutboundEditor::OpenEditor()
{
    int resultCode = this->exec();
    return resultCode == QDialog::Accepted ? Result : Original;
}

QString OutboundEditor::GetFriendlyName()
{
    auto host = ui->ipLineEdit->text().replace(":", "-").replace("/", "_").replace("\\", "_");
    auto port = ui->portLineEdit->text().replace(":", "-").replace("/", "_").replace("\\", "_");
    auto type = OutboundType;
    QString name = Tag.isEmpty() ? host + "-[" + port + "]-" + type : Tag;
    return name;
}

void OutboundEditor::ReLoad_GUI_JSON_ModelContent()
{
    if (OutboundType == "vmess") {
        ui->outBoundTypeCombo->setCurrentIndex(0);
        ui->ipLineEdit->setText(QSTRING(vmess.address));
        ui->portLineEdit->setText(QString::number(vmess.port));
        ui->idLineEdit->setText(QSTRING(vmess.users.front().id));
        ui->alterLineEdit->setText(QString::number(vmess.users.front().alterId));
        ui->securityCombo->setCurrentText(QSTRING(vmess.users.front().security));
        ui->tranportCombo->setCurrentText(QSTRING(stream.network));
        ui->tlsCB->setChecked(stream.security == "tls");
        // TCP
        ui->tcpHeaderTypeCB->setCurrentText(QSTRING(stream.tcpSettings.header.type));
        ui->tcpRequestTxt->setPlainText(StructToJsonString(stream.tcpSettings.header.request));
        ui->tcpRespTxt->setPlainText(StructToJsonString(stream.tcpSettings.header.response));
        // HTTP
        QString allHosts;

        foreach (auto host, stream.httpSettings.host) {
            allHosts = allHosts + QSTRING(host) + "\r\n";
        }

        ui->httpHostTxt->setPlainText(allHosts);
        ui->httpPathTxt->setText(QSTRING(stream.httpSettings.path));
        // WS
        ui->wsPathTxt->setText(QSTRING(stream.wsSettings.path));
        QString wsHeaders = std::accumulate(stream.wsSettings.headers.begin(), stream.wsSettings.headers.end(), QString(), [](QString in1, const pair<string, string> &in2) {
            in1 += QSTRING(in2.first + "|" + in2.second) + "\r\n";
            return in1;
        });
        ui->wsHeadersTxt->setPlainText(wsHeaders);
        // mKCP
        ui->kcpMTU->setValue(stream.kcpSettings.mtu);
        ui->kcpTTI->setValue(stream.kcpSettings.tti);
        ui->kcpHeaderType->setCurrentText(QSTRING(stream.kcpSettings.header.type));
        ui->kcpCongestionCB->setChecked(stream.kcpSettings.congestion);
        ui->kcpReadBufferSB->setValue(stream.kcpSettings.readBufferSize);
        ui->kcpUploadCapacSB->setValue(stream.kcpSettings.uplinkCapacity);
        ui->kcpDownCapacitySB->setValue(stream.kcpSettings.downlinkCapacity);
        ui->kcpWriteBufferSB->setValue(stream.kcpSettings.writeBufferSize);
        // DS
        ui->dsPathTxt->setText(QSTRING(stream.dsSettings.path));
        // QUIC
        ui->quicKeyTxt->setText(QSTRING(stream.quicSettings.key));
        ui->quicSecurityCB->setCurrentText(QSTRING(stream.quicSettings.security));
        ui->quicHeaderTypeCB->setCurrentText(QSTRING(stream.quicSettings.header.type));
        // SOCKOPT
        ui->tProxyCB->setCurrentText(QSTRING(stream.sockopt.tproxy));
        ui->tcpFastOpenCB->setChecked(stream.sockopt.tcpFastOpen);
        ui->soMarkSpinBox->setValue(stream.sockopt.mark);
    } else if (OutboundType == "shadowsocks") {
        ui->outBoundTypeCombo->setCurrentIndex(1);
        // ShadowSocks Configs
        ui->ipLineEdit->setText(QSTRING(shadowsocks.address));
        ui->portLineEdit->setText(QString::number(shadowsocks.port));
        ui->ss_emailTxt->setText(QSTRING(shadowsocks.email));
        ui->ss_levelSpin->setValue(shadowsocks.level);
        ui->ss_otaCheckBox->setChecked(shadowsocks.ota);
        ui->ss_passwordTxt->setText(QSTRING(shadowsocks.password));
        ui->ss_encryptionMethod->setCurrentText(QSTRING(shadowsocks.method));
    } else if (OutboundType == "socks") {
        ui->outBoundTypeCombo->setCurrentIndex(2);
        ui->ipLineEdit->setText(QSTRING(socks.address));
        ui->portLineEdit->setText(QString::number(socks.port));

        if (socks.users.empty()) socks.users.push_back(SocksServerObject::UserObject());

        ui->socks_PasswordTxt->setText(QSTRING(socks.users.front().pass));
        ui->socks_UserNameTxt->setText(QSTRING(socks.users.front().user));
    }

    ui->muxEnabledCB->setChecked(Mux["enabled"].toBool());
    ui->muxConcurrencyTxt->setValue(Mux["concurrency"].toInt());
}


void OutboundEditor::on_buttonBox_accepted()
{
    Result = GenerateConnectionJson();
}

void OutboundEditor::on_ipLineEdit_textEdited(const QString &arg1)
{
    vmess.address = arg1.toStdString();
    shadowsocks.address = arg1.toStdString();
    socks.address = arg1.toStdString();
}

void OutboundEditor::on_portLineEdit_textEdited(const QString &arg1)
{
    if (arg1 != "") {
        vmess.port = stoi(arg1.toStdString());
        shadowsocks.port = stoi(arg1.toStdString());
        socks.port = stoi(arg1.toStdString());
    }
}

void OutboundEditor::on_idLineEdit_textEdited(const QString &arg1)
{
    if (vmess.users.empty()) vmess.users.push_back(VMessServerObject::UserObject());

    vmess.users.front().id = arg1.toStdString();
}

void OutboundEditor::on_alterLineEdit_textEdited(const QString &arg1)
{
    if (vmess.users.empty()) vmess.users.push_back(VMessServerObject::UserObject());

    vmess.users.front().alterId = stoi(arg1.toStdString());
}

void OutboundEditor::on_securityCombo_currentIndexChanged(const QString &arg1)
{
    if (vmess.users.empty()) vmess.users.push_back(VMessServerObject::UserObject());

    vmess.users.front().security = arg1.toStdString();
}

void OutboundEditor::on_tranportCombo_currentIndexChanged(const QString &arg1)
{
    stream.network = arg1.toStdString();
}

void OutboundEditor::on_httpPathTxt_textEdited(const QString &arg1)
{
    stream.httpSettings.path = arg1.toStdString();
}

void OutboundEditor::on_httpHostTxt_textChanged()
{
    try {
        QStringList hosts = ui->httpHostTxt->toPlainText().replace("\r", "").split("\n");
        stream.httpSettings.host.clear();

        foreach (auto host, hosts) {
            if (host.trimmed() != "")
                stream.httpSettings.host.push_back(host.trimmed().toStdString());
        }

        BLACK(httpHostTxt)
    } catch (...) {
        RED(httpHostTxt)
    }
}

void OutboundEditor::on_wsHeadersTxt_textChanged()
{
    try {
        QStringList headers = ui->wsHeadersTxt->toPlainText().replace("\r", "").split("\n");
        stream.wsSettings.headers.clear();

        foreach (auto header, headers) {
            if (header.isEmpty()) continue;

            auto content = header.split("|");

            if (content.length() < 2) throw "fast fail to set RED color";

            stream.wsSettings.headers.insert(make_pair(content[0].toStdString(), content[1].toStdString()));
        }

        BLACK(wsHeadersTxt)
    } catch (...) {
        RED(wsHeadersTxt)
    }
}


void OutboundEditor::on_tcpRequestDefBtn_clicked()
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
}

void OutboundEditor::on_tcpRespDefBtn_clicked()
{
    ui->tcpRespTxt->clear();
    ui->tcpRespTxt->insertPlainText("{\"version\":\"1.1\",\"status\":\"200\",\"reason\":\"OK\",\"headers\":{\"Content-Type\":[\"application/octet-stream\",\"video/mpeg\"],\"Transfer-Encoding\":[\"chunked\"],\"Connection\":[\"keep-alive\"],\"Pragma\":\"no-cache\"}}");
}

QJsonObject OutboundEditor::GenerateConnectionJson()
{
    QJsonObject settings;
    auto streaming = JsonFromString(StructToJsonString(stream));

    if (OutboundType == "vmess") {
        // VMess is only a ServerObject, and we need an array { "vnext": [] }
        QJsonArray vnext;
        vnext.append(GetRootObject(vmess));
        settings.insert("vnext", vnext);
    } else if (OutboundType == "shadowsocks") {
        streaming = QJsonObject();
        QJsonArray servers;
        servers.append(GetRootObject(shadowsocks));
        settings["servers"] = servers;
    } else if (OutboundType == "socks") {
        streaming = QJsonObject();
        QJsonArray servers;
        servers.append(GetRootObject(socks));
        settings["servers"] = servers;
    }

    auto root = GenerateOutboundEntry(OutboundType, settings, streaming, Mux, "0.0.0.0", Tag);
    return root;
}

void OutboundEditor::on_tlsCB_stateChanged(int arg1)
{
    stream.security = arg1 == Qt::Checked ? "tls" : "none";
}
void OutboundEditor::on_soMarkSpinBox_valueChanged(int arg1)
{
    stream.sockopt.mark = arg1;
}
void OutboundEditor::on_tcpFastOpenCB_stateChanged(int arg1)
{
    stream.sockopt.tcpFastOpen = arg1 == Qt::Checked;
}
void OutboundEditor::on_tProxyCB_currentIndexChanged(const QString &arg1)
{
    stream.sockopt.tproxy = arg1.toStdString();
}
void OutboundEditor::on_quicSecurityCB_currentTextChanged(const QString &arg1)
{
    stream.quicSettings.security = arg1.toStdString();
}
void OutboundEditor::on_quicKeyTxt_textEdited(const QString &arg1)
{
    stream.quicSettings.key = arg1.toStdString();
}
void OutboundEditor::on_quicHeaderTypeCB_currentIndexChanged(const QString &arg1)
{
    stream.quicSettings.header.type = arg1.toStdString();
}
void OutboundEditor::on_tcpHeaderTypeCB_currentIndexChanged(const QString &arg1)
{
    stream.tcpSettings.header.type = arg1.toStdString();
}
void OutboundEditor::on_wsPathTxt_textEdited(const QString &arg1)
{
    stream.wsSettings.path = arg1.toStdString();
}
void OutboundEditor::on_kcpMTU_valueChanged(int arg1)
{
    stream.kcpSettings.mtu = arg1;
}
void OutboundEditor::on_kcpTTI_valueChanged(int arg1)
{
    stream.kcpSettings.tti  = arg1;
}
void OutboundEditor::on_kcpUploadCapacSB_valueChanged(int arg1)
{
    stream.kcpSettings.uplinkCapacity = arg1;
}
void OutboundEditor::on_kcpCongestionCB_stateChanged(int arg1)
{
    stream.kcpSettings.congestion = arg1 == Qt::Checked;
}
void OutboundEditor::on_kcpDownCapacitySB_valueChanged(int arg1)
{
    stream.kcpSettings.downlinkCapacity = arg1;
}
void OutboundEditor::on_kcpReadBufferSB_valueChanged(int arg1)
{
    stream.kcpSettings.readBufferSize = arg1;
}
void OutboundEditor::on_kcpWriteBufferSB_valueChanged(int arg1)
{
    stream.kcpSettings.writeBufferSize = arg1;
}
void OutboundEditor::on_kcpHeaderType_currentTextChanged(const QString &arg1)
{
    stream.kcpSettings.header.type = arg1.toStdString();
}
void OutboundEditor::on_tranportCombo_currentIndexChanged(int index)
{
    ui->v2rayStackView->setCurrentIndex(index);
}
void OutboundEditor::on_dsPathTxt_textEdited(const QString &arg1)
{
    stream.dsSettings.path = arg1.toStdString();
}
void OutboundEditor::on_outBoundTypeCombo_currentIndexChanged(int index)
{
    ui->outboundTypeStackView->setCurrentIndex(index);
    OutboundType = ui->outBoundTypeCombo->currentText().toLower();
}

void OutboundEditor::on_ss_emailTxt_textEdited(const QString &arg1)
{
    shadowsocks.email = arg1.toStdString();
}

void OutboundEditor::on_ss_passwordTxt_textEdited(const QString &arg1)
{
    shadowsocks.password = arg1.toStdString();
}

void OutboundEditor::on_ss_encryptionMethod_currentIndexChanged(const QString &arg1)
{
    shadowsocks.method = arg1.toStdString();
}

void OutboundEditor::on_ss_levelSpin_valueChanged(int arg1)
{
    shadowsocks.level = arg1;
}

void OutboundEditor::on_ss_otaCheckBox_stateChanged(int arg1)
{
    shadowsocks.ota = arg1 == Qt::Checked;
}

void OutboundEditor::on_socks_UserNameTxt_textEdited(const QString &arg1)
{
    socks.users.front().user = arg1.toStdString();
}

void OutboundEditor::on_socks_PasswordTxt_textEdited(const QString &arg1)
{
    socks.users.front().pass = arg1.toStdString();
}

void OutboundEditor::on_tcpRequestEditBtn_clicked()
{
    JsonEditor *w = new JsonEditor(JsonFromString(ui->tcpRequestTxt->toPlainText()), this);
    auto rString = JsonToString(w->OpenEditor());
    ui->tcpRequestTxt->setPlainText(rString);
    auto tcpReqObject = StructFromJsonString<TSObjects::HTTPRequestObject>(rString);
    stream.tcpSettings.header.request = tcpReqObject;
    delete w;
}

void OutboundEditor::on_tcpResponseEditBtn_clicked()
{
    JsonEditor *w = new JsonEditor(JsonFromString(ui->tcpRespTxt->toPlainText()), this);
    auto rString = JsonToString(w->OpenEditor());
    ui->tcpRespTxt->setPlainText(rString);
    auto tcpRspObject = StructFromJsonString<TSObjects::HTTPResponseObject>(rString);
    stream.tcpSettings.header.response = tcpRspObject;
    delete w;
}

void OutboundEditor::on_tagTxt_textEdited(const QString &arg1)
{
    Tag = arg1;
}

void OutboundEditor::on_muxEnabledCB_stateChanged(int arg1)
{
    Mux["enabled"] = arg1 == Qt::Checked;
}

void OutboundEditor::on_muxConcurrencyTxt_valueChanged(int arg1)
{
    Mux["concurrency"] = arg1;
}
