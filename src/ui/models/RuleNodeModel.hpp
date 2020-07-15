#pragma once

#include "ui/models/NodeModelsBase.hpp"
#include "ui/widgets/node/RuleWidget.hpp"

#include <QtCore/qglobal.h>

class QvRuleNodeModel : public NodeDataModel
{
    Q_OBJECT
  public:
    QvRuleNodeModel(std::shared_ptr<RuleNodeData> data);
    ~QvRuleNodeModel(){};

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
        if (portType == PortType::In || portType == PortType::Out)
            return 1;
        return 0;
    }

    QString name() const override
    {
        return "RuleNode";
    }

    std::shared_ptr<NodeDataType> dataType(PortType portType, PortIndex) const override
    {
        switch (portType)
        {
            case PortType::In: return NODE_TYPE_INBOUND;
            case PortType::Out: return NODE_TYPE_OUTBOUND;
            default: return {};
        }
    }

    std::shared_ptr<NodeData> outData(PortIndex) override
    {
        return _ruleTag;
    }

    void setInData(std::shared_ptr<NodeData>, int) override{};
    void setInData(std::vector<std::shared_ptr<NodeData>>, int) override{};
    void setData(std::shared_ptr<RuleObject> data)
    {
        _ruleTag = std::make_shared<RuleNodeData>(data);
        ruleWidget->adjustSize();
    }

    QWidget *embeddedWidget() override
    {
        return ruleWidget;
    }

    ConnectionPolicy portInConnectionPolicy(PortIndex) const override
    {
        return ConnectionPolicy::Many;
    }

    ConnectionPolicy portOutConnectionPolicy(PortIndex) const override
    {
        return ConnectionPolicy::One;
    }
    std::unique_ptr<NodeDataModel> clone() const override
    {
        return std::make_unique<QvRuleNodeModel>(_ruleTag);
    }

  private:
    NodeValidationState modelValidationState = NodeValidationState::Warning;
    QString modelValidationError = tr("Missing or incorrect inputs");
    //
    std::shared_ptr<RuleNodeData> _ruleTag;
    QvNodeRuleWidget *ruleWidget;
};
