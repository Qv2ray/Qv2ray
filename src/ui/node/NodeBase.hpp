#pragma once

#include "NodeDispatcher.hpp"
#include "common/QvHelpers.hpp"

#include <QLabel>
#include <memory>
#include <nodes/internal/Connection.hpp>
#include <nodes/internal/NodeDataModel.hpp>
#include <nodes/internal/PortType.hpp>

constexpr auto GRAPH_NODE_LABEL_FONTSIZE_INCREMENT = 3;

using QtNodes::NodeData;
using QtNodes::NodeDataModel;
using QtNodes::NodeDataType;
using QtNodes::NodeValidationState;
using QtNodes::PortIndex;
using QtNodes::PortType;

namespace Qv2ray::ui::nodemodels
{
    const auto NODE_TYPE_OUTBOUND = std::make_shared<NodeDataType>("outbound", QObject::tr("Outbound"));
    const auto NODE_TYPE_INBOUND = std::make_shared<NodeDataType>("inbound", QObject::tr("Inbound"));
    const auto NODE_TYPE_RULE = std::make_shared<NodeDataType>("rule", QObject::tr("Rule"));

    class QvNodeWidget : public QWidget
    {
        Q_OBJECT
      public:
        explicit QvNodeWidget(std::shared_ptr<NodeDispatcher> _dispatcher, QWidget *parent) : QWidget(parent), dispatcher(_dispatcher){};
        template<typename T>
        void setValue(std::shared_ptr<T>);
      signals:
        void OnSizeUpdated();

      protected:
        std::shared_ptr<NodeDispatcher> dispatcher;
    };

#define DECL_NODE_DATA_TYPE(name, TYPE, INNER_TYPE)                                                                                             \
    class name : public NodeData                                                                                                                \
    {                                                                                                                                           \
      public:                                                                                                                                   \
        explicit name(std::shared_ptr<INNER_TYPE> data) : data(data){};                                                                         \
        std::shared_ptr<NodeDataType> type() const override                                                                                     \
        {                                                                                                                                       \
            return TYPE;                                                                                                                        \
        }                                                                                                                                       \
        std::shared_ptr<INNER_TYPE> GetInbound()                                                                                                \
        {                                                                                                                                       \
            return data;                                                                                                                        \
        }                                                                                                                                       \
                                                                                                                                                \
      private:                                                                                                                                  \
        std::shared_ptr<INNER_TYPE> data;                                                                                                       \
    }

    DECL_NODE_DATA_TYPE(InboundNodeData, NODE_TYPE_INBOUND, INBOUND);
    DECL_NODE_DATA_TYPE(OutboundNodeData, NODE_TYPE_OUTBOUND, OutboundObjectMeta);
    DECL_NODE_DATA_TYPE(RuleNodeData, NODE_TYPE_RULE, RuleObject);

    //
    //***********************************************************************************************************************************
    //
#define DECL_NODE_DATA_MODEL(NAME, CONTENT_TYPE)                                                                                                \
    class NAME : public NodeDataModel                                                                                                           \
    {                                                                                                                                           \
        Q_OBJECT                                                                                                                                \
      public:                                                                                                                                   \
        explicit NAME(std::shared_ptr<NodeDispatcher>, std::shared_ptr<CONTENT_TYPE>);                                                          \
        ~NAME(){};                                                                                                                              \
                                                                                                                                                \
        inline QString caption() const override                                                                                                 \
        {                                                                                                                                       \
            return {};                                                                                                                          \
        }                                                                                                                                       \
        inline bool captionVisible() const override                                                                                             \
        {                                                                                                                                       \
            return false;                                                                                                                       \
        }                                                                                                                                       \
        inline QString name() const override                                                                                                    \
        {                                                                                                                                       \
            return #NAME;                                                                                                                       \
        }                                                                                                                                       \
        ConnectionPolicy portOutConnectionPolicy(PortIndex) const override;                                                                     \
        ConnectionPolicy portInConnectionPolicy(PortIndex) const override;                                                                      \
        unsigned int nPorts(PortType portType) const override;                                                                                  \
        std::shared_ptr<NodeDataType> dataType(PortType portType, PortIndex portIndex) const override;                                          \
                                                                                                                                                \
      public:                                                                                                                                   \
        void onNodeHoverEnter() override;                                                                                                       \
        void onNodeHoverLeave() override;                                                                                                       \
        virtual void setInData(std::shared_ptr<NodeData> nodeData, PortIndex port) override;                                                    \
        virtual void setInData(std::vector<std::shared_ptr<NodeData>> nodeData, PortIndex port) override;                                       \
        virtual std::shared_ptr<NodeData> outData(PortIndex port) override;                                                                     \
        inline QWidget *embeddedWidget() override                                                                                               \
        {                                                                                                                                       \
            return widget;                                                                                                                      \
        }                                                                                                                                       \
        inline std::unique_ptr<NodeDataModel> clone() const override                                                                            \
        {                                                                                                                                       \
            return {};                                                                                                                          \
        };                                                                                                                                      \
                                                                                                                                                \
        void inputConnectionCreated(const QtNodes::Connection &) override;                                                                      \
        void inputConnectionDeleted(const QtNodes::Connection &) override;                                                                      \
        void outputConnectionCreated(const QtNodes::Connection &) override;                                                                     \
        void outputConnectionDeleted(const QtNodes::Connection &) override;                                                                     \
                                                                                                                                                \
      private:                                                                                                                                  \
        std::shared_ptr<CONTENT_TYPE> dataptr;                                                                                                  \
        QvNodeWidget *widget;                                                                                                                   \
        std::shared_ptr<NodeDispatcher> dispatcher;                                                                                             \
    }

    DECL_NODE_DATA_MODEL(InboundNodeModel, INBOUND);
    DECL_NODE_DATA_MODEL(OutboundNodeModel, OutboundObjectMeta);
    DECL_NODE_DATA_MODEL(RuleNodeModel, RuleObject);

} // namespace Qv2ray::ui::nodemodels

using namespace Qv2ray::ui::nodemodels;
