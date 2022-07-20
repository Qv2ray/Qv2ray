#pragma once

#include "QvPlugin/Gui/QvGUIPluginInterface.hpp"
#include "V2RayModels.hpp"
#include "ui_httpout.h"

class HttpOutboundEditor
    : public Qv2rayPlugin::Gui::PluginProtocolEditor
    , private Ui::httpOutEditor
{
    Q_OBJECT

  public:
    explicit HttpOutboundEditor(QWidget *parent = nullptr);

    void Load() override
    {
        http.loadJson(settings);
        http.user.ReadWriteBind(http_UserNameTxt, "text", &QLineEdit::textEdited);
        http.pass.ReadWriteBind(http_PasswordTxt, "text", &QLineEdit::textEdited);
    }

    void Store() override
    {
        settings = IOProtocolSettings{ http.toJson() };
    }

  protected:
    void changeEvent(QEvent *e) override;

  private:
    Qv2ray::Models::HTTPSOCKSObject http;
};
