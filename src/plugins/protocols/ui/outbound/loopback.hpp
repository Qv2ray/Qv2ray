#pragma once

#include "QvPlugin/Gui/QvGUIPluginInterface.hpp"
#include "ui_loopback.h"

class LoopbackSettingsEditor
    : public Qv2rayPlugin::Gui::PluginProtocolEditor
    , private Ui::loopback
{
    Q_OBJECT

  public:
    explicit LoopbackSettingsEditor(QWidget *parent = nullptr);

    void Load() override
    {
        inboundTagTxt->setText(settings["inboundTag"].toString());
    }

    void Store() override
    {
    }

  protected:
    void changeEvent(QEvent *e) override;

  private slots:
    void on_inboundTagTxt_textEdited(const QString &arg1);
};
