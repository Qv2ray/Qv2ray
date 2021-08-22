#pragma once

#include "QvPlugin/Gui/QvGUIPluginInterface.hpp"
#include "ui_freedom.h"

class FreedomOutboundEditor
    : public Qv2rayPlugin::Gui::PluginProtocolEditor
    , private Ui::freedomOutEditor
{
    Q_OBJECT

  public:
    explicit FreedomOutboundEditor(QWidget *parent = nullptr);

    void Load() override
    {
        isLoading = true;
        DSCB->setCurrentText(settings["domainStrategy"].toString());
        redirectTxt->setText(settings["redirect"].toString());
        isLoading = false;
    };
    void Store() override{};

  protected:
    void changeEvent(QEvent *e) override;

  private slots:
    void on_DSCB_currentTextChanged(const QString &arg1);
    void on_redirectTxt_textEdited(const QString &arg1);

  private:
    bool isLoading = false;
};
