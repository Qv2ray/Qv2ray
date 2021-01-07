#include "vmess.hpp"

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

void VmessOutboundEditor::on_idLineEdit_textEdited(const QString &arg1)
{
    const static QRegularExpression regExpUUID("^[0-9a-f]{8}(-[0-9a-f]{4}){3}-[0-9a-f]{12}$", QRegularExpression::CaseInsensitiveOption);

    if (!regExpUUID.match(arg1).hasMatch())
    {
        RED(idLineEdit)
    }
    else
    {
        BLACK(idLineEdit)
    }
    if (vmess.users.isEmpty())
        vmess.users << VMessServerObject::UserObject{};
    vmess.users.front().id = arg1;
}

void VmessOutboundEditor::on_securityCombo_currentIndexChanged(int arg1)
{
    if (vmess.users.isEmpty())
        vmess.users << VMessServerObject::UserObject{};
    vmess.users.front().security = securityCombo->itemText(arg1);
}

void VmessOutboundEditor::on_alterLineEdit_valueChanged(int arg1)
{
    if (vmess.users.isEmpty())
        vmess.users << VMessServerObject::UserObject{};
    vmess.users.front().alterId = arg1;
}
