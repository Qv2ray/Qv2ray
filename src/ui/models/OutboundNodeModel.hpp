#pragma once
#include "ui/models/NodeModelsBase.hpp"
#include "ui/widgets/node/InboundOutboundWidget.hpp"

#include <QtCore/qglobal.h>

class QvOutboundNodeModel : public NodeDataModel
{
    Q_OBJECT
  public:
    explicit QvOutboundNodeModel(std::shared_ptr<OutboundNodeData> data);
    ~QvOutboundNodeModel(){};
    void setInData(std::shared_ptr<NodeData>, int) override{};
    void setInData(std::vector<std::shared_ptr<NodeData>>, int) override{};

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
        return portType == PortType::In ? 1 : 0;
    }

    QString name() const override
    {
        return "OutboundNode";
    }

    std::shared_ptr<NodeDataType> dataType(PortType portType, PortIndex portIndex) const override
    {
        Q_UNUSED(portType)
        Q_UNUSED(portIndex)
        return NODE_TYPE_OUTBOUND;
    }

    std::shared_ptr<NodeData> outData(PortIndex) override
    {
        return _out;
    }

    void setData(std::shared_ptr<OUTBOUND> data)
    {
        _out = std::make_shared<OutboundNodeData>(data);
        widget->adjustSize();
    }

    QWidget *embeddedWidget() override
    {
        return nullptr;
    }

    ConnectionPolicy portInConnectionPolicy(PortIndex) const override
    {
        return ConnectionPolicy::Many;
    }
    std::unique_ptr<NodeDataModel> clone() const override
    {
        return std::make_unique<QvOutboundNodeModel>(_out);
    }

  private:
    NodeValidationState modelValidationState = NodeValidationState::Warning;
    QString modelValidationError = tr("Missing or incorrect inputs");
    //
    std::shared_ptr<OutboundNodeData> _out;
    InboundOutboundWidget *widget;
};
