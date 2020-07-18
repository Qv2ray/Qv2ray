#include "OutboundChainWidget.hpp"

#include "base/Qv2rayBase.hpp"

OutboundChainWidget::OutboundChainWidget(QWidget *parent) : QvNodeWidget(parent)
{
    setupUi(this);
}

void OutboundChainWidget::setValue(std::shared_ptr<OutboundObjectMeta> data)
{
}

void OutboundChainWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type())
    {
        case QEvent::LanguageChange: retranslateUi(this); break;
        default: break;
    }
}
