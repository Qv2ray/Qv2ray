#pragma once

#include "CommonTypes.hpp"
#include "QvGUIPluginInterface.hpp"
#include "ui_httpout.h"

class HttpOutboundEditor
    : public Qv2rayPlugin::QvPluginEditor
    , private Ui::httpOutEditor
{
    Q_OBJECT

  public:
    explicit HttpOutboundEditor(QWidget *parent = nullptr);

    void SetHostAddress(const QString &server, int port) override
    {
        http.set_address(server);
        http.set_port(port);
    }

    QPair<QString, int> GetHostAddress() const override
    {
        return { http.address(), http.port() };
    }

    void SetContent(const QJsonObject &source) override
    {
        auto servers = source["servers"].toArray();
        if (servers.isEmpty())
            return;
        const auto content = servers.first().toObject();

        QJS_CLEAR_BINDINGS

        http.loadJson(content);
        if (http.users().isEmpty())
            http.users().push_back({});

        QJS_RWBINDING(http.users().first(), user, http_UserNameTxt, text, &QLineEdit::textEdited)
        QJS_RWBINDING(http.users().first(), pass, http_PasswordTxt, text, &QLineEdit::textEdited)
    }

    const QJsonObject GetContent() const override
    {
        auto result = http.toJson();
        if (http.users().isEmpty() || (http.users().first().user().isEmpty() && http.users().first().pass().isEmpty()))
            result.remove("users");
        return QJsonObject{ { "servers", QJsonArray{ result } } };
    }

  protected:
    void changeEvent(QEvent *e) override;

  private:
    HttpServerObject http;
    QJS_BINDING_HELPERS
};
