#pragma once

#include "CommonTypes.hpp"
#include "QvGUIPluginInterface.hpp"
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
        socks.set_address(server);
        socks.set_port(port);
    }

    QPair<QString, int> GetHostAddress() const override
    {
        return { socks.address(), socks.port() };
    }

    void SetContent(const QJsonObject &source) override
    {
        auto servers = source["servers"].toArray();
        if (servers.isEmpty())
            return;
        const auto content = servers.first().toObject();
        QJS_CLEAR_BINDINGS
        socks.loadJson(content);
        QJS_RWBINDING(socks.users().first(), user, socks_UserNameTxt, text, &QLineEdit::textEdited)
        QJS_RWBINDING(socks.users().first(), pass, socks_PasswordTxt, text, &QLineEdit::textEdited)
    }

    const QJsonObject GetContent() const override
    {
        auto result = socks.toJson();
        if (socks.users().isEmpty() || (socks.users().first().user().isEmpty() && socks.users().first().pass().isEmpty()))
            result.remove("users");
        return QJsonObject{ { "servers", QJsonArray{ result } } };
    }

  protected:
    void changeEvent(QEvent *e) override;

  private:
    SocksServerObject socks;
    QJS_BINDING_HELPERS
};
