#include "ChainOutboundWidget.hpp"

ChainOutboundWidget::ChainOutboundWidget(std::shared_ptr<NodeDispatcher> _dispatcher, QWidget *parent) : QvNodeWidget(_dispatcher, parent)
{
    setupUi(this);
    // Simple slot to update UI
    connect(_dispatcher.get(), &NodeDispatcher::OnObjectTagChanged, [this](ComplexTagNodeMode _t1, const QString _t2, const QString _t3) {
        if (tagLabel->text() == _t2)
        {
            tagLabel->setText(_t3);
            OnSizeUpdated();
        }
    });
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

void ChainOutboundWidget::setValue(std::shared_ptr<OutboundObjectMeta> data)
{
    tagLabel->setText(data->getDisplayName());
}
