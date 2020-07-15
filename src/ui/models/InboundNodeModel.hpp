#pragma once

#include "NodeModelsBase.hpp"
#include "ui/widgets/node/InboundOutboundWidget.hpp"

#include <QtCore/qglobal.h>

class QvInboundNodeModel : public NodeDataModel
{
    Q_OBJECT
  public:
    explicit QvInboundNodeModel(std::shared_ptr<InboundNodeData> data);
    ~QvInboundNodeModel(){};

    QString caption() const override
    {
        return "Nothing";
    }

    bool captionVisible() const override
    {
        return false;
    }

    unsigned int nPorts(PortType portType) const override
    {
        return portType == PortType::Out ? 1 : 0;
    }

    QString name() const override
    {
        return "InboundNode";
    }

    std::shared_ptr<NodeDataType> dataType(PortType portType, PortIndex portIndex) const override
    {
        Q_UNUSED(portType)
        Q_UNUSED(portIndex)
        return NODE_TYPE_INBOUND;
    }

    std::shared_ptr<NodeData> outData(PortIndex) override
    {
        return _in;
    }

    void setInData(std::shared_ptr<NodeData>, int) override{};

    void setData(std::shared_ptr<INBOUND> data)
    {
        _in = std::make_shared<InboundNodeData>(data);
        widget->adjustSize();
    }

    QWidget *embeddedWidget() override
    {
        return widget;
    }
    std::unique_ptr<NodeDataModel> clone() const override
    {
        return std::make_unique<QvInboundNodeModel>(_in);
    }

  private:
    NodeValidationState modelValidationState = NodeValidationState::Warning;
    QString modelValidationError = tr("Missing or incorrect inputs");
    //
    std::shared_ptr<InboundNodeData> _in;
    InboundOutboundWidget *widget;
};
