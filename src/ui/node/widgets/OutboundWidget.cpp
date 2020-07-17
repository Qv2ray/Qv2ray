#include "OutboundWidget.hpp"

#include "base/Qv2rayBase.hpp"

OutboundWidget::OutboundWidget(QWidget *parent) : QWidget(parent)
{
    setupUi(this);
}

void OutboundWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type())
    {
        case QEvent::LanguageChange: retranslateUi(this); break;
        default: break;
    }
}
