#pragma once

#include "CommonTypes.hpp"
#include "QvGUIPluginInterface.hpp"
#include "ui_shadowsocks.h"

class ShadowsocksOutboundEditor
    : public Qv2rayPlugin::QvPluginEditor
    , private Ui::shadowsocksOutEditor
{
    Q_OBJECT

  public:
    explicit ShadowsocksOutboundEditor(QWidget *parent = nullptr);

    void SetHostAddress(const QString &addr, int port) override
    {
        shadowsocks.set_address(addr);
        shadowsocks.set_port(port);
    };
    QPair<QString, int> GetHostAddress() const override
    {
        return { shadowsocks.address, shadowsocks.port };
    };

    void SetContent(const QJsonObject &content) override
    {
        if (content["servers"].toArray().isEmpty())
            content["servers"] = QJsonArray{ QJsonObject{} };
        QJS_CLEAR_BINDINGS
        shadowsocks.loadJson(content["servers"].toArray().first().toObject());
        QJS_RWBINDING(shadowsocks, method, ss_encryptionMethod, currentText, &QComboBox::currentTextChanged)
        QJS_RWBINDING(shadowsocks, password, ss_passwordTxt, text, &QLineEdit::textEdited)
    }
    const QJsonObject GetContent() const override
    {
        auto result = content;
        QJsonArray servers;
        servers.append(shadowsocks.toJson());
        result.insert("servers", servers);
        return result;
    }

  protected:
    void changeEvent(QEvent *e) override;

  private:
    ShadowSocksServerObject shadowsocks;
    QJS_BINDING_HELPERS
};
