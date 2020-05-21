#include "DnsSettingsWidget.hpp"

DnsSettingsWidget::DnsSettingsWidget(QWidget *parent) : QWidget(parent)
{
    setupUi(this);
    QvMessageBusConnect(DnsSettingsWidget);
}

QvMessageBusSlotImpl(DnsSettingsWidget)
{
    switch (msg)
    {
        MBRetranslateDefaultImpl;
        case HIDE_WINDOWS:
        case SHOW_WINDOWS:
        case UPDATE_COLORSCHEME: break;
    }
}
