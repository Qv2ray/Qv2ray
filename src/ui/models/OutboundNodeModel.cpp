#include "ui/models/OutboundNodeModel.hpp"

QvOutboundNodeModel::QvOutboundNodeModel(std::shared_ptr<OutboundNodeData> data): NodeDataModel()
{
    _out = data;
    _label = new QLabel();
    //
    QFont font = _label->font();
    font.setPointSize(font.pointSize() + GRAPH_NODE_LABEL_FONTSIZE_INCREMENT);
    _label->setFont(font);
    //
    _label->setText(data->GetOutbound());
    _label->setWindowFlags(Qt::FramelessWindowHint);
    _label->setAttribute(Qt::WA_TranslucentBackground);
}
