#pragma once

#include "ui/node/NodeBase.hpp"
#include "ui_ChainOutboundWidget.h"

class ChainOutboundWidget
    : public QvNodeWidget
    , private Ui::ChainOutboundWidget
{
    Q_OBJECT

  public:
    explicit ChainOutboundWidget(std::shared_ptr<NodeDispatcher> _dispatcher, QWidget *parent = nullptr);
    void setValue(std::shared_ptr<QString>);

  protected:
    void changeEvent(QEvent *e);
};
