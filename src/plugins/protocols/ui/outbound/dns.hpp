#pragma once

#include "QvPlugin/Gui/QvGUIPluginInterface.hpp"
#include "ui_dns.h"

class DnsOutboundEditor
    : public Qv2rayPlugin::Gui::PluginProtocolEditor
    , private Ui::dnsOutEditor
{
    Q_OBJECT

  public:
    explicit DnsOutboundEditor(QWidget *parent = nullptr);

    virtual void Load() override;
    virtual void Store() override;

  protected:
    void changeEvent(QEvent *e) override;

  private slots:
    void on_tcpCB_clicked();
    void on_udpCB_clicked();
    void on_originalCB_clicked();
    void on_addressTxt_textEdited(const QString &arg1);
    void on_portSB_valueChanged(int arg1);

  private:
    bool isLoading = false;
};
