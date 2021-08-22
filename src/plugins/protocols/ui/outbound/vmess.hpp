#pragma once

#include "QvPlugin/Gui/QvGUIPluginInterface.hpp"
#include "V2RayModels.hpp"
#include "ui_vmess.h"

class VmessOutboundEditor
    : public Qv2rayPlugin::Gui::PluginProtocolEditor
    , private Ui::vmessOutEditor
{
    Q_OBJECT

  public:
    explicit VmessOutboundEditor(QWidget *parent = nullptr);

    void Load() override;
    void Store() override
    {
        settings = IOProtocolSettings{ vmess.toJson() };
    }

  private:
    Qv2ray::Models::VMessClientObject vmess;

  protected:
    void changeEvent(QEvent *e) override;
};
