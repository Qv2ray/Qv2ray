#include "socksout.hpp"

SocksOutboundEditor::SocksOutboundEditor(QWidget *parent) : Qv2rayPlugin::QvPluginEditor(parent)
{
    setupUi(this);
    setProperty("QV2RAY_INTERNAL_HAS_STREAMSETTINGS", true);
    setProperty("QV2RAY_INTERNAL_HAS_FORWARD_PROXY", true);
}

void SocksOutboundEditor::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type())
    {
        case QEvent::LanguageChange: retranslateUi(this); break;
        default: break;
    }
}

void SocksOutboundEditor::on_socks_UserNameTxt_textEdited(const QString &arg1)
{
    if (socks.users.isEmpty())
        socks.users << HTTPSOCKSUserObject{};
    socks.users.front().user = arg1;
}

void SocksOutboundEditor::on_socks_PasswordTxt_textEdited(const QString &arg1)
{
    if (socks.users.isEmpty())
        socks.users << HTTPSOCKSUserObject{};
    socks.users.front().pass = arg1;
}
