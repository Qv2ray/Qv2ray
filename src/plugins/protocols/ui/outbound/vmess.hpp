#pragma once

#include "CommonTypes.hpp"
#include "QvGUIPluginInterface.hpp"
#include "ui_vmess.h"

class VmessOutboundEditor
    : public Qv2rayPlugin::QvPluginEditor
    , private Ui::vmessOutEditor
{
    Q_OBJECT

  public:
    explicit VmessOutboundEditor(QWidget *parent = nullptr);

    void SetHostAddress(const QString &addr, int port) override
    {
        vmess.set_address(addr);
        vmess.set_port(port);
    }

    QPair<QString, int> GetHostAddress() const override
    {
        return { vmess.address(), vmess.port() };
    }

    void SetContent(const QJsonObject &content) override;
    const QJsonObject GetContent() const override
    {
        auto result = content;
        QJsonArray vnext;
        vnext.append(vmess.toJson());
        result.insert("vnext", vnext);
        return result;
    }

  private:
    VMessServerObject vmess;
    QJS_BINDING_HELPERS

  protected:
    void changeEvent(QEvent *e) override;
};
