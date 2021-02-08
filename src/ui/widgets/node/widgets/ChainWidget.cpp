#include "ChainWidget.hpp"

#include "base/Qv2rayBase.hpp"

ChainWidget::ChainWidget(std::shared_ptr<NodeDispatcher> _dispatcher, QWidget *parent) : QvNodeWidget(_dispatcher, parent)
{
    setupUi(this);
    editChainBtn->setIcon(QIcon(QV2RAY_COLORSCHEME_FILE("edit")));
}

void ChainWidget::setValue(std::shared_ptr<OutboundObjectMeta> data)
{
    dataptr = data;
    displayNameTxt->setText(data->getDisplayName());
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

void ChainWidget::on_displayNameTxt_textEdited(const QString &arg1)
{
    const auto originalTag = dataptr->getDisplayName();
    if (originalTag == arg1 || dispatcher->RenameTag<NODE_OUTBOUND>(originalTag, arg1))
    {
        dataptr->displayName = arg1;
        BLACK(displayNameTxt);
    }
    else
    {
        RED(displayNameTxt);
    }
}

void ChainWidget::on_editChainBtn_clicked()
{
    emit dispatcher->RequestEditChain(dataptr->getDisplayName());
}
