#pragma once

#include "QvPlugin/Gui/QvGUIPluginInterface.hpp"
#include "V2RayModels.hpp"
#include "ui_vless.h"

class VlessOutboundEditor
    : public Qv2rayPlugin::Gui::PluginProtocolEditor
    , private Ui::vlessOutEditor
{
    Q_OBJECT

  public:
    explicit VlessOutboundEditor(QWidget *parent = nullptr);

    void Load() override
    {
        vless.loadJson(settings);
        vless.encryption.ReadWriteBind(vLessSecurityCombo, "currentText", &QComboBox::currentIndexChanged);
        vless.id.ReadWriteBind(vLessIDTxt, "text", &QLineEdit::textEdited);
    }

    void Store() override
    {
        settings = IOProtocolSettings{ vless.toJson() };
    }

  protected:
    void changeEvent(QEvent *e) override;

  private:
    Qv2ray::Models::VLESSClientObject vless;
};
