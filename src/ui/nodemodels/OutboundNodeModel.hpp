#ifndef QVOUTBOUNDNODEMODEL_HPP
#define QVOUTBOUNDNODEMODEL_HPP

#include <QtCore/qglobal.h>
#include "ui/nodemodels/NodeModelsBase.hpp"

class QvOutboundNodeModel : public NodeDataModel
{
        Q_OBJECT
    public:
        explicit QvOutboundNodeModel(std::shared_ptr<OutboundNodeData> data);

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

        NodeDataType dataType(PortType portType, PortIndex portIndex) const override
        {
            Q_UNUSED(portType);
            Q_UNUSED(portIndex);
            return outboundType;
        }

        std::shared_ptr<NodeData> outData(PortIndex) override
        {
            return _out;
        }

        void setInData(shared_ptr<NodeData>, int) override {}

        void setInData(vector<shared_ptr<NodeData>>, int) override {}
        void setData(const QString &data)
        {
            _out = make_shared<OutboundNodeData>(data);
            _label->setText(_out->GetOutbound());
        }

        QWidget *embeddedWidget() override
        {
            return _label;
        }

        ConnectionPolicy portInConnectionPolicy(PortIndex) const override
        {
            return ConnectionPolicy::Many;
        }
    private:
        NodeValidationState modelValidationState = NodeValidationState::Warning;
        QString modelValidationError = tr("Missing or incorrect inputs");
        //
        std::shared_ptr<OutboundNodeData> _out;
        QLabel *_label;
};

#endif // QVOUTBOUNDNODEMODEL_HPP
