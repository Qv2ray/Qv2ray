#include <QDebug>
#include <QFile>
#include <QIntValidator>
#include <iostream>

#include "w_OutboundEditor.hpp"
#include "w_MainWindow.hpp"
#include "w_JsonEditor.hpp"
#include "w_RoutesEditor.hpp"

OutboundEditor::OutboundEditor(QWidget *parent)
    : QDialog(parent),
      Tag(""),
      Mux(),
      stream(),
      vmess(),
      shadowsocks()
{
    setupUi(this);
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

OutboundEditor::OutboundEditor(OUTBOUND outboundEntry, QWidget *parent)
    : OutboundEditor(parent)
{
    Original = outboundEntry;
    Tag = outboundEntry["tag"].toString();
    tagTxt->setText(Tag);
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
}

OUTBOUND OutboundEditor::OpenEditor()
{
    int resultCode = this->exec();
    return resultCode == QDialog::Accepted ? Result : Original;
}

QString OutboundEditor::GetFriendlyName()
{
    auto host = ipLineEdit->text().replace(":", "-").replace("/", "_").replace("\\", "_");
    auto port = portLineEdit->text().replace(":", "-").replace("/", "_").replace("\\", "_");
    auto type = OutboundType;
    QString name = Tag.isEmpty() ? host + "-[" + port + "]-" + type : Tag;
    return name;
}

void OutboundEditor::ReLoad_GUI_JSON_ModelContent()
{
    if (OutboundType == "vmess") {
        outBoundTypeCombo->setCurrentIndex(0);
        ipLineEdit->setText(vmess.address);
        portLineEdit->setText(QString::number(vmess.port));
        idLineEdit->setText(vmess.users.front().id);
        alterLineEdit->setValue(vmess.users.front().alterId);
        securityCombo->setCurrentText(vmess.users.front().security);
        tranportCombo->setCurrentText(stream.network);
        tlsCB->setChecked(stream.security == "tls");
        // TCP
        tcpHeaderTypeCB->setCurrentText(stream.tcpSettings.header.type);
        tcpRequestTxt->setPlainText(StructToJsonString(stream.tcpSettings.header.request));
        tcpRespTxt->setPlainText(StructToJsonString(stream.tcpSettings.header.response));
        // HTTP
        QString allHosts;

        foreach (auto host, stream.httpSettings.host) {
            allHosts = allHosts + host + "\r\n";
        }

        httpHostTxt->setPlainText(allHosts);
        httpPathTxt->setText(stream.httpSettings.path);
        // WS
        wsPathTxt->setText(stream.wsSettings.path);
        QString wsHeaders;

        for (auto item = stream.wsSettings.headers.begin(); item != stream.wsSettings.headers.end(); item++) {
            wsHeaders += item.key() + "|" + item.value() + NEWLINE;
        }

        wsHeadersTxt->setPlainText(wsHeaders);
        // mKCP
        kcpMTU->setValue(stream.kcpSettings.mtu);
        kcpTTI->setValue(stream.kcpSettings.tti);
        kcpHeaderType->setCurrentText(stream.kcpSettings.header.type);
        kcpCongestionCB->setChecked(stream.kcpSettings.congestion);
        kcpReadBufferSB->setValue(stream.kcpSettings.readBufferSize);
        kcpUploadCapacSB->setValue(stream.kcpSettings.uplinkCapacity);
        kcpDownCapacitySB->setValue(stream.kcpSettings.downlinkCapacity);
        kcpWriteBufferSB->setValue(stream.kcpSettings.writeBufferSize);
        // DS
        dsPathTxt->setText(stream.dsSettings.path);
        // QUIC
        quicKeyTxt->setText(stream.quicSettings.key);
        quicSecurityCB->setCurrentText(stream.quicSettings.security);
        quicHeaderTypeCB->setCurrentText(stream.quicSettings.header.type);
        // SOCKOPT
        tProxyCB->setCurrentText(stream.sockopt.tproxy);
        tcpFastOpenCB->setChecked(stream.sockopt.tcpFastOpen);
        soMarkSpinBox->setValue(stream.sockopt.mark);
    } else if (OutboundType == "shadowsocks") {
        outBoundTypeCombo->setCurrentIndex(1);
        // ShadowSocks Configs
        ipLineEdit->setText(shadowsocks.address);
        portLineEdit->setText(QString::number(shadowsocks.port));
        ss_emailTxt->setText(shadowsocks.email);
        ss_levelSpin->setValue(shadowsocks.level);
        ss_otaCheckBox->setChecked(shadowsocks.ota);
        ss_passwordTxt->setText(shadowsocks.password);
        ss_encryptionMethod->setCurrentText(shadowsocks.method);
    } else if (OutboundType == "socks") {
        outBoundTypeCombo->setCurrentIndex(2);
        ipLineEdit->setText(socks.address);
        portLineEdit->setText(QString::number(socks.port));

        if (socks.users.empty()) socks.users.push_back(SocksServerObject::UserObject());

        socks_PasswordTxt->setText(socks.users.front().pass);
        socks_UserNameTxt->setText(socks.users.front().user);
    }

    muxEnabledCB->setChecked(Mux["enabled"].toBool());
    muxConcurrencyTxt->setValue(Mux["concurrency"].toInt());
}


void OutboundEditor::on_buttonBox_accepted()
{
    Result = GenerateConnectionJson();
}

void OutboundEditor::on_ipLineEdit_textEdited(const QString &arg1)
{
    vmess.address = arg1;
    shadowsocks.address = arg1;
    socks.address = arg1;
}

void OutboundEditor::on_portLineEdit_textEdited(const QString &arg1)
{
    if (arg1 != "") {
        vmess.port = arg1.toInt();
        shadowsocks.port = arg1.toInt();
        socks.port = arg1.toInt();
    }
}

void OutboundEditor::on_idLineEdit_textEdited(const QString &arg1)
{
    if (vmess.users.empty()) vmess.users.push_back(VMessServerObject::UserObject());

    vmess.users.front().id = arg1;
}

void OutboundEditor::on_securityCombo_currentIndexChanged(const QString &arg1)
{
    if (vmess.users.empty()) vmess.users.push_back(VMessServerObject::UserObject());

    vmess.users.front().security = arg1;
}

void OutboundEditor::on_tranportCombo_currentIndexChanged(const QString &arg1)
{
    stream.network = arg1;
}

void OutboundEditor::on_httpPathTxt_textEdited(const QString &arg1)
{
    stream.httpSettings.path = arg1;
}

void OutboundEditor::on_httpHostTxt_textChanged()
{
    try {
        QStringList hosts = httpHostTxt->toPlainText().replace("\r", "").split("\n");
        stream.httpSettings.host.clear();

        foreach (auto host, hosts) {
            if (host.trimmed() != "")
                stream.httpSettings.host.push_back(host.trimmed());
        }

        BLACK(httpHostTxt)
    } catch (...) {
        RED(httpHostTxt)
    }
}

void OutboundEditor::on_wsHeadersTxt_textChanged()
{
    try {
        QStringList headers = wsHeadersTxt->toPlainText().replace("\r", "").split("\n");
        stream.wsSettings.headers.clear();

        foreach (auto header, headers) {
            if (header.isEmpty()) continue;

            auto index = header.indexOf("|");

            if (index < 0) throw "fast fail to set RED color";

            stream.wsSettings.headers[header.left(index)] = header.right(index + 1);
        }

        BLACK(wsHeadersTxt)
    } catch (...) {
        RED(wsHeadersTxt)
    }
}


void OutboundEditor::on_tcpRequestDefBtn_clicked()
{
    tcpRequestTxt->clear();
    tcpRequestTxt->insertPlainText("{\"version\":\"1.1\",\"method\":\"GET\",\"path\":[\"/\"],\"headers\":"
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
    tcpRespTxt->clear();
    tcpRespTxt->insertPlainText("{\"version\":\"1.1\",\"status\":\"200\",\"reason\":\"OK\",\"headers\":{\"Content-Type\":[\"application/octet-stream\",\"video/mpeg\"],\"Transfer-Encoding\":[\"chunked\"],\"Connection\":[\"keep-alive\"],\"Pragma\":\"no-cache\"}}");
}

OUTBOUND OutboundEditor::GenerateConnectionJson()
{
    OUTBOUNDSETTING settings;
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
    stream.sockopt.tproxy = arg1;
}
void OutboundEditor::on_quicSecurityCB_currentTextChanged(const QString &arg1)
{
    stream.quicSettings.security = arg1;
}
void OutboundEditor::on_quicKeyTxt_textEdited(const QString &arg1)
{
    stream.quicSettings.key = arg1;
}
void OutboundEditor::on_quicHeaderTypeCB_currentIndexChanged(const QString &arg1)
{
    stream.quicSettings.header.type = arg1;
}
void OutboundEditor::on_tcpHeaderTypeCB_currentIndexChanged(const QString &arg1)
{
    stream.tcpSettings.header.type = arg1;
}
void OutboundEditor::on_wsPathTxt_textEdited(const QString &arg1)
{
    stream.wsSettings.path = arg1;
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
    stream.kcpSettings.header.type = arg1;
}
void OutboundEditor::on_tranportCombo_currentIndexChanged(int index)
{
    v2rayStackView->setCurrentIndex(index);
}
void OutboundEditor::on_dsPathTxt_textEdited(const QString &arg1)
{
    stream.dsSettings.path = arg1;
}
void OutboundEditor::on_outBoundTypeCombo_currentIndexChanged(int index)
{
    outboundTypeStackView->setCurrentIndex(index);
    OutboundType = outBoundTypeCombo->currentText().toLower();
}

void OutboundEditor::on_ss_emailTxt_textEdited(const QString &arg1)
{
    shadowsocks.email = arg1;
}

void OutboundEditor::on_ss_passwordTxt_textEdited(const QString &arg1)
{
    shadowsocks.password = arg1;
}

void OutboundEditor::on_ss_encryptionMethod_currentIndexChanged(const QString &arg1)
{
    shadowsocks.method = arg1;
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
    socks.users.front().user = arg1;
}

void OutboundEditor::on_socks_PasswordTxt_textEdited(const QString &arg1)
{
    socks.users.front().pass = arg1;
}

void OutboundEditor::on_tcpRequestEditBtn_clicked()
{
    JsonEditor *w = new JsonEditor(JsonFromString(tcpRequestTxt->toPlainText()), this);
    auto rString = JsonToString(w->OpenEditor());
    tcpRequestTxt->setPlainText(rString);
    auto tcpReqObject = StructFromJsonString<TSObjects::HTTPRequestObject>(rString);
    stream.tcpSettings.header.request = tcpReqObject;
    delete w;
}

void OutboundEditor::on_tcpResponseEditBtn_clicked()
{
    JsonEditor *w = new JsonEditor(JsonFromString(tcpRespTxt->toPlainText()), this);
    auto rString = JsonToString(w->OpenEditor());
    tcpRespTxt->setPlainText(rString);
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

void OutboundEditor::on_alterLineEdit_valueChanged(int arg1)
{
    if (vmess.users.empty()) vmess.users.push_back(VMessServerObject::UserObject());

    vmess.users.front().alterId = arg1;
}
