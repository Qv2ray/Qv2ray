#include "InboundOutboundWidget.hpp"

InboundOutboundWidget::InboundOutboundWidget(QWidget *parent) : QWidget(parent)
{
    setupUi(this);
}

void InboundOutboundWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type())
    {
        case QEvent::LanguageChange: retranslateUi(this); break;
        default: break;
    }
}
