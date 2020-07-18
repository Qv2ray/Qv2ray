#include "OutboundBalancerWidget.hpp"

#include "base/Qv2rayBase.hpp"

OutboundBalancerWidget::OutboundBalancerWidget(QWidget *parent) : QvNodeWidget(parent)
{
    setupUi(this);
    balancerAddBtn->setIcon(QICON_R("add.png"));
    balancerDelBtn->setIcon(QICON_R("delete.png"));
}

void OutboundBalancerWidget::setValue(std::shared_ptr<OutboundObjectMeta> data)
{
    outboundData = data;
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

void OutboundBalancerWidget::on_balancerAddBtn_clicked()
{
    const auto balancerTx = balancerSelectionCombo->currentText().trimmed();

    if (!balancerTx.isEmpty())
    {
        targetList.append(balancerSelectionCombo->currentText());
        balancerList->addItem(balancerTx);
        balancerSelectionCombo->setEditText("");
    }
    else
    {
        // statusLabel->setText(tr("Balancer is empty, not processing."));
    }
}

void OutboundBalancerWidget::on_balancerDelBtn_clicked()
{
    if (balancerList->currentRow() < 0)
    {
        return;
    }

    targetList.removeAt(balancerList->currentRow());
    balancerList->takeItem(balancerList->currentRow());
    // statusLabel->setText(tr("Removed a balancer entry."));
}
