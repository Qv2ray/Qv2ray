#include "dokodemo-door.hpp"

DokodemoDoorInboundEditor::DokodemoDoorInboundEditor(QWidget *parent) : Qv2rayPlugin::Gui::PluginProtocolEditor(parent)
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

void DokodemoDoorInboundEditor::Load()
{
    isLoading = true;
    // Dokodemo-Door
    dokoFollowRedirectCB->setChecked(settings["followRedirect"].toBool());
    dokoIPAddrTxt->setText(settings["address"].toString());
    dokoPortSB->setValue(settings["port"].toInt());
    dokoTimeoutSB->setValue(settings["timeout"].toInt());
    dokoTCPCB->setChecked(settings["network"].toString().contains("tcp"));
    dokoUDPCB->setChecked(settings["network"].toString().contains("udp"));
    isLoading = false;
}
void DokodemoDoorInboundEditor::Store()
{
}

void DokodemoDoorInboundEditor::on_dokoIPAddrTxt_textEdited(const QString &arg1)
{
    settings["address"] = arg1;
}

void DokodemoDoorInboundEditor::on_dokoPortSB_valueChanged(int arg1)
{
    settings["port"] = arg1;
}

#define SET_NETWORK                                                                                                                                                      \
    bool hasTCP = dokoTCPCB->checkState() == Qt::Checked;                                                                                                                \
    bool hasUDP = dokoUDPCB->checkState() == Qt::Checked;                                                                                                                \
    QStringList list;                                                                                                                                                    \
    if (hasTCP)                                                                                                                                                          \
        list << "tcp";                                                                                                                                                   \
    if (hasUDP)                                                                                                                                                          \
        list << "udp";                                                                                                                                                   \
    settings["network"] = list.join(",")

void DokodemoDoorInboundEditor::on_dokoTCPCB_stateChanged(int arg1)
{
    Q_UNUSED(arg1)
    SET_NETWORK;
}

void DokodemoDoorInboundEditor::on_dokoUDPCB_stateChanged(int arg1)
{
    Q_UNUSED(arg1)
    SET_NETWORK;
}

void DokodemoDoorInboundEditor::on_dokoTimeoutSB_valueChanged(int arg1)
{
    settings["timeout"] = arg1;
}

void DokodemoDoorInboundEditor::on_dokoFollowRedirectCB_stateChanged(int arg1)
{
    settings["followRedirect"] = arg1 == Qt::Checked;
}
