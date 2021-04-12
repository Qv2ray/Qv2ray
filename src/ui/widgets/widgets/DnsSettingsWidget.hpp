#pragma once

#include "base/Qv2rayBase.hpp"
#include "ui/common/QvMessageBus.hpp"
#include "ui_DnsSettingsWidget.h"

namespace Qv2ray::ui::widgets
{
    class AutoCompleteTextEdit;
}

class DnsSettingsWidget
    : public QWidget
    , private Ui::DnsSettingsWidget
{
    Q_OBJECT

  public:
    explicit DnsSettingsWidget(QWidget *parent = nullptr);
    void SetDNSObject(const DNSObject &dns, const FakeDNSObject &fakeDNS);
    std::pair<DNSObject, FakeDNSObject> GetDNSObject();
    bool CheckIsValidDNS() const;

  private slots:
    void on_dnsClientIPTxt_textEdited(const QString &arg1);
    void on_dnsTagTxt_textEdited(const QString &arg1);
    void on_addServerBtn_clicked();
    void on_removeServerBtn_clicked();
    void on_serversListbox_currentRowChanged(int currentRow);
    void on_moveServerUpBtn_clicked();
    void on_moveServerDownBtn_clicked();
    void on_serverAddressTxt_textEdited(const QString &arg1);
    void on_serverPortSB_valueChanged(int arg1);
    void on_addStaticHostBtn_clicked();
    void on_removeStaticHostBtn_clicked();
    void on_detailsSettingsGB_toggled(bool arg1);
    void on_staticResolvedDomainsTable_cellChanged(int row, int column);

    void on_fakeDNSIPPool_currentTextChanged(const QString &arg1);

    void on_fakeDNSIPPoolSize_valueChanged(int arg1);

    void on_dnsDisableCacheCB_stateChanged(int arg1);

    void on_dnsDisableFallbackCB_stateChanged(int arg1);

    void on_dnsQueryStrategyCB_currentTextChanged(const QString &arg1);

  private:
    void updateColorScheme();
    void ShowCurrentDnsServerDetails();
    void ProcessDnsPortEnabledState();
    QvMessageBusSlotDecl;
    DNSObject dns;
    FakeDNSObject fakeDNS;
    // int currentServerIndex;
    //
    Qv2ray::ui::widgets::AutoCompleteTextEdit *domainListTxt;
    Qv2ray::ui::widgets::AutoCompleteTextEdit *ipListTxt;
};
