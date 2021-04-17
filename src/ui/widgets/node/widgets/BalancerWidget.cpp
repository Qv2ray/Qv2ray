#include "BalancerWidget.hpp"

#include "base/Qv2rayBase.hpp"

BalancerWidget::BalancerWidget(std::shared_ptr<NodeDispatcher> _dispatcher, QWidget *parent) : QvNodeWidget(_dispatcher, parent)
{
    setupUi(this);
    balancerAddBtn->setIcon(QIcon(QV2RAY_COLORSCHEME_FILE("add")));
    balancerDelBtn->setIcon(QIcon(QV2RAY_COLORSCHEME_FILE("ashbin")));
    connect(dispatcher.get(), &NodeDispatcher::OnOutboundCreated, this, &BalancerWidget::OutboundCreated);
    connect(dispatcher.get(), &NodeDispatcher::OnOutboundDeleted, this, &BalancerWidget::OutboundDeleted);
    connect(dispatcher.get(), &NodeDispatcher::OnObjectTagChanged, this, &BalancerWidget::OnTagChanged);
}

void BalancerWidget::setValue(std::shared_ptr<OutboundObjectMeta> data)
{
    outboundData = data;
    balancerSelectionCombo->clear();
    balancerSelectionCombo->addItems(dispatcher->GetRealOutboundTags());
    balancerTagTxt->setText(data->getDisplayName());
    balancerList->addItems(data->outboundTags);
    strategyCB->setCurrentText(data->strategyType);
}

void BalancerWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type())
    {
        case QEvent::LanguageChange: retranslateUi(this); break;
        default: break;
    }
}

void BalancerWidget::on_balancerAddBtn_clicked()
{
    const auto balancerTx = balancerSelectionCombo->currentText().trimmed();

    if (!balancerTx.isEmpty())
    {
        outboundData->outboundTags.append(balancerSelectionCombo->currentText());
        balancerList->addItem(balancerTx);
        balancerSelectionCombo->setEditText("");
    }
}

void BalancerWidget::OutboundCreated(std::shared_ptr<OutboundObjectMeta> data, QtNodes::Node &)
{
    if (data->metaType != METAOUTBOUND_BALANCER)
        balancerSelectionCombo->addItem(data->getDisplayName());
}

void BalancerWidget::OutboundDeleted(const OutboundObjectMeta &data)
{
    if (data.metaType != METAOUTBOUND_BALANCER)
        balancerSelectionCombo->removeItem(balancerSelectionCombo->findText(data.getDisplayName()));
}

void BalancerWidget::OnTagChanged(ComplexTagNodeMode type, const QString originalTag, const QString newTag)
{
    if (type != NODE_OUTBOUND)
        return;
    const auto index = balancerSelectionCombo->findText(originalTag);
    if (index >= 0)
        balancerSelectionCombo->setItemText(index, newTag);
}

void BalancerWidget::on_balancerDelBtn_clicked()
{
    if (balancerList->currentRow() < 0)
        return;

    outboundData->outboundTags.removeOne(balancerList->currentItem()->text());
    balancerList->takeItem(balancerList->currentRow());
}

void BalancerWidget::on_balancerTagTxt_textEdited(const QString &arg1)
{
    const auto originalName = outboundData->getDisplayName();
    if (originalName == arg1 || dispatcher->RenameTag<NODE_OUTBOUND>(originalName, arg1))
    {
        outboundData->displayName = arg1;
        BLACK(balancerTagTxt);
    }
    else
    {
        RED(balancerTagTxt);
    }
}

void BalancerWidget::on_showHideBtn_clicked()
{
    tabWidget->setVisible(!tabWidget->isVisible());
}

void BalancerWidget::on_strategyCB_currentIndexChanged(const QString &arg1)
{
    outboundData->strategyType = arg1;
}
