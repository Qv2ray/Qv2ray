#include "RuleNodeModel.hpp"

#include "core/CoreUtils.hpp"
#include "ui/widgets/node/widgets/RuleWidget.hpp"

#define QV_MODULE_NAME "Node::RuleNodeModel"

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

void RuleNodeModel::inputConnectionCreated(const QtNodes::Connection &){};
void RuleNodeModel::inputConnectionDeleted(const QtNodes::Connection &c)
{
    if (dispatcher->IsNodeConstructing())
        return;
    const auto &inNodeData = convert_nodedata<InboundNodeData>(c.getNode(PortType::Out));
    const auto inboundTag = getTag(*inNodeData->GetData());
    dataptr->inboundTag.removeAll(inboundTag);
}

void RuleNodeModel::outputConnectionCreated(const QtNodes::Connection &){};
void RuleNodeModel::outputConnectionDeleted(const QtNodes::Connection &)
{
    if (dispatcher->IsNodeConstructing())
        return;
    dataptr->balancerTag.clear();
    dataptr->outboundTag.clear();
}

void RuleNodeModel::setInData(std::vector<std::shared_ptr<NodeData>> indata, PortIndex)
{
    if (dispatcher->IsNodeConstructing())
        return;
    dataptr->inboundTag.clear();
    for (const auto d : indata)
    {
        if (!d)
        {
            LOG("Invalid inbound nodedata to rule.");
            continue;
        }
        const auto inboundTag = getTag(*static_cast<InboundNodeData *>(d.get())->GetData());
        dataptr->inboundTag.push_back(inboundTag);
        DEBUG("Connecting inbound:", inboundTag, "to", dataptr->QV2RAY_RULE_TAG);
    }
}

void RuleNodeModel::onNodeHoverEnter(){};
void RuleNodeModel::onNodeHoverLeave(){};
