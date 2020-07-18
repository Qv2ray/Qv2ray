#pragma once

#include "ui/node/NodeBase.hpp"
#include "ui_InboundWidget.h"

class InboundWidget
    : public QvNodeWidget
    , private Ui::InboundWidget
{
    Q_OBJECT

  public:
    explicit InboundWidget(QWidget *parent = nullptr);
    void setValue(std::shared_ptr<INBOUND> data);

  protected:
    void changeEvent(QEvent *e) override;
};
