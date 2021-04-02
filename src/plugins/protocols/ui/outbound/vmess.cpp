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

    QJS_CLEAR_BINDINGS

    vmess.loadJson(content["vnext"].toArray().first().toObject());
    if (vmess.users->isEmpty())
        vmess.users->push_back({});

    QJS_RWBINDING(vmess.users->first(), security, securityCombo, currentText, &QComboBox::currentIndexChanged)
    QJS_RWBINDING(vmess.users->first(), alterId, alterLineEdit, value, &QSpinBox::valueChanged)
    QJS_RWBINDING(vmess.users->first(), id, idLineEdit, text, &QLineEdit::textEdited)

    if (alterLineEdit->value() > 0)
    {
        const auto msg = tr("VMess MD5 with Non-zero AlterID has been deprecated, please use VMessAEAD.");
        emit InternalProtocolSupportPluginInstance->PluginErrorMessageBox(tr("Non AEAD VMess detected"), msg);
    }
}
