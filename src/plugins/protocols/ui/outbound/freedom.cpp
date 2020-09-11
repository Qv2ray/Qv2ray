#include "freedom.hpp"

FreedomOutboundEditor::FreedomOutboundEditor(QWidget *parent) : Qv2rayPlugin::QvPluginEditor(parent)
{
    setupUi(this);
    // Should freedom outbound use StreamSettings?
    setProperty("QV2RAY_INTERNAL_HAS_STREAMSETTINGS", true);
    setProperty("QV2RAY_INTERNAL_HAS_FORWARD_PROXY", true);
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
    PLUGIN_EDITOR_LOADING_GUARD
    content["domainStrategy"] = arg1;
}

void FreedomOutboundEditor::on_redirectTxt_textEdited(const QString &arg1)
{
    PLUGIN_EDITOR_LOADING_GUARD
    content["redirect"] = arg1;
}
