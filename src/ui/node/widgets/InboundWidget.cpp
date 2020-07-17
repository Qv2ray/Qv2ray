#include "InboundWidget.hpp"

InboundWidget::InboundWidget(QWidget *parent) : QvNodeWidget(parent)
{
    setupUi(this);
}

void InboundWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type())
    {
        case QEvent::LanguageChange: retranslateUi(this); break;
        default: break;
    }
}
