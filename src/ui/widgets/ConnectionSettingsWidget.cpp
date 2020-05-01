#include "ConnectionSettingsWidget.hpp"

ConnectionSettingsWidget::ConnectionSettingsWidget(QWidget *parent) : QWidget(parent)
{
    setupUi(this);
}

void ConnectionSettingsWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type())
    {
        case QEvent::LanguageChange: retranslateUi(this); break;
        default: break;
    }
}
