#include "ChainEditorWidget.hpp"

ChainEditorWidget::ChainEditorWidget(QWidget *parent) : QtNodes::FlowView(parent)
{
    setupUi(this);
    setScene(new QtNodes::FlowScene(this));
}

void ChainEditorWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type())
    {
        case QEvent::LanguageChange: retranslateUi(this); break;
        default: break;
    }
}
