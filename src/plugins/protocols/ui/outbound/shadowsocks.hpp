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
        shadowsocks.address = addr;
        shadowsocks.port = port;
    };
    QPair<QString, int> GetHostAddress() const override
    {
        return { shadowsocks.address, shadowsocks.port };
    };

    void SetContent(const QJsonObject &content) override
    {
        PLUGIN_EDITOR_LOADING_SCOPE({
            if (content["servers"].toArray().isEmpty())
                content["servers"] = QJsonArray{ QJsonObject{} };
            // ShadowSocks Configs
            shadowsocks = ShadowSocksServerObject::fromJson(content["servers"].toArray().first().toObject());
            ss_passwordTxt->setText(shadowsocks.password);
            ss_encryptionMethod->setCurrentText(shadowsocks.method);
        })
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

  private slots:
    void on_ss_encryptionMethod_currentTextChanged(const QString &arg1);
    void on_ss_passwordTxt_textEdited(const QString &arg1);

  private:
    ShadowSocksServerObject shadowsocks;
};
