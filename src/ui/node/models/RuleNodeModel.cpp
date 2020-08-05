#include "ui/node/models/RuleNodeModel.hpp"

#include "core/CoreUtils.hpp"
#include "ui/node/widgets/RuleWidget.hpp"

RuleNodeModel::RuleNodeModel(std::shared_ptr<NodeDispatcher> _dispatcher, std::shared_ptr<node_data_t> data) : NodeDataModel()
{
    dataptr = data;
    dispatcher = _dispatcher;
    widget = new QvNodeRuleWidget(dispatcher);
    connect(widget, &QvNodeWidget::OnSizeUpdated, this, &NodeDataModel::embeddedWidgetSizeUpdated);
    ((QvNodeRuleWidget *) widget)->setValue(data);
    widget->setWindowFlags(Qt::FramelessWindowHint);
    widget->setAttribute(Qt::WA_TranslucentBackground);
    //
    const auto renameFunc = [this](ComplexTagNodeMode mode, const QString originalTag, const QString newTag) {
        if (mode == NODE_INBOUND)
        {
            if (dataptr->inboundTag.contains(originalTag))
            {
                dataptr->inboundTag.removeAll(originalTag);
                dataptr->inboundTag.push_back(newTag);
            }
        }
        else if (mode == NODE_OUTBOUND)
        {
            if (dataptr->outboundTag == originalTag)
                dataptr->outboundTag = newTag;
            if (dataptr->balancerTag == originalTag)
                dataptr->balancerTag = newTag;
        }
    };
    connect(dispatcher.get(), &NodeDispatcher::OnObjectTagChanged, renameFunc);
}

void RuleNodeModel::inputConnectionCreated(const QtNodes::Connection &c)
{
    if (dispatcher->IsNodeConstructing())
        return;
    const auto &inNodeData = convert_nodedata<InboundNodeData>(c.getNode(PortType::Out));
    const auto inboundTag = getTag(*inNodeData->GetData());
    if (!dataptr->inboundTag.contains(inboundTag))
        dataptr->inboundTag.push_back(inboundTag);
}

void RuleNodeModel::inputConnectionDeleted(const QtNodes::Connection &c)
{
    if (dispatcher->IsNodeConstructing())
        return;
    const auto &inNodeData = convert_nodedata<InboundNodeData>(c.getNode(PortType::Out));
    const auto inboundTag = getTag(*inNodeData->GetData());
    dataptr->inboundTag.removeAll(inboundTag);
}

void RuleNodeModel::outputConnectionCreated(const QtNodes::Connection &c)
{
    if (dispatcher->IsNodeConstructing())
        return;
    const auto &outbound = convert_nodedata<OutboundNodeData>(c.getNode(PortType::In));
    switch (outbound->GetData().get()->metaType)
    {
        case METAOUTBOUND_CHAIN:
        case METAOUTBOUND_EXTERNAL:
        case METAOUTBOUND_ORIGINAL:
        {
            dataptr->balancerTag.clear();
            dataptr->outboundTag = outbound->GetData()->getDisplayName();
            break;
        }
        case METAOUTBOUND_BALANCER:
        {
            dataptr->outboundTag.clear();
            dataptr->balancerTag = outbound->GetData()->getDisplayName();
            break;
        }
    }
}

void RuleNodeModel::outputConnectionDeleted(const QtNodes::Connection &)
{
    if (dispatcher->IsNodeConstructing())
        return;
    dataptr->balancerTag.clear();
    dataptr->outboundTag.clear();
}

void RuleNodeModel::setInData(std::vector<std::shared_ptr<NodeData>>, PortIndex){};
void RuleNodeModel::onNodeHoverEnter(){};
void RuleNodeModel::onNodeHoverLeave(){};
