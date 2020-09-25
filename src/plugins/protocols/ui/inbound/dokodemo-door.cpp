#include "dokodemo-door.hpp"

DokodemoDoorInboundEditor::DokodemoDoorInboundEditor(QWidget *parent) : Qv2rayPlugin::QvPluginEditor(parent)
{
    setupUi(this);
    setProperty("QV2RAY_INTERNAL_HAS_STREAMSETTINGS", true);
}

void DokodemoDoorInboundEditor::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type())
    {
        case QEvent::LanguageChange: retranslateUi(this); break;
        default: break;
    }
}

void DokodemoDoorInboundEditor::SetContent(const QJsonObject &content)
{
    PLUGIN_EDITOR_LOADING_SCOPE({
        this->content = content;
        // Dokodemo-Door
        dokoFollowRedirectCB->setChecked(content["followRedirect"].toBool());
        dokoIPAddrTxt->setText(content["address"].toString());
        dokoPortSB->setValue(content["port"].toInt());
        dokoTimeoutSB->setValue(content["timeout"].toInt());
        dokoTCPCB->setChecked(content["network"].toString().contains("tcp"));
        dokoUDPCB->setChecked(content["network"].toString().contains("udp"));
    })
}
const QJsonObject DokodemoDoorInboundEditor::GetContent() const
{
    return content;
}

void DokodemoDoorInboundEditor::on_dokoIPAddrTxt_textEdited(const QString &arg1)
{
    PLUGIN_EDITOR_LOADING_GUARD
    content["address"] = arg1;
}

void DokodemoDoorInboundEditor::on_dokoPortSB_valueChanged(int arg1)
{
    PLUGIN_EDITOR_LOADING_GUARD
    content["port"] = arg1;
}

#define SET_NETWORK                                                                                                                                  \
    bool hasTCP = dokoTCPCB->checkState() == Qt::Checked;                                                                                            \
    bool hasUDP = dokoUDPCB->checkState() == Qt::Checked;                                                                                            \
    QStringList list;                                                                                                                                \
    if (hasTCP)                                                                                                                                      \
        list << "tcp";                                                                                                                               \
    if (hasUDP)                                                                                                                                      \
        list << "udp";                                                                                                                               \
    content["network"] = list.join(",")

void DokodemoDoorInboundEditor::on_dokoTCPCB_stateChanged(int arg1)
{
    PLUGIN_EDITOR_LOADING_GUARD
    Q_UNUSED(arg1)
    SET_NETWORK;
}

void DokodemoDoorInboundEditor::on_dokoUDPCB_stateChanged(int arg1)
{
    PLUGIN_EDITOR_LOADING_GUARD
    Q_UNUSED(arg1)
    SET_NETWORK;
}

void DokodemoDoorInboundEditor::on_dokoTimeoutSB_valueChanged(int arg1)
{
    PLUGIN_EDITOR_LOADING_GUARD
    content["timeout"] = arg1;
}

void DokodemoDoorInboundEditor::on_dokoFollowRedirectCB_stateChanged(int arg1)
{
    PLUGIN_EDITOR_LOADING_GUARD
    content["followRedirect"] = arg1 == Qt::Checked;
}
