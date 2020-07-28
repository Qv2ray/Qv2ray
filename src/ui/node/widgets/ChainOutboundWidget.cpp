#include "ChainOutboundWidget.hpp"

ChainOutboundWidget::ChainOutboundWidget(std::shared_ptr<NodeDispatcher> _dispatcher, QWidget *parent) : QvNodeWidget(_dispatcher, parent)
{
    setupUi(this);
}

void ChainOutboundWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type())
    {
        case QEvent::LanguageChange: retranslateUi(this); break;
        default: break;
    }
}

void ChainOutboundWidget::setValue(std::shared_ptr<QString> tag)
{
    tagLabel->setText(*tag);
}
