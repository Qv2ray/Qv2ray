#include "DnsSettingsWidget.hpp"

#include "components/geosite/QvGeositeReader.hpp"
#include "ui/widgets/QvAutoCompleteTextEdit.hpp"

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
    //
    domainsLayout->addWidget(domainListTxt);
    expectedIPsLayout->addWidget(ipListTxt);
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
    for (const auto &server : dns.servers)
    {
        serversListbox->addItem(server.QV2RAY_DNS_NAME);
    }
    if (serversListbox->count() > 0)
    {
        currentServerIndex = 0;
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
}

void DnsSettingsWidget::ShowCurrentDnsServerDetails()
{
    serverAddressTxt->setText(dns.servers[currentServerIndex].address);
    detailsSetingsGB->setChecked(!dns.servers[currentServerIndex].QV2RAY_DNS_IS_PURE_ADDRESS);
    //
    domainListTxt->setPlainText(dns.servers[currentServerIndex].domains.join(NEWLINE));
    ipListTxt->setPlainText(dns.servers[currentServerIndex].expectIPs.join(NEWLINE));
    //
    serverPortSB->setValue(dns.servers[currentServerIndex].port);
}

DNSObject DnsSettingsWidget::GetDNSObject() const
{
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
    o.QV2RAY_DNS_NAME = tr("New DNS");
    dns.servers.push_back(o);
    serversListbox->addItem(o.QV2RAY_DNS_NAME);
    currentServerIndex = serversListbox->count() - 1;
    ShowCurrentDnsServerDetails();
}

void DnsSettingsWidget::on_removeServerBtn_clicked()
{
    serversListbox->removeItemWidget(serversListbox->currentItem());
    currentServerIndex = 0;
    if (serversListbox->count() > 0)
    {
    }
}

void DnsSettingsWidget::on_detailsSetingsGB_clicked(bool checked)
{
    dns.servers[currentServerIndex].QV2RAY_DNS_IS_PURE_ADDRESS = !checked;
}

void DnsSettingsWidget::on_serversListbox_currentRowChanged(int currentRow)
{
    currentServerIndex = currentRow;
    ShowCurrentDnsServerDetails();
}
