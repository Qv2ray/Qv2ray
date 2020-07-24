#include "RoutingEditorWidget.hpp"

RoutingEditorWidget::RoutingEditorWidget(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);
}

void RoutingEditorWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
        break;
    default:
        break;
    }
}
