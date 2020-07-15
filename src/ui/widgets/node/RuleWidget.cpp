#include "RuleWidget.hpp"

QvNodeRuleWidget::QvNodeRuleWidget(QWidget *parent) : QWidget(parent)
{
    setupUi(this);
}

void QvNodeRuleWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type())
    {
        case QEvent::LanguageChange: retranslateUi(this); break;
        default: break;
    }
}
