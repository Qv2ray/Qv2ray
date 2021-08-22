#pragma once

#include "QvPlugin/Gui/QvGUIPluginInterface.hpp"
#include "ui_PluginSettingsWidget.h"
class SimplePluginSettingsWidget
    : public Qv2rayPlugin::Gui::PluginSettingsWidget
    , private Ui::PluginSettingsWidget
{
    Q_OBJECT

  public:
    explicit SimplePluginSettingsWidget(QWidget *parent = nullptr);

    virtual void Load() override;
    virtual void Store() override;

  protected:
    void changeEvent(QEvent *e) override;
};
