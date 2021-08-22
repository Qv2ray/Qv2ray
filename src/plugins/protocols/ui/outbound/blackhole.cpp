#include "blackhole.hpp"

BlackholeOutboundEditor::BlackholeOutboundEditor(QWidget *parent) : Qv2rayPlugin::Gui::PluginProtocolEditor(parent)
{
    setupUi(this);
    setProperty("QV2RAY_INTERNAL_HAS_STREAMSETTINGS", false);
}

void BlackholeOutboundEditor::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type())
    {
        case QEvent::LanguageChange: retranslateUi(this); break;
        default: break;
    }
}

void BlackholeOutboundEditor::Load()
{
    isLoading = true;
    if (settings.contains("response") && settings["response"].toObject().contains("type"))
        responseTypeCB->setCurrentText(settings["response"].toObject()["type"].toString());
    isLoading = false;
}

void BlackholeOutboundEditor::Store()
{
}

void BlackholeOutboundEditor::on_responseTypeCB_currentTextChanged(const QString &arg1)
{
    if (isLoading)
        return;
    settings = IOProtocolSettings{ QJsonObject{ { "response", QJsonObject{ { "type", arg1 } } } } };
}
