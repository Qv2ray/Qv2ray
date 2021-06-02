#include "vmess.hpp"

#include "BuiltinProtocolPlugin.hpp"

VmessOutboundEditor::VmessOutboundEditor(QWidget *parent) : Qv2rayPlugin::QvPluginEditor(parent)
{
    setupUi(this);
    setProperty("QV2RAY_INTERNAL_HAS_STREAMSETTINGS", true);
    setProperty("QV2RAY_INTERNAL_HAS_FORWARD_PROXY", true);
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

void VmessOutboundEditor::SetContent(const QJsonObject &content)
{
    this->content = content;
    if (content["vnext"].toArray().isEmpty())
        content["vnext"] = QJsonArray{ QJsonObject{} };

    vmess.loadJson(content["vnext"].toArray().first().toObject());
    if (vmess.users->isEmpty())
        vmess.users->push_back({});

    vmess.users->first().security.ReadWriteBind(securityCombo, "currentText", &QComboBox::currentIndexChanged);
    vmess.users->first().alterId.ReadWriteBind(alterLineEdit, "value", &QSpinBox::valueChanged);
    vmess.users->first().id.ReadWriteBind(idLineEdit, "text", &QLineEdit::textEdited);

    if (alterLineEdit->value() > 0)
    {
        const auto msg = tr("VMess MD5 with Non-zero AlterID has been deprecated, please use VMessAEAD.");
        emit PluginInstance->PluginErrorMessageBox(tr("Non AEAD VMess detected"), msg);
    }
}
