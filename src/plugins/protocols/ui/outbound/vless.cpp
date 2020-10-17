#include "vless.hpp"

#define ENSURE_USERS                                                                                                                                 \
    if (vless.users.isEmpty())                                                                                                                       \
        vless.users.push_back({});

VlessOutboundEditor::VlessOutboundEditor(QWidget *parent) : Qv2rayPlugin::QvPluginEditor(parent)
{
    setupUi(this);
    setProperty("QV2RAY_INTERNAL_HAS_STREAMSETTINGS", true);
    setProperty("QV2RAY_INTERNAL_HAS_FORWARD_PROXY", true);
}

void VlessOutboundEditor::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type())
    {
        case QEvent::LanguageChange: retranslateUi(this); break;
        default: break;
    }
}

void VlessOutboundEditor::on_vLessIDTxt_textEdited(const QString &arg1)
{
    const static QRegularExpression regExpUUID("^[0-9a-f]{8}(-[0-9a-f]{4}){3}-[0-9a-f]{12}$", QRegularExpression::CaseInsensitiveOption);
    if (!regExpUUID.match(arg1).hasMatch())
    {
        RED(vLessIDTxt);
    }
    else
    {
        BLACK(vLessIDTxt);
    }
    ENSURE_USERS
    vless.users.front().id = arg1;
}

void VlessOutboundEditor::on_vLessSecurityCombo_currentTextChanged(const QString &arg1)
{
    ENSURE_USERS
    vless.users.front().encryption = arg1;
}

void VlessOutboundEditor::on_flowCombo_currentTextChanged(const QString &arg1)
{

    ENSURE_USERS
    PLUGIN_EDITOR_LOADING_GUARD
    vless.users.front().flow = arg1;
}
