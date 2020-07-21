#include "BalancerWidget.hpp"

#include "base/Qv2rayBase.hpp"
#include "ui/common/UIBase.hpp"

BalancerWidget::BalancerWidget(std::shared_ptr<NodeDispatcher> _dispatcher, QWidget *parent) : QvNodeWidget(_dispatcher, parent)
{
    setupUi(this);
    balancerAddBtn->setIcon(QICON_R("add"));
    balancerDelBtn->setIcon(QICON_R("ashbin"));
}

void BalancerWidget::setValue(std::shared_ptr<OutboundObjectMeta> data)
{
    outboundData = data;
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
        targetList.append(balancerSelectionCombo->currentText());
        balancerList->addItem(balancerTx);
        balancerSelectionCombo->setEditText("");
    }
    else
    {
        // statusLabel->setText(tr("Balancer is empty, not processing."));
    }
}

void BalancerWidget::on_balancerDelBtn_clicked()
{
    if (balancerList->currentRow() < 0)
    {
        return;
    }

    targetList.removeAt(balancerList->currentRow());
    balancerList->takeItem(balancerList->currentRow());
    // statusLabel->setText(tr("Removed a balancer entry."));
}
