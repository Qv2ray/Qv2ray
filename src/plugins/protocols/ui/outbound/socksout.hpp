#pragma once

#include "QvGUIPluginInterface.hpp"
#include "common/CommonTypes.hpp"
#include "ui_socksout.h"

class SocksOutboundEditor
    : public Qv2rayPlugin::QvPluginEditor
    , private Ui::socksOutEditor
{
    Q_OBJECT

  public:
    explicit SocksOutboundEditor(QWidget *parent = nullptr);

    void SetHostAddress(const QString &server, int port) override
    {
        socks.address = server;
        socks.port = port;
    }

    QPair<QString, int> GetHostAddress() const override
    {
        return { socks.address, socks.port };
    }

    void SetContent(const QJsonObject &content) override
    {
        socks.loadJson(content);
        PLUGIN_EDITOR_LOADING_SCOPE({
            if (!socks.users.isEmpty())
            {
            }
        })
    }
    const QJsonObject GetContent() const override
    {
        auto result = socks.toJson();
        if (socks.users.isEmpty())
            result.remove("users");
        return result;
    }

  protected:
    void changeEvent(QEvent *e) override;

  private slots:
    void on_socks_UserNameTxt_textEdited(const QString &arg1);
    void on_socks_PasswordTxt_textEdited(const QString &arg1);

  private:
    SocksServerObject socks;
};
