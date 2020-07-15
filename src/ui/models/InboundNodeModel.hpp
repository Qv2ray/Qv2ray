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
        return {};
    }

    bool captionVisible() const override
    {
        return false;
    }

    unsigned int nPorts(PortType portType) const override;
    QString name() const override;
    std::shared_ptr<NodeDataType> dataType(PortType portType, PortIndex portIndex) const override;
    std::shared_ptr<NodeData> outData(PortIndex) override;
    void setInData(std::shared_ptr<NodeData>, int) override{};
    void setData(std::shared_ptr<INBOUND> data);

    QWidget *embeddedWidget() override
    {
        return widget;
    }
    std::unique_ptr<NodeDataModel> clone() const override;

  private:
    NodeValidationState modelValidationState = NodeValidationState::Warning;
    QString modelValidationError = tr("Missing or incorrect inputs");
    //
    std::shared_ptr<InboundNodeData> _in;
    InboundOutboundWidget *widget;
};
