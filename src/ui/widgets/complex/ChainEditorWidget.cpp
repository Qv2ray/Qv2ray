#include "ChainEditorWidget.hpp"

ChainEditorWidget::ChainEditorWidget(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);
}

void ChainEditorWidget::changeEvent(QEvent *e)
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
