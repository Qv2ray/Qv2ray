#include "vmess.hpp"

#include "BuiltinProtocolPlugin.hpp"

VmessOutboundEditor::VmessOutboundEditor(QWidget *parent) : Qv2rayPlugin::Gui::PluginProtocolEditor(parent)
{
    setupUi(this);
    setProperty("QV2RAY_INTERNAL_HAS_STREAMSETTINGS", true);
}

void VmessOutboundEditor::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type())
    {
        case QEvent::LanguageChange: retranslateUi(this); break;
        default: break;
    }
}

void VmessOutboundEditor::Load()
{
    vmess.loadJson(settings);

    vmess.security.ReadWriteBind(securityCombo, "currentText", &QComboBox::currentIndexChanged);
    vmess.id.ReadWriteBind(idLineEdit, "text", &QLineEdit::textEdited);
    vmess.experiments.ReadWriteBind(experimentsTxt, "text", &QLineEdit::textEdited);
}
