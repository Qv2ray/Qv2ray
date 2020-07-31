#include "ChainEditorWidget.hpp"

#include "ui/node/NodeBase.hpp"

constexpr auto GRAPH_GLOBAL_OFFSET_X = -20;
constexpr auto GRAPH_GLOBAL_OFFSET_Y = -300;

ChainEditorWidget::ChainEditorWidget(std::shared_ptr<NodeDispatcher> dispatcher, QWidget *parent) : QWidget(parent), dispatcher(dispatcher)
{
    setupUi(this);
    QvMessageBusConnect(ChainEditorWidget);
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
    //
    connect(dispatcher.get(), &NodeDispatcher::OnChainedOutboundCreated, this, &ChainEditorWidget::OnDispatcherOutboundCreated);
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

void ChainEditorWidget::OnDispatcherOutboundCreated(std::shared_ptr<OutboundObjectMeta>, QtNodes::Node &node)
{
    const auto outboundCount = dispatcher->ChainedOutboundsCount();
    const static int offsets[]{ 0, 300, -300 };
    auto pos = this->pos();
    pos.setX(pos.x() + GRAPH_GLOBAL_OFFSET_X + offsets[outboundCount % 3]);
    pos.setY(pos.y() + outboundCount * 100 + GRAPH_GLOBAL_OFFSET_Y);
    scene->setNodePosition(node, pos);
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

void ChainEditorWidget::on_chainComboBox_currentIndexChanged(const QString &arg1)
{
}
