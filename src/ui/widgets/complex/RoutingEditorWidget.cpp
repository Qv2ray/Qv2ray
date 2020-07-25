#include "RoutingEditorWidget.hpp"

#include <QVBoxLayout>
#include <nodes/FlowScene>

RoutingEditorWidget::RoutingEditorWidget(QWidget *parent) : QtNodes::FlowView(parent)
{
    setupUi(this);
    setScene(new QtNodes::FlowScene(this));
    scaleDown();
}

void RoutingEditorWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type())
    {
        case QEvent::LanguageChange: retranslateUi(this); break;
        default: break;
    }
}
