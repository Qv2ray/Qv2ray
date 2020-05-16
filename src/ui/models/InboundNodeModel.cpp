#include "InboundNodeModel.hpp"

QvInboundNodeModel::QvInboundNodeModel(std::shared_ptr<InboundNodeData> data) : NodeDataModel()
{
    _in = data;
    _label = new QLabel();
    //
    QFont font = _label->font();
    font.setPointSize(font.pointSize() + GRAPH_NODE_LABEL_FONTSIZE_INCREMENT);
    _label->setFont(font);
    //
    _label->setText(data->GetInbound());
    _label->setWindowFlags(Qt::FramelessWindowHint);
    _label->setAttribute(Qt::WA_TranslucentBackground);
}
