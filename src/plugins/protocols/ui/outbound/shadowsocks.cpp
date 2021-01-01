#include "shadowsocks.hpp"

ShadowsocksOutboundEditor::ShadowsocksOutboundEditor(QWidget *parent) : Qv2rayPlugin::QvPluginEditor(parent)
{
    setupUi(this);
    setProperty("QV2RAY_INTERNAL_HAS_STREAMSETTINGS", true);
    setProperty("QV2RAY_INTERNAL_HAS_FORWARD_PROXY", true);
}

void ShadowsocksOutboundEditor::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type())
    {
        case QEvent::LanguageChange: retranslateUi(this); break;
        default: break;
    }
}

void ShadowsocksOutboundEditor::on_ss_passwordTxt_textEdited(const QString &arg1)
{
    PLUGIN_EDITOR_LOADING_GUARD
    shadowsocks.password = arg1;
}

void ShadowsocksOutboundEditor::on_ss_encryptionMethod_currentTextChanged(const QString &arg1)
{
    PLUGIN_EDITOR_LOADING_GUARD
    shadowsocks.method = arg1;
}
