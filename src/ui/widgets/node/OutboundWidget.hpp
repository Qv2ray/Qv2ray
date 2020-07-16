#pragma once

#include "ui_OutboundWidget.h"

class OutboundWidget
    : public QWidget
    , private Ui::OutboundWidget
{
    Q_OBJECT

  public:
    explicit OutboundWidget(QWidget *parent = nullptr);
    void on_balancerAddBtn_clicked();

    void on_balancerDelBtn_clicked();

  protected:
    void changeEvent(QEvent *e);
    QStringList targetList;
};
