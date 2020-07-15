#pragma once

#include "ui_OutboundBalancerWidget.h"

class OutboundBalancerWidget
    : public QWidget
    , private Ui::OutboundBalancerWidget
{
    Q_OBJECT

  public:
    explicit OutboundBalancerWidget(QWidget *parent = nullptr);

  protected:
    void changeEvent(QEvent *e);
};
