#include "ChainEditorWidget.hpp"

ChainEditorWidget::ChainEditorWidget(QWidget *parent) : QWidget(parent)
{
    setupUi(this);
    QvMessageBusConnect(ChainEditorWidget);
    scene = new QtNodes::FlowScene(this);
    view = new QtNodes::FlowView(scene, this);
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

QvMessageBusSlotImpl(ChainEditorWidget)
{
    switch (msg)
    {
        MBRetranslateDefaultImpl;
        MBUpdateColorSchemeDefaultImpl;
        default: break;
    }
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
