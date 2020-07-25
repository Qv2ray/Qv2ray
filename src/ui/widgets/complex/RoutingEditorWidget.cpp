#include "RoutingEditorWidget.hpp"

#include <QVBoxLayout>
#include <nodes/FlowScene>

RoutingEditorWidget::RoutingEditorWidget(QWidget *parent) : QWidget(parent)
{
    setupUi(this);
    scene = new QtNodes::FlowScene(this);
    view = new QtNodes::FlowView(scene, this);
    view->scaleDown();

    if (!viewWidget->layout())
    {
        // The QWidget will take ownership of layout.
        viewWidget->setLayout(new QVBoxLayout());
    }
    auto l = viewWidget->layout();
    l->addWidget(view);
    l->setContentsMargins(0, 0, 0, 0);
    l->setSpacing(0);
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
