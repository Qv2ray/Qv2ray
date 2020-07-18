#include "ChainWidget.hpp"

#include "base/Qv2rayBase.hpp"

ChainWidget::ChainWidget(std::shared_ptr<NodeDispatcher> _dispatcher, QWidget *parent) : QvNodeWidget(_dispatcher, parent)
{
    setupUi(this);
}

void ChainWidget::setValue(std::shared_ptr<OutboundObjectMeta> data)
{
}

void ChainWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type())
    {
        case QEvent::LanguageChange: retranslateUi(this); break;
        default: break;
    }
}
