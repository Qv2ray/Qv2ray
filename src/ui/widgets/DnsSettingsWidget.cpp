#include "DnsSettingsWidget.hpp"

#include "common/QvHelpers.hpp"
#include "components/geosite/QvGeositeReader.hpp"
#include "ui/widgets/QvAutoCompleteTextEdit.hpp"

#define UPDATEUI                                                                                                                                \
    detailsSettingsGB->setEnabled(serversListbox->count() > 0);                                                                                 \
    serverAddressTxt->setEnabled(serversListbox->count() > 0);                                                                                  \
    removeServerBtn->setEnabled(serversListbox->count() > 0);
#define currentServerIndex serversListbox->currentRow()
DnsSettingsWidget::DnsSettingsWidget(QWidget *parent) : QWidget(parent)
{
    setupUi(this);
    QvMessageBusConnect(DnsSettingsWidget);
    //
    auto sourceStringsDomain = ReadGeoSiteFromFile(GlobalConfig.kernelConfig.AssetsPath() + "/geosite.dat");
    auto sourceStringsIP = ReadGeoSiteFromFile(GlobalConfig.kernelConfig.AssetsPath() + "/geoip.dat");
    //
    domainListTxt = new AutoCompleteTextEdit("geosite", sourceStringsDomain, this);
    ipListTxt = new AutoCompleteTextEdit("geoip", sourceStringsIP, this);
    connect(domainListTxt, &AutoCompleteTextEdit::textChanged,
            [&]() { this->dns.servers[currentServerIndex].domains = SplitLines(domainListTxt->toPlainText()); });
    connect(ipListTxt, &AutoCompleteTextEdit::textChanged,
            [&]() { this->dns.servers[currentServerIndex].expectIPs = SplitLines(ipListTxt->toPlainText()); });

    //
    domainsLayout->addWidget(domainListTxt);
    expectedIPsLayout->addWidget(ipListTxt);
    detailsSettingsGB->setCheckable(true);
    detailsSettingsGB->setChecked(false);
    UPDATEUI
}

QvMessageBusSlotImpl(DnsSettingsWidget)
{
    switch (msg)
    {
        MBRetranslateDefaultImpl;
        case HIDE_WINDOWS:
        case SHOW_WINDOWS:
        case UPDATE_COLORSCHEME: break;
    }
}

void DnsSettingsWidget::SetDNSObject(const DNSObject &_dns)
{
    this->dns = _dns;
    //
    dnsClientIPTxt->setText(dns.clientIp);
    dnsTagTxt->setText(dns.tag);
    serversListbox->clear();
    for (const auto &server : dns.servers)
    {
        serversListbox->addItem(server.address);
    }
    if (serversListbox->count() > 0)
    {
        serversListbox->setCurrentRow(0);
        ShowCurrentDnsServerDetails();
    }
    staticResolvedDomainsTable->clearContents();
    for (const auto &[host, ip] : dns.hosts.toStdMap())
    {
        const auto rowId = staticResolvedDomainsTable->rowCount();
        staticResolvedDomainsTable->insertRow(rowId);
        staticResolvedDomainsTable->setItem(rowId, 0, new QTableWidgetItem(host));
        staticResolvedDomainsTable->setItem(rowId, 1, new QTableWidgetItem(ip));
    }
    UPDATEUI
}

void DnsSettingsWidget::ShowCurrentDnsServerDetails()
{
    serverAddressTxt->setText(dns.servers[currentServerIndex].address);
    //
    domainListTxt->setPlainText(dns.servers[currentServerIndex].domains.join(NEWLINE));
    ipListTxt->setPlainText(dns.servers[currentServerIndex].expectIPs.join(NEWLINE));
    //
    serverPortSB->setValue(dns.servers[currentServerIndex].port);
    detailsSettingsGB->setChecked(dns.servers[currentServerIndex].QV2RAY_DNS_IS_COMPLEX_DNS);
}

DNSObject DnsSettingsWidget::GetDNSObject()
{
    dns.hosts.clear();
    for (auto i = 0; i < staticResolvedDomainsTable->rowCount(); i++)
    {
        const auto &item1 = staticResolvedDomainsTable->item(i, 0);
        const auto &item2 = staticResolvedDomainsTable->item(i, 1);
        if (item1 && item2)
            dns.hosts[item1->text()] = item2->text();
    }
    return dns;
}

void DnsSettingsWidget::on_dnsClientIPTxt_textEdited(const QString &arg1)
{
    dns.clientIp = arg1;
}

void DnsSettingsWidget::on_dnsTagTxt_textEdited(const QString &arg1)
{
    dns.tag = arg1;
}
void DnsSettingsWidget::on_addServerBtn_clicked()
{
    DNSObject::DNSServerObject o;
    o.address = "1.1.1.1";
    o.port = 53;
    dns.servers.push_back(o);
    serversListbox->addItem(o.address);
    serversListbox->setCurrentRow(serversListbox->count() - 1);
    UPDATEUI
    ShowCurrentDnsServerDetails();
}

void DnsSettingsWidget::on_removeServerBtn_clicked()
{
    dns.servers.removeAt(currentServerIndex);
    // Block the signals
    serversListbox->blockSignals(true);
    auto item = serversListbox->item(currentServerIndex);
    serversListbox->removeItemWidget(item);
    delete item;
    serversListbox->blockSignals(false);
    UPDATEUI

    if (serversListbox->count() > 0)
    {
        if (currentServerIndex < 0)
            serversListbox->setCurrentRow(0);
        ShowCurrentDnsServerDetails();
    }
}

void DnsSettingsWidget::on_serversListbox_currentRowChanged(int currentRow)
{
    if (currentRow < 0)
        return;
    ShowCurrentDnsServerDetails();
}

void DnsSettingsWidget::on_serverAddressTxt_textEdited(const QString &arg1)
{
    dns.servers[currentServerIndex].address = arg1;
    serversListbox->currentItem()->setText(arg1);
}

void DnsSettingsWidget::on_serverPortSB_valueChanged(int arg1)
{
    dns.servers[currentServerIndex].port = arg1;
}

void DnsSettingsWidget::on_addStaticHostBtn_clicked()
{
    if (staticResolvedDomainsTable->rowCount() >= 0)
        staticResolvedDomainsTable->insertRow(staticResolvedDomainsTable->rowCount());
}

void DnsSettingsWidget::on_removeStaticHostBtn_clicked()
{
    if (staticResolvedDomainsTable->rowCount() >= 0)
        staticResolvedDomainsTable->removeRow(staticResolvedDomainsTable->currentRow());
}

void DnsSettingsWidget::on_detailsSettingsGB_toggled(bool arg1)
{
    if (currentServerIndex > 0)
        dns.servers[currentServerIndex].QV2RAY_DNS_IS_COMPLEX_DNS = arg1;
    // detailsSettingsGB->setChecked(dns.servers[currentServerIndex].QV2RAY_DNS_IS_COMPLEX_DNS);
}
