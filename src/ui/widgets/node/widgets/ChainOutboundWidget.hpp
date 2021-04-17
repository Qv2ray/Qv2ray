#pragma once

#include "ui/widgets/node/NodeBase.hpp"
#include "ui_ChainOutboundWidget.h"

class ChainOutboundWidget
    : public QvNodeWidget
    , private Ui::ChainOutboundWidget
{
    Q_OBJECT

  public:
    explicit ChainOutboundWidget(std::shared_ptr<NodeDispatcher> _dispatcher, QWidget *parent = nullptr);
    void setValue(std::shared_ptr<OutboundObjectMeta>);

  protected:
    void changeEvent(QEvent *e);
  private slots:
    void on_chainPortSB_valueChanged(int arg1);

  private:
    std::shared_ptr<OutboundObjectMeta> chain;
};
