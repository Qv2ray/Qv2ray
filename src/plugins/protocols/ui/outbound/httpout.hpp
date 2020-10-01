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
        http.address = server;
        http.port = port;
    }

    QPair<QString, int> GetHostAddress() const override
    {
        return { http.address, http.port };
    }

    void SetContent(const QJsonObject &source) override
    {
        auto servers = source["servers"].toArray();
        if (servers.isEmpty())
            return;
        const auto content = servers.first().toObject();
        http.loadJson(content);
        PLUGIN_EDITOR_LOADING_SCOPE({
            if (http.users.isEmpty())
                http.users.push_back({});
            http_UserNameTxt->setText(http.users.first().user);
            http_PasswordTxt->setText(http.users.first().pass);
        })
    }

    const QJsonObject GetContent() const override
    {
        auto result = http.toJson();
        if (http.users.isEmpty() || (http.users.first().user.isEmpty() && http.users.first().pass.isEmpty()))
            result.remove("users");
        return QJsonObject{ { "servers", QJsonArray{ result } } };
    }

  protected:
    void changeEvent(QEvent *e) override;

  private slots:
    void on_http_PasswordTxt_textEdited(const QString &arg1);
    void on_http_UserNameTxt_textEdited(const QString &arg1);

  private:
    HttpServerObject http;
};
