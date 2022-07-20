#pragma once

#include "QvPlugin/Gui/QvGUIPluginInterface.hpp"
#include "ui_blackhole.h"

class BlackholeOutboundEditor
    : public Qv2rayPlugin::Gui::PluginProtocolEditor
    , private Ui::blackholeOutEditor
{
    Q_OBJECT

  public:
    explicit BlackholeOutboundEditor(QWidget *parent = nullptr);

    virtual void Load() override;
    virtual void Store() override;

  protected:
    void changeEvent(QEvent *e) override;

  private slots:
    void on_responseTypeCB_currentTextChanged(const QString &arg1);

  private:
    bool isLoading = false;
};
