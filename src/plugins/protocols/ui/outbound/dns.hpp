#pragma once

#include "QvGUIPluginInterface.hpp"
#include "ui_dns.h"

class DnsOutboundEditor
    : public Qv2rayPlugin::QvPluginEditor
    , private Ui::dnsOutEditor
{
    Q_OBJECT

  public:
    explicit DnsOutboundEditor(QWidget *parent = nullptr);

    void SetHostAddress(const QString &, int) override{};
    QPair<QString, int> GetHostAddress() const override
    {
        return {};
    };

    void SetContent(const QJsonObject &_content) override
    {
        this->content = _content;
        PLUGIN_EDITOR_LOADING_SCOPE({
            if (content.contains("network"))
            {
                tcpCB->setChecked(content["network"] == "tcp");
                udpCB->setChecked(content["network"] == "udp");
            }
            else
            {
                originalCB->setChecked(true);
            }
            if (content.contains("address"))
                addressTxt->setText(content["address"].toString());
            if (content.contains("port"))
                portSB->setValue(content["port"].toInt());
        })
    };
    const QJsonObject GetContent() const override
    {
        return content;
    };

  protected:
    void changeEvent(QEvent *e) override;

  private slots:
    void on_tcpCB_clicked();
    void on_udpCB_clicked();
    void on_originalCB_clicked();
    void on_addressTxt_textEdited(const QString &arg1);
    void on_portSB_valueChanged(int arg1);
};
