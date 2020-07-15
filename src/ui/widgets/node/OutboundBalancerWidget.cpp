#include "OutboundBalancerWidget.hpp"

OutboundBalancerWidget::OutboundBalancerWidget(QWidget *parent) : QWidget(parent)
{
    setupUi(this);
}

void OutboundBalancerWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type())
    {
        case QEvent::LanguageChange: retranslateUi(this); break;
        default: break;
    }
}
