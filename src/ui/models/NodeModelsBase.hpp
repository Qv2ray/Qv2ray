#pragma once

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

const int GRAPH_NODE_LABEL_FONTSIZE_INCREMENT = 3;

namespace Qv2ray::ui::nodemodels
{
    const std::shared_ptr<NodeDataType> outboundType = std::make_shared<NodeDataType>("outbound", QObject::tr("Outbound"));
    const std::shared_ptr<NodeDataType> inboundType = std::make_shared<NodeDataType>("inbound", QObject::tr("Inbound"));
    /// The class can potentially incapsulate any user data
    /// need to be transferred within the Node Editor graph
    class InboundNodeData : public NodeData
    {
      public:
        explicit InboundNodeData(QString in) : _inboundTag(in)
        {
        }

        std::shared_ptr<NodeDataType> type() const override
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
        explicit OutboundNodeData(QString out) : _outboundTag(out)
        {
        }

        std::shared_ptr<NodeDataType> type() const override
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
        explicit RuleNodeData(QString out) : _ruleTag(out)
        {
        }

        std::shared_ptr<NodeDataType> type() const override
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
