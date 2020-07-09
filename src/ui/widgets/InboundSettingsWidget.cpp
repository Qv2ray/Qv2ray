#include "InboundSettingsWidget.hpp"
InboundSettingsWidget::InboundSettingsWidget(QWidget *parent) : QWidget(parent)
{
    setupUi(this);
}

QvMessageBusSlotImpl(InboundSettingsWidget)
{
    switch (msg)
    {
        MBRetranslateDefaultImpl;
        case HIDE_WINDOWS:
        case SHOW_WINDOWS: break;
        default: break;
    }
}

void InboundSettingsWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type())
    {
        case QEvent::LanguageChange: retranslateUi(this); break;
        default: break;
    }
}
