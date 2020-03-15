#pragma once

#include "NodeDataModel.hpp"
#include "PortType.hpp"
#include "common/QvHelpers.hpp"

#include <QLabel>

using QtNodes::NodeData;
using QtNodes::NodeDataModel;
using QtNodes::NodeDataType;
using QtNodes::NodeValidationState;
using QtNodes::PortIndex;
using QtNodes::PortType;

using QtNodes::NodeData;
using QtNodes::NodeDataType;

const int GRAPH_NODE_LABEL_FONTSIZE_INCREMENT = 3;

namespace Qv2ray::ui::nodemodels
{
    const NodeDataType outboundType = { "outbound", "Outbound Object" };
    const NodeDataType inboundType = { "inbound", "Inbound Object" };
    /// The class can potentially incapsulate any user data
    /// need to be transferred within the Node Editor graph
    class InboundNodeData : public NodeData
    {
      public:
        InboundNodeData(){ DEBUG(MODULE_GRAPH, "DANGER: Initialising a data model without value.") } InboundNodeData(QString in)
            : _inboundTag(in)
        {
        }

        NodeDataType type() const override
        {
            return inboundType;
        }

        QString GetInbound()
        {
            return _inboundTag;
        }

      private:
        QString _inboundTag;
    };

    /// The class can potentially incapsulate any user data
    /// need to be transferred within the Node Editor graph
    class OutboundNodeData : public NodeData
    {
      public:
        OutboundNodeData()
            : _outboundTag(){ DEBUG(MODULE_GRAPH, "DANGER: Initialising a data model without value.") } OutboundNodeData(QString out)
            : _outboundTag(out)
        {
        }

        NodeDataType type() const override
        {
            return outboundType;
        }

        QString GetOutbound()
        {
            return _outboundTag;
        }

      private:
        QString _outboundTag;
    };

    /// The class can potentially incapsulate any user data
    /// need to be transferred within the Node Editor graph
    class RuleNodeData : public NodeData
    {
      public:
        RuleNodeData()
            : _ruleTag(){ DEBUG(MODULE_GRAPH, "DANGER: Initialising a data model without value.") } RuleNodeData(QString out) : _ruleTag(out)
        {
        }

        NodeDataType type() const override
        {
            return outboundType;
        }

        QString GetRuleTag()
        {
            return _ruleTag;
        }

      private:
        QString _ruleTag;
    };
} // namespace Qv2ray::ui::nodemodels

using namespace Qv2ray::ui::nodemodels;
