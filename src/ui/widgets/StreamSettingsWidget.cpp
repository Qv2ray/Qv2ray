#include "StreamSettingsWidget.hpp"
#include "common/QvHelpers.hpp"
#include "ui/editors/w_JsonEditor.hpp"

StreamSettingsWidget::StreamSettingsWidget(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);
}

StreamSettingsObject StreamSettingsWidget::GetStreamSettings()
{
    return stream;
}

void StreamSettingsWidget::SetStreamObject(StreamSettingsObject sso)
{
    stream = sso;
    //
    transportCombo->setCurrentText(stream.network);
    tlsCB->setChecked(stream.security == "tls");
    // TCP
    tcpHeaderTypeCB->setCurrentText(stream.tcpSettings.header.type);
    tcpRequestTxt->setPlainText(StructToJsonString(stream.tcpSettings.header.request));
    tcpRespTxt->setPlainText(StructToJsonString(stream.tcpSettings.header.response));
    // HTTP
    QString allHosts;

    for (auto host : stream.httpSettings.host) {
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
}


void StreamSettingsWidget::on_transportCombo_currentIndexChanged(int index)
{
    v2rayStackView->setCurrentIndex(index);
}

void StreamSettingsWidget::on_httpPathTxt_textEdited(const QString &arg1)
{
    stream.httpSettings.path = arg1;
}

void StreamSettingsWidget::on_httpHostTxt_textChanged()
{
    try {
        QStringList hosts = httpHostTxt->toPlainText().replace("\r", "").split("\n");
        stream.httpSettings.host.clear();

        for (auto host : hosts) {
            if (!host.trimmed().isEmpty()) {
                stream.httpSettings.host.push_back(host.trimmed());
            }
        }

        BLACK(httpHostTxt)
    } catch (...) {
        RED(httpHostTxt)
    }
}

void StreamSettingsWidget::on_wsHeadersTxt_textChanged()
{
    try {
        QStringList headers = SplitLines(wsHeadersTxt->toPlainText());
        stream.wsSettings.headers.clear();

        for (auto header : headers) {
            if (header.isEmpty()) continue;

            auto index = header.indexOf("|");

            if (index < 0) throw "fast fail to set RED color";

            auto key = header.left(index);
            auto value = header.right(header.length() - index - 1);
            stream.wsSettings.headers[key] = value;
        }

        BLACK(wsHeadersTxt)
    } catch (...) {
        RED(wsHeadersTxt)
    }
}


void StreamSettingsWidget::on_tcpRequestDefBtn_clicked()
{
    tcpRequestTxt->clear();
    tcpRequestTxt->setPlainText("{\"version\":\"1.1\",\"method\":\"GET\",\"path\":[\"/\"],\"headers\":"
                                "{\"Host\":[\"www.baidu.com\",\"www.bing.com\"],\"User-Agent\":"
                                "[\"Mozilla/5.0 (Windows NT 10.0; WOW64) "
                                "AppleWebKit/537.36 (KHTML, like Gecko) Chrome/53.0.2785.143 Safari/537.36\","
                                "\"Mozilla/5.0 (iPhone; CPU iPhone OS 10_0_2 like Mac OS X) "
                                "AppleWebKit/601.1 (KHTML, like Gecko) CriOS/53.0.2785.109 Mobile/14A456 "
                                "Safari/601.1.46\"],\"Accept-Encoding\":[\"gzip, deflate\"],"
                                "\"Connection\":[\"keep-alive\"],\"Pragma\":\"no-cache\"}}");
}

void StreamSettingsWidget::on_tcpRespDefBtn_clicked()
{
    tcpRespTxt->clear();
    tcpRespTxt->setPlainText("{\"version\":\"1.1\",\"status\":\"200\",\"reason\":\"OK\",\"headers\":{\"Content-Type\":[\"application/octet-stream\",\"video/mpeg\"],\"Transfer-Encoding\":[\"chunked\"],\"Connection\":[\"keep-alive\"],\"Pragma\":\"no-cache\"}}");
}

void StreamSettingsWidget::on_tlsCB_stateChanged(int arg1)
{
    stream.security = arg1 == Qt::Checked ? "tls" : "none";
}

void StreamSettingsWidget::on_soMarkSpinBox_valueChanged(int arg1)
{
    stream.sockopt.mark = arg1;
}

void StreamSettingsWidget::on_tcpFastOpenCB_stateChanged(int arg1)
{
    stream.sockopt.tcpFastOpen = arg1 == Qt::Checked;
}

void StreamSettingsWidget::on_tProxyCB_currentIndexChanged(const QString &arg1)
{
    stream.sockopt.tproxy = arg1;
}

void StreamSettingsWidget::on_quicSecurityCB_currentTextChanged(const QString &arg1)
{
    stream.quicSettings.security = arg1;
}

void StreamSettingsWidget::on_quicKeyTxt_textEdited(const QString &arg1)
{
    stream.quicSettings.key = arg1;
}

void StreamSettingsWidget::on_quicHeaderTypeCB_currentIndexChanged(const QString &arg1)
{
    stream.quicSettings.header.type = arg1;
}

void StreamSettingsWidget::on_tcpHeaderTypeCB_currentIndexChanged(const QString &arg1)
{
    stream.tcpSettings.header.type = arg1;
}

void StreamSettingsWidget::on_wsPathTxt_textEdited(const QString &arg1)
{
    stream.wsSettings.path = arg1;
}

void StreamSettingsWidget::on_kcpMTU_valueChanged(int arg1)
{
    stream.kcpSettings.mtu = arg1;
}

void StreamSettingsWidget::on_kcpTTI_valueChanged(int arg1)
{
    stream.kcpSettings.tti  = arg1;
}

void StreamSettingsWidget::on_kcpUploadCapacSB_valueChanged(int arg1)
{
    stream.kcpSettings.uplinkCapacity = arg1;
}

void StreamSettingsWidget::on_kcpCongestionCB_stateChanged(int arg1)
{
    stream.kcpSettings.congestion = arg1 == Qt::Checked;
}

void StreamSettingsWidget::on_kcpDownCapacitySB_valueChanged(int arg1)
{
    stream.kcpSettings.downlinkCapacity = arg1;
}

void StreamSettingsWidget::on_kcpReadBufferSB_valueChanged(int arg1)
{
    stream.kcpSettings.readBufferSize = arg1;
}

void StreamSettingsWidget::on_kcpWriteBufferSB_valueChanged(int arg1)
{
    stream.kcpSettings.writeBufferSize = arg1;
}

void StreamSettingsWidget::on_kcpHeaderType_currentTextChanged(const QString &arg1)
{
    stream.kcpSettings.header.type = arg1;
}

void StreamSettingsWidget::on_dsPathTxt_textEdited(const QString &arg1)
{
    stream.dsSettings.path = arg1;
}

void StreamSettingsWidget::on_tcpRequestEditBtn_clicked()
{
    JsonEditor w(JsonFromString(tcpRequestTxt->toPlainText()), this);
    auto rString = JsonToString(w.OpenEditor());
    tcpRequestTxt->setPlainText(rString);
    auto tcpReqObject = StructFromJsonString<HTTPRequestObject>(rString);
    stream.tcpSettings.header.request = tcpReqObject;
}

void StreamSettingsWidget::on_tcpResponseEditBtn_clicked()
{
    JsonEditor w(JsonFromString(tcpRespTxt->toPlainText()), this);
    auto rString = JsonToString(w.OpenEditor());
    tcpRespTxt->setPlainText(rString);
    auto tcpRspObject = StructFromJsonString<HTTPResponseObject>(rString);
    stream.tcpSettings.header.response = tcpRspObject;
}

void StreamSettingsWidget::on_transportCombo_currentIndexChanged(const QString &arg1)
{
    stream.network = arg1;
}

void StreamSettingsWidget::on_serverNameTxt_textEdited(const QString &arg1)
{
    stream.tlsSettings.serverName = arg1;
}

void StreamSettingsWidget::on_allowInsecureCB_stateChanged(int arg1)
{
    stream.tlsSettings.allowInsecure = arg1 == Qt::Checked;
}

void StreamSettingsWidget::on_alpnTxt_textChanged()
{
    stream.tlsSettings.alpn = SplitLines(alpnTxt->toPlainText());
}
