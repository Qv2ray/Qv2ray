#include "StreamSettingsWidget.hpp"

#include "Qv2rayBase/Common/Utils.hpp"
#include "ui/WidgetUIBase.hpp"
#include "ui/windows/editors/w_JsonEditor.hpp"

#include <QStringBuilder>

StreamSettingsWidget::StreamSettingsWidget(QWidget *parent) : QWidget(parent)
{
    setupUi(this);
    stream.wsSettings->earlyDataHeaderName.ReadWriteBind(wsEarlyDataHeaderTxt, "text", &QLineEdit::textEdited);
    stream.sockopt->tcpKeepAliveInterval.ReadWriteBind(tcpKeepAliveIntervalSB, "value", &QSpinBox::valueChanged);
    stream.httpSettings->method.ReadWriteBind(httpMethodTxt, "text", &QLineEdit::textEdited);
}

Qv2ray::Models::StreamSettingsObject StreamSettingsWidget::GetStreamSettings() const
{
    return stream;
}

void StreamSettingsWidget::SetStreamObject(const Qv2ray::Models::StreamSettingsObject &sso)
{
    stream = sso;
    transportCombo->setCurrentText(stream.network);
    // TLS XTLS
    {
        const static QMap<QString, int> securityIndexMap{ { "none", 0 }, { "tls", 1 } };
        if (securityIndexMap.contains(stream.security))
            securityTypeCB->setCurrentIndex(securityIndexMap[stream.security]);
        else
            qInfo() << "Unsupported Security Type:" << stream.security;

        {
            serverNameTxt->setText(stream.tlsSettings->serverName);
            disableSessionResumptionCB->setChecked(stream.tlsSettings->disableSessionResumption);
            disableSystemRoot->setChecked(stream.tlsSettings->disableSystemRoot);
            alpnTxt->setText(stream.tlsSettings->alpn->join("|"));
        }
    }
    // TCP
    {
        tcpHeaderTypeCB->setCurrentText(stream.tcpSettings->header->type);
        tcpRequestTxt->setPlainText(JsonToString(stream.tcpSettings->header->request->toJson()));
        tcpRespTxt->setPlainText(JsonToString(stream.tcpSettings->header->response->toJson()));
    }
    // HTTP
    {
        httpHostTxt->setPlainText(stream.httpSettings->host->join(NEWLINE));
        httpPathTxt->setText(stream.httpSettings->path);
    }
    // WS
    {
        wsPathTxt->setText(stream.wsSettings->path);
        QString wsHeaders;
        for (const auto &[key, value] : stream.wsSettings->headers->toStdMap())
        {
            wsHeaders = wsHeaders % key % "|" % value % NEWLINE;
        }
        wsHeadersTxt->setPlainText(wsHeaders);
        wsEarlyDataSB->setValue(stream.wsSettings->maxEarlyData);
        wsBrowserForwardCB->setChecked(stream.wsSettings->useBrowserForwarding);
    }
    // mKCP
    {
        kcpMTU->setValue(stream.kcpSettings->mtu);
        kcpTTI->setValue(stream.kcpSettings->tti);
        kcpHeaderType->setCurrentText(stream.kcpSettings->header->type);
        kcpCongestionCB->setChecked(stream.kcpSettings->congestion);
        kcpReadBufferSB->setValue(stream.kcpSettings->readBufferSize);
        kcpUploadCapacSB->setValue(stream.kcpSettings->uplinkCapacity);
        kcpDownCapacitySB->setValue(stream.kcpSettings->downlinkCapacity);
        kcpWriteBufferSB->setValue(stream.kcpSettings->writeBufferSize);
        kcpSeedTxt->setText(stream.kcpSettings->seed);
    }
    // DS
    {
        dsPathTxt->setText(stream.dsSettings->path);
    }
    // QUIC
    {
        quicKeyTxt->setText(stream.quicSettings->key);
        quicSecurityCB->setCurrentText(stream.quicSettings->security);
        quicHeaderTypeCB->setCurrentText(stream.quicSettings->header->type);
    }
    // gRPC
    {
        grpcServiceNameTxt->setText(stream.grpcSettings->serviceName);
    }
    // SOCKOPT
    {
        tProxyCB->setCurrentText(stream.sockopt->tproxy);
        tcpFastOpenCB->setChecked(stream.sockopt->tcpFastOpen);
        soMarkSpinBox->setValue(stream.sockopt->mark);
    }
}

void StreamSettingsWidget::on_httpPathTxt_textEdited(const QString &arg1)
{
    stream.httpSettings->path = arg1;
}

void StreamSettingsWidget::on_httpHostTxt_textChanged()
{
    const auto hosts = httpHostTxt->toPlainText().replace("\r", "").split("\n");
    stream.httpSettings->host->clear();
    for (const auto &host : hosts)
    {
        if (!host.trimmed().isEmpty())
            stream.httpSettings->host->push_back(host.trimmed());
    }
}

void StreamSettingsWidget::on_wsHeadersTxt_textChanged()
{
    const auto headers = SplitLines(wsHeadersTxt->toPlainText());
    stream.wsSettings->headers->clear();
    for (const auto &header : headers)
    {
        if (header.isEmpty())
            continue;

        if (!header.contains("|"))
        {
            qInfo() << "Header missing '|' separator";
            RED(wsHeadersTxt);
            return;
        }

        const auto index = header.indexOf("|");
        auto key = header.left(index);
        auto value = header.right(header.length() - index - 1);
        stream.wsSettings->headers->insert(key, value);
    }
    BLACK(wsHeadersTxt);
}

void StreamSettingsWidget::on_tcpRequestDefBtn_clicked()
{
    tcpRequestTxt->clear();
    tcpRequestTxt->setPlainText(JsonToString(Qv2ray::Models::transfer::HTTPRequestObject().toJson()["headers"].toObject()));
}

void StreamSettingsWidget::on_tcpRespDefBtn_clicked()
{
    tcpRespTxt->clear();
    tcpRespTxt->setPlainText(JsonToString(Qv2ray::Models::transfer::HTTPResponseObject().toJson()["headers"].toObject()));
}

void StreamSettingsWidget::on_soMarkSpinBox_valueChanged(int arg1)
{
    stream.sockopt->mark = arg1;
}

void StreamSettingsWidget::on_tcpFastOpenCB_stateChanged(int arg1)
{
    stream.sockopt->tcpFastOpen = arg1 == Qt::Checked;
}

void StreamSettingsWidget::on_tProxyCB_currentIndexChanged(int arg1)
{
    stream.sockopt->tproxy = tProxyCB->itemText(arg1);
}

void StreamSettingsWidget::on_quicSecurityCB_currentIndexChanged(int arg1)
{
    stream.quicSettings->security = quicSecurityCB->itemText(arg1);
}

void StreamSettingsWidget::on_quicKeyTxt_textEdited(const QString &arg1)
{
    stream.quicSettings->key = arg1;
}

void StreamSettingsWidget::on_quicHeaderTypeCB_currentIndexChanged(int arg1)
{
    stream.quicSettings->header->type = quicHeaderTypeCB->itemText(arg1);
}

void StreamSettingsWidget::on_tcpHeaderTypeCB_currentIndexChanged(int arg1)
{
    stream.tcpSettings->header->type = tcpHeaderTypeCB->itemText(arg1);
}

void StreamSettingsWidget::on_wsPathTxt_textEdited(const QString &arg1)
{
    stream.wsSettings->path = arg1;
}

void StreamSettingsWidget::on_kcpMTU_valueChanged(int arg1)
{
    stream.kcpSettings->mtu = arg1;
}

void StreamSettingsWidget::on_kcpTTI_valueChanged(int arg1)
{
    stream.kcpSettings->tti = arg1;
}

void StreamSettingsWidget::on_kcpUploadCapacSB_valueChanged(int arg1)
{
    stream.kcpSettings->uplinkCapacity = arg1;
}

void StreamSettingsWidget::on_kcpCongestionCB_stateChanged(int arg1)
{
    stream.kcpSettings->congestion = arg1 == Qt::Checked;
}

void StreamSettingsWidget::on_kcpDownCapacitySB_valueChanged(int arg1)
{
    stream.kcpSettings->downlinkCapacity = arg1;
}

void StreamSettingsWidget::on_kcpReadBufferSB_valueChanged(int arg1)
{
    stream.kcpSettings->readBufferSize = arg1;
}

void StreamSettingsWidget::on_kcpWriteBufferSB_valueChanged(int arg1)
{
    stream.kcpSettings->writeBufferSize = arg1;
}

void StreamSettingsWidget::on_kcpHeaderType_currentIndexChanged(int arg1)
{
    stream.kcpSettings->header->type = kcpHeaderType->itemText(arg1);
}

void StreamSettingsWidget::on_kcpSeedTxt_textEdited(const QString &arg1)
{
    stream.kcpSettings->seed = arg1;
}

void StreamSettingsWidget::on_dsPathTxt_textEdited(const QString &arg1)
{
    stream.dsSettings->path = arg1;
}

void StreamSettingsWidget::on_tcpRequestEditBtn_clicked()
{
    JsonEditor w(JsonFromString(tcpRequestTxt->toPlainText()), this);
    auto rJson = w.OpenEditor();
    tcpRequestTxt->setPlainText(JsonToString(rJson));
    Qv2ray::Models::transfer::HTTPRequestObject tcpReqObject;
    tcpReqObject.loadJson(rJson);
    stream.tcpSettings->header->request = tcpReqObject;
}

void StreamSettingsWidget::on_tcpResponseEditBtn_clicked()
{
    JsonEditor w(JsonFromString(tcpRespTxt->toPlainText()), this);
    auto rJson = w.OpenEditor();
    tcpRespTxt->setPlainText(JsonToString(rJson));
    Qv2ray::Models::transfer::HTTPResponseObject tcpRspObject;
    tcpRspObject.loadJson(rJson);
    stream.tcpSettings->header->response = tcpRspObject;
}

void StreamSettingsWidget::on_transportCombo_currentIndexChanged(int arg1)
{
    stream.network = transportCombo->itemText(arg1);
    v2rayStackView->setCurrentIndex(arg1);
}

void StreamSettingsWidget::on_securityTypeCB_currentIndexChanged(int arg1)
{
    stream.security = securityTypeCB->itemText(arg1).toLower();
}

//
// Dirty hack, since XTLSSettings are the same as TLSSettings (Split them if required in the future)
//
void StreamSettingsWidget::on_serverNameTxt_textEdited(const QString &arg1)
{
    stream.tlsSettings->serverName = arg1.trimmed();
}

void StreamSettingsWidget::on_disableSessionResumptionCB_stateChanged(int arg1)
{
    stream.tlsSettings->disableSessionResumption = arg1 == Qt::Checked;
}

void StreamSettingsWidget::on_alpnTxt_textEdited(const QString &arg1)
{
    stream.tlsSettings->alpn = arg1.split('|', Qt::SplitBehaviorFlags::SkipEmptyParts);
}

void StreamSettingsWidget::on_disableSystemRoot_stateChanged(int arg1)
{
    stream.tlsSettings->disableSystemRoot = arg1;
}

void StreamSettingsWidget::on_openCertEditorBtn_clicked()
{
}

void StreamSettingsWidget::on_grpcServiceNameTxt_textEdited(const QString &arg1)
{
    stream.grpcSettings->serviceName = arg1;
}

void StreamSettingsWidget::on_wsEarlyDataSB_valueChanged(int arg1)
{
    stream.wsSettings->maxEarlyData = arg1;
}

void StreamSettingsWidget::on_wsBrowserForwardCB_stateChanged(int arg1)
{
    stream.wsSettings->useBrowserForwarding = arg1 == Qt::Checked;
}
