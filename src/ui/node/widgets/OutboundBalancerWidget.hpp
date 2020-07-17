#pragma once

#include "ui_OutboundBalancerWidget.h"

class OutboundBalancerWidget
    : public QWidget
    , private Ui::OutboundBalancerWidget
{
    Q_OBJECT

  public:
    explicit OutboundBalancerWidget(QWidget *parent = nullptr);
    void on_balancerAddBtn_clicked();

    void on_balancerDelBtn_clicked();

  protected:
    void changeEvent(QEvent *e);
    QStringList targetList;
};
