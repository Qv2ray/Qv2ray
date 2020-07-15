#pragma once

#include "base/Qv2rayBase.hpp"
#include "common/QvHelpers.hpp"

#include <QLabel>
#include <memory>
#include <nodes/internal/NodeDataModel.hpp>
#include <nodes/internal/PortType.hpp>

using QtNodes::NodeData;
using QtNodes::NodeDataModel;
using QtNodes::NodeDataType;
using QtNodes::NodeValidationState;
using QtNodes::PortIndex;
using QtNodes::PortType;

using QtNodes::NodeData;
using QtNodes::NodeDataType;

constexpr auto GRAPH_NODE_LABEL_FONTSIZE_INCREMENT = 3;

namespace Qv2ray::ui::nodemodels
{
    const std::shared_ptr<NodeDataType> NODE_TYPE_OUTBOUND = std::make_shared<NodeDataType>("outbound", QObject::tr("Outbound"));
    const std::shared_ptr<NodeDataType> NODE_TYPE_INBOUND = std::make_shared<NodeDataType>("inbound", QObject::tr("Inbound"));

    class InboundNodeData : public NodeData
    {
      public:
        explicit InboundNodeData(std::shared_ptr<INBOUND> data) : inboundData(data){};

        std::shared_ptr<NodeDataType> type() const override
        {
            return NODE_TYPE_INBOUND;
        }

        std::shared_ptr<INBOUND> GetInbound()
        {
            return inboundData;
        }

      private:
        std::shared_ptr<INBOUND> inboundData;
    };

    class OutboundNodeData : public NodeData
    {
      public:
        explicit OutboundNodeData(std::shared_ptr<OUTBOUND> data) : outboundData(data){};

        std::shared_ptr<NodeDataType> type() const override
        {
            return NODE_TYPE_INBOUND;
        }

        std::shared_ptr<OUTBOUND> GetOutbound()
        {
            return outboundData;
        }

      private:
        std::shared_ptr<OUTBOUND> outboundData;
    };

    class RuleNodeData : public NodeData
    {

      public:
        explicit RuleNodeData(std::shared_ptr<RuleObject> rule) : _rule(rule){};

        std::shared_ptr<NodeDataType> type() const override
        {
            return NODE_TYPE_INBOUND;
        }

        std::shared_ptr<RuleObject> GetRule()
        {
            return _rule;
        }

      private:
        std::shared_ptr<RuleObject> _rule;
    };
} // namespace Qv2ray::ui::nodemodels

using namespace Qv2ray::ui::nodemodels;
