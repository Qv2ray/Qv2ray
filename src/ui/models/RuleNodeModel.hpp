#pragma once

#include <QtCore/qglobal.h>
#include "ui/models/NodeModelsBase.hpp"

class QvRuleNodeDataModel : public NodeDataModel
{
        Q_OBJECT
    public:
        QvRuleNodeDataModel(std::shared_ptr<RuleNodeData> data);
        ~QvRuleNodeDataModel()
        {
            //if (_label) {
            //    delete _label;
            //}
        }

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
            if (portType == PortType::In) {
                return 1;
            } else if (portType == PortType::Out) {
                return 1;
            } else {
                return 0;
            }
        }

        QString name() const override
        {
            return "RuleNode";
        }

        NodeDataType dataType(PortType portType, PortIndex portIndex) const override
        {
            Q_UNUSED(portIndex)

            switch (portType) {
                case PortType::In:
                    return inboundType;

                case PortType::Out:
                    return outboundType;

                case PortType::None:
                    break;
            }

            return NodeDataType();
        }

        std::shared_ptr<NodeData> outData(PortIndex port) override
        {
            Q_UNUSED(port)
            return _ruleTag;
        }

        void setInData(std::shared_ptr<NodeData>, int) override {}
        void setInData(vector<shared_ptr<NodeData>>, int) override {}
        void setData(const QString &data)
        {
            _ruleTag = make_shared<RuleNodeData>(data);
            _label->setText(_ruleTag->GetRuleTag());
            _label->adjustSize();
        }

        QWidget *embeddedWidget() override
        {
            return _label;
        }

        ConnectionPolicy portInConnectionPolicy(PortIndex) const override
        {
            return ConnectionPolicy::Many;
        }

        ConnectionPolicy portOutConnectionPolicy(PortIndex) const override
        {
            return ConnectionPolicy::One;
        }

    private:
        NodeValidationState modelValidationState = NodeValidationState::Warning;
        QString modelValidationError = tr("Missing or incorrect inputs");
        //
        shared_ptr<RuleNodeData> _ruleTag;
        QLabel *_label;
};
