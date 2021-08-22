#include "PluginSettingsWidget.hpp"

SimplePluginSettingsWidget::SimplePluginSettingsWidget(QWidget *parent) : Qv2rayPlugin::Gui::PluginSettingsWidget(parent)
{
    setupUi(this);
}

void SimplePluginSettingsWidget::Load()
{
}

void SimplePluginSettingsWidget::Store()
{
}

void SimplePluginSettingsWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type())
    {
        case QEvent::LanguageChange: retranslateUi(this); break;
        default: break;
    }
}
