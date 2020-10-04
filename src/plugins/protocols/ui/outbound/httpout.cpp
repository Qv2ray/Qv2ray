#include "httpout.hpp"

HttpOutboundEditor::HttpOutboundEditor(QWidget *parent) : Qv2rayPlugin::QvPluginEditor(parent)
{
    setupUi(this);
    setProperty("QV2RAY_INTERNAL_HAS_STREAMSETTINGS", true);
    setProperty("QV2RAY_INTERNAL_HAS_FORWARD_PROXY", true);
}

void HttpOutboundEditor::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type())
    {
        case QEvent::LanguageChange: retranslateUi(this); break;
        default: break;
    }
}

void HttpOutboundEditor::on_http_UserNameTxt_textEdited(const QString &arg1)
{
    if (http.users.isEmpty())
        http.users << HTTPSOCKSUserObject{};
    http.users.front().user = arg1;
}

void HttpOutboundEditor::on_http_PasswordTxt_textEdited(const QString &arg1)
{
    if (http.users.isEmpty())
        http.users << HTTPSOCKSUserObject{};
    http.users.front().pass = arg1;
}
