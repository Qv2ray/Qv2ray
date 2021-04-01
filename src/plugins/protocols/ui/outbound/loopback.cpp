#include "loopback.hpp"

LoopbackSettingsEditor::LoopbackSettingsEditor(QWidget *parent) : Qv2rayPlugin::QvPluginEditor(parent)
{
    setupUi(this);
}

void LoopbackSettingsEditor::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type())
    {
        case QEvent::LanguageChange: retranslateUi(this); break;
        default: break;
    }
}

void LoopbackSettingsEditor::on_inboundTagTxt_textEdited(const QString &arg1)
{
    loopbackSettings["inboundTag"] = arg1;
}
