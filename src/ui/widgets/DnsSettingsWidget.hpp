#pragma once

#include "base/Qv2rayBase.hpp"
#include "ui/messaging/QvMessageBus.hpp"
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
    void SetDNSObject(const DNSObject &dns);
    DNSObject GetDNSObject() const;

  private slots:
    void on_dnsClientIPTxt_textEdited(const QString &arg1);

    void on_dnsTagTxt_textEdited(const QString &arg1);

    void on_addServerBtn_clicked();

    void on_removeServerBtn_clicked();

    void on_detailsSetingsGB_clicked(bool checked);

    void on_serversListbox_currentRowChanged(int currentRow);

  private:
    void ShowCurrentDnsServerDetails();
    QvMessageBusSlotDecl;
    DNSObject dns;
    int currentServerIndex;
    //
    Qv2ray::ui::widgets::AutoCompleteTextEdit *domainListTxt;
    Qv2ray::ui::widgets::AutoCompleteTextEdit *ipListTxt;
};
