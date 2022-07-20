#include "DnsSettingsWidget.hpp"

#include "GeositeReader/GeositeReader.hpp"
#include "Qv2rayBase/Common/Utils.hpp"
#include "ui/WidgetUIBase.hpp"
#include "ui/widgets/AutoCompleteTextEdit.hpp"

#define CHECK_DISABLE_MOVE_BTN                                                                                                                                           \
    if (serversListbox->count() <= 1)                                                                                                                                    \
    {                                                                                                                                                                    \
        moveServerUpBtn->setEnabled(false);                                                                                                                              \
        moveServerDownBtn->setEnabled(false);                                                                                                                            \
    }

#define UPDATE_UI_ENABLED_STATE                                                                                                                                          \
    detailsSettingsGB->setEnabled(serversListbox->count() > 0);                                                                                                          \
    serverAddressTxt->setEnabled(serversListbox->count() > 0);                                                                                                           \
    removeServerBtn->setEnabled(serversListbox->count() > 0);                                                                                                            \
    ProcessDnsPortEnabledState();                                                                                                                                        \
    CHECK_DISABLE_MOVE_BTN

#define currentServerIndex serversListbox->currentRow()

void DnsSettingsWidget::updateColorScheme()
{
    addServerBtn->setIcon(QIcon(STYLE_RESX("add")));
    removeServerBtn->setIcon(QIcon(STYLE_RESX("minus")));
    moveServerUpBtn->setIcon(QIcon(STYLE_RESX("arrow-up")));
    moveServerDownBtn->setIcon(QIcon(STYLE_RESX("arrow-down")));
    addStaticHostBtn->setIcon(QIcon(STYLE_RESX("add")));
    removeStaticHostBtn->setIcon(QIcon(STYLE_RESX("minus")));
}

DnsSettingsWidget::DnsSettingsWidget(QWidget *parent) : QWidget(parent)
{
    setupUi(this);
    QvMessageBusConnect();

    auto sourceStringsIP = GeositeReader::ReadGeoSiteFromFile(GlobalConfig->behaviorConfig->GeoIPPath);
    auto sourceStringsDomain = GeositeReader::ReadGeoSiteFromFile(GlobalConfig->behaviorConfig->GeoSitePath);

    domainListTxt = new AutoCompleteTextEdit("geosite", sourceStringsDomain, this);
    ipListTxt = new AutoCompleteTextEdit("geoip", sourceStringsIP, this);
    connect(domainListTxt, &AutoCompleteTextEdit::textChanged, [&]() { (*dns.servers)[currentServerIndex].domains = SplitLines(domainListTxt->toPlainText()); });
    connect(ipListTxt, &AutoCompleteTextEdit::textChanged, [&]() { (*dns.servers)[currentServerIndex].expectIPs = SplitLines(ipListTxt->toPlainText()); });

    domainsLayout->addWidget(domainListTxt);
    expectedIPsLayout->addWidget(ipListTxt);
    detailsSettingsGB->setCheckable(true);
    detailsSettingsGB->setChecked(false);
    UPDATE_UI_ENABLED_STATE;
    updateColorScheme();
}

QvMessageBusSlotImpl(DnsSettingsWidget)
{
    switch (msg)
    {
        case MessageBus::UPDATE_COLORSCHEME:
        {
            updateColorScheme();
            break;
        }
    }
}

void DnsSettingsWidget::SetDNSObject(const Qv2ray::Models::V2RayDNSObject &_dns, const QList<Qv2ray::Models::V2RayFakeDNSObject> &_fakeDNS)
{
    this->dns = _dns;

    dnsClientIPTxt->setText(dns.clientIp);
    dnsTagTxt->setText(dns.tag);

    serversListbox->clear();
    std::for_each(dns.servers->begin(), dns.servers->end(), [&](const auto &dns) { serversListbox->addItem(dns.address); });

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

    dnsQueryStrategyCB->setCurrentText(dns.queryStrategy);
    dnsDisableFallbackCB->setChecked(dns.disableFallback);
    dnsDisableCacheCB->setChecked(dns.disableCache);

    // WARNING TODO: BAD HACK need list model
    if (!_fakeDNS.isEmpty())
    {
        fakeDNSIPPool->setCurrentText(_fakeDNS.first().ipPool);
        fakeDNSIPPoolSize->setValue(_fakeDNS.first().poolSize);
        if (_fakeDNS.size() > 1)
        {
            fakeDNSIPv6Pool->setCurrentText(_fakeDNS.at(1).ipPool);
            fakeDNSIPv6PoolSize->setValue(_fakeDNS.at(1).poolSize);
        }
    }
    UPDATE_UI_ENABLED_STATE
}

bool DnsSettingsWidget::CheckIsValidDNS() const
{
    if (!dns.clientIp->isEmpty() && !IsValidIPAddress(dns.clientIp))
        return false;
    for (const auto &server : *dns.servers)
    {
        if (!IsValidV2RayDNSServer(server.address))
            return false;
    }
    return true;
}

void DnsSettingsWidget::ProcessDnsPortEnabledState()
{
    if (detailsSettingsGB->isChecked())
    {
        const auto isDoHDoT = serverAddressTxt->text().startsWith("https:") || serverAddressTxt->text().startsWith("https+");
        serverPortSB->setEnabled(!isDoHDoT);
    }
}

void DnsSettingsWidget::ShowCurrentDnsServerDetails()
{
    serverAddressTxt->setText((*dns.servers)[currentServerIndex].address);
    //
    domainListTxt->setPlainText((*dns.servers)[currentServerIndex].domains->join('\n'));
    ipListTxt->setPlainText((*dns.servers)[currentServerIndex].expectIPs->join('\n'));
    //
    serverPortSB->setValue((*dns.servers)[currentServerIndex].port);
    detailsSettingsGB->setChecked((*dns.servers)[currentServerIndex].QV2RAY_DNS_IS_COMPLEX_DNS);
    //
    if (serverAddressTxt->text().isEmpty() || IsValidV2RayDNSServer(serverAddressTxt->text()))
    {
        BLACK(serverAddressTxt);
    }
    else
    {
        RED(serverAddressTxt);
    }
    ProcessDnsPortEnabledState();
}

std::pair<Qv2ray::Models::V2RayDNSObject, QList<Qv2ray::Models::V2RayFakeDNSObject>> DnsSettingsWidget::GetDNSObject()
{
    dns.hosts.clear();
    for (auto i = 0; i < staticResolvedDomainsTable->rowCount(); i++)
    {
        const auto &item1 = staticResolvedDomainsTable->item(i, 0);
        const auto &item2 = staticResolvedDomainsTable->item(i, 1);
        if (item1 && item2)
            dns.hosts.insert(item1->text(), item2->text());
    }

    QList<Qv2ray::Models::V2RayFakeDNSObject> fakeDNS;
    if (!fakeDNSIPPool->currentText().isEmpty())
        fakeDNS.append(V2RayFakeDNSObject{ { fakeDNSIPPool->currentText() }, fakeDNSIPPoolSize->value() });
    if (!fakeDNSIPv6Pool->currentText().isEmpty())
        fakeDNS.append(V2RayFakeDNSObject{ { fakeDNSIPv6Pool->currentText() }, fakeDNSIPv6PoolSize->value() });
    return { dns, fakeDNS };
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
    Qv2ray::Models::V2RayDNSObject::V2RayDNSServerObject o;
    o.address = u"1.1.1.1"_qs;
    o.port = 53;
    dns.servers->push_back(o);
    serversListbox->addItem(o.address);
    serversListbox->setCurrentRow(serversListbox->count() - 1);
    UPDATE_UI_ENABLED_STATE
    ShowCurrentDnsServerDetails();
}

void DnsSettingsWidget::on_removeServerBtn_clicked()
{
    dns.servers->removeAt(currentServerIndex);
    // Block the signals
    serversListbox->blockSignals(true);
    auto item = serversListbox->item(currentServerIndex);
    serversListbox->removeItemWidget(item);
    delete item;
    serversListbox->blockSignals(false);
    UPDATE_UI_ENABLED_STATE

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
    auto temp = (*dns.servers)[currentServerIndex - 1];
    (*dns.servers)[currentServerIndex - 1] = (*dns.servers)[currentServerIndex];
    (*dns.servers)[currentServerIndex] = temp;

    serversListbox->currentItem()->setText((*dns.servers)[currentServerIndex].address);
    serversListbox->setCurrentRow(currentServerIndex - 1);
    serversListbox->currentItem()->setText((*dns.servers)[currentServerIndex].address);
}

void DnsSettingsWidget::on_moveServerDownBtn_clicked()
{
    auto temp = (*dns.servers)[currentServerIndex + 1];
    (*dns.servers)[currentServerIndex + 1] = (*dns.servers)[currentServerIndex];
    (*dns.servers)[currentServerIndex] = temp;

    serversListbox->currentItem()->setText((*dns.servers)[currentServerIndex].address);
    serversListbox->setCurrentRow(currentServerIndex + 1);
    serversListbox->currentItem()->setText((*dns.servers)[currentServerIndex].address);
}

void DnsSettingsWidget::on_serverAddressTxt_textEdited(const QString &arg1)
{
    (*dns.servers)[currentServerIndex].address = arg1;
    serversListbox->currentItem()->setText(arg1);
    if (arg1.isEmpty() || IsValidV2RayDNSServer(arg1))
    {
        BLACK(serverAddressTxt);
    }
    else
    {
        RED(serverAddressTxt);
    }

    ProcessDnsPortEnabledState();
}

void DnsSettingsWidget::on_serverPortSB_valueChanged(int arg1)
{
    (*dns.servers)[currentServerIndex].port = arg1;
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
        (*dns.servers)[currentServerIndex].QV2RAY_DNS_IS_COMPLEX_DNS = arg1;
    // detailsSettingsGB->setChecked(dns.servers[currentServerIndex].QV2RAY_DNS_IS_COMPLEX_DNS);
}

void DnsSettingsWidget::on_dnsDisableCacheCB_stateChanged(int arg1)
{
    dns.disableCache = arg1 == Qt::Checked;
}

void DnsSettingsWidget::on_dnsDisableFallbackCB_stateChanged(int arg1)
{
    dns.disableFallback = arg1 == Qt::Checked;
}

void DnsSettingsWidget::on_dnsQueryStrategyCB_currentTextChanged(const QString &arg1)
{
    dns.queryStrategy = arg1;
}
