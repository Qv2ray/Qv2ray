#include "DnsSettingsWidget.hpp"

#include "common/QvHelpers.hpp"
#include "components/geosite/QvGeositeReader.hpp"
#include "ui/widgets/QvAutoCompleteTextEdit.hpp"

using Qv2ray::common::validation::IsIPv4Address;
using Qv2ray::common::validation::IsIPv6Address;
using Qv2ray::common::validation::IsValidDNSServer;
using Qv2ray::common::validation::IsValidIPAddress;

#define CHECK_DISABLE_MOVE_BTN                                                                                                                  \
    if (serversListbox->count() <= 1)                                                                                                           \
    {                                                                                                                                           \
        moveServerUpBtn->setEnabled(false);                                                                                                     \
        moveServerDownBtn->setEnabled(false);                                                                                                   \
    }

#define UPDATEUI                                                                                                                                \
    detailsSettingsGB->setEnabled(serversListbox->count() > 0);                                                                                 \
    serverAddressTxt->setEnabled(serversListbox->count() > 0);                                                                                  \
    removeServerBtn->setEnabled(serversListbox->count() > 0);                                                                                   \
    ProcessDnsPortEnabledState();                                                                                                               \
    CHECK_DISABLE_MOVE_BTN

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
    staticResolvedDomainsTable->resizeColumnsToContents();
    UPDATEUI
}

bool DnsSettingsWidget::CheckIsValidDNS() const
{
    if (!dns.clientIp.isEmpty() && !IsValidIPAddress(dns.clientIp))
        return false;
    for (const auto &server : dns.servers)
    {
        if (!IsValidDNSServer(server.address))
            return false;
    }
    return true;
}

void DnsSettingsWidget::ProcessDnsPortEnabledState()
{
    if (detailsSettingsGB->isChecked())
    {
        if (const auto addr = serverAddressTxt->text(); addr.startsWith("https:") || addr.startsWith("https+"))
        {
            serverPortSB->setEnabled(false);
        }
        else
        {
            serverPortSB->setEnabled(true);
        }
    }
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
    //
    if (serverAddressTxt->text().isEmpty() || IsValidDNSServer(serverAddressTxt->text()))
    {
        BLACK(serverAddressTxt)
    }
    else
    {
        RED(serverAddressTxt)
    }
    ProcessDnsPortEnabledState();
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

    moveServerUpBtn->setEnabled(true);
    moveServerDownBtn->setEnabled(true);
    if (currentRow == 0)
    {
        moveServerUpBtn->setEnabled(false);
    }
    if (currentRow == serversListbox->count() - 1)
    {
        moveServerDownBtn->setEnabled(false);
    }

    ShowCurrentDnsServerDetails();
}

void DnsSettingsWidget::on_moveServerUpBtn_clicked()
{
    auto temp = dns.servers[currentServerIndex - 1];
    dns.servers[currentServerIndex - 1] = dns.servers[currentServerIndex];
    dns.servers[currentServerIndex] = temp;

    serversListbox->currentItem()->setText(dns.servers[currentServerIndex].address);
    serversListbox->setCurrentRow(currentServerIndex - 1);
    serversListbox->currentItem()->setText(dns.servers[currentServerIndex].address);
}

void DnsSettingsWidget::on_moveServerDownBtn_clicked()
{
    auto temp = dns.servers[currentServerIndex + 1];
    dns.servers[currentServerIndex + 1] = dns.servers[currentServerIndex];
    dns.servers[currentServerIndex] = temp;

    serversListbox->currentItem()->setText(dns.servers[currentServerIndex].address);
    serversListbox->setCurrentRow(currentServerIndex + 1);
    serversListbox->currentItem()->setText(dns.servers[currentServerIndex].address);
}

void DnsSettingsWidget::on_serverAddressTxt_textEdited(const QString &arg1)
{
    dns.servers[currentServerIndex].address = arg1;
    serversListbox->currentItem()->setText(arg1);
    if (arg1.isEmpty() || IsValidDNSServer(arg1))
    {
        BLACK(serverAddressTxt)
    }
    else
    {
        RED(serverAddressTxt)
    }

    ProcessDnsPortEnabledState();
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
    staticResolvedDomainsTable->resizeColumnsToContents();
}

void DnsSettingsWidget::on_staticResolvedDomainsTable_cellChanged(int, int)
{
    staticResolvedDomainsTable->resizeColumnsToContents();
}

void DnsSettingsWidget::on_detailsSettingsGB_toggled(bool arg1)
{
    if (currentServerIndex >= 0)
        dns.servers[currentServerIndex].QV2RAY_DNS_IS_COMPLEX_DNS = arg1;
    // detailsSettingsGB->setChecked(dns.servers[currentServerIndex].QV2RAY_DNS_IS_COMPLEX_DNS);
}
