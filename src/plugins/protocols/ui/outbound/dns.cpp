#include "dns.hpp"

DnsOutboundEditor::DnsOutboundEditor(QWidget *parent) : Qv2rayPlugin::Gui::PluginProtocolEditor(parent)
{
    setupUi(this);
    setProperty("QV2RAY_INTERNAL_HAS_STREAMSETTINGS", false);
}

void DnsOutboundEditor::Load()
{
    isLoading = true;
    if (settings.contains("network"))
    {
        tcpCB->setChecked(settings["network"] == "tcp");
        udpCB->setChecked(settings["network"] == "udp");
    }
    else
    {
        originalCB->setChecked(true);
    }
    if (settings.contains("address"))
        addressTxt->setText(settings["address"].toString());
    if (settings.contains("port"))
        portSB->setValue(settings["port"].toInt());
    isLoading = false;
}

void DnsOutboundEditor::Store()
{
}

void DnsOutboundEditor::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type())
    {
        case QEvent::LanguageChange: retranslateUi(this); break;
        default: break;
    }
}

void DnsOutboundEditor::on_tcpCB_clicked()
{
    settings["network"] = "tcp";
}

void DnsOutboundEditor::on_udpCB_clicked()
{
    settings["network"] = "udp";
}

void DnsOutboundEditor::on_originalCB_clicked()
{
    settings.remove("network");
}

void DnsOutboundEditor::on_addressTxt_textEdited(const QString &arg1)
{
    if (arg1.isEmpty())
        settings.remove("network");
    else
        settings["network"] = arg1;
}

void DnsOutboundEditor::on_portSB_valueChanged(int arg1)
{
    if (arg1 < 0)
        settings.remove("port");
    else
        settings["port"] = arg1;
}
