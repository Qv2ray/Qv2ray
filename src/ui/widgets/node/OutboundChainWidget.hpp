#pragma once

#include "ui_OutboundChainWidget.h"

class OutboundChainWidget
    : public QWidget
    , private Ui::OutboundChainWidget
{
    Q_OBJECT

  public:
    explicit OutboundChainWidget(QWidget *parent = nullptr);

  protected:
    void changeEvent(QEvent *e);
    QStringList targetList;
};
