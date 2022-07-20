#include "freedom.hpp"

FreedomOutboundEditor::FreedomOutboundEditor(QWidget *parent) : Qv2rayPlugin::Gui::PluginProtocolEditor(parent)
{
    setupUi(this);
    // Should freedom outbound use StreamSettings?
    setProperty("QV2RAY_INTERNAL_HAS_STREAMSETTINGS", false);
}

void FreedomOutboundEditor::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type())
    {
        case QEvent::LanguageChange: retranslateUi(this); break;
        default: break;
    }
}

void FreedomOutboundEditor::on_DSCB_currentTextChanged(const QString &arg1)
{
    settings["domainStrategy"] = arg1;
}

void FreedomOutboundEditor::on_redirectTxt_textEdited(const QString &arg1)
{
    settings["redirect"] = arg1;
}
