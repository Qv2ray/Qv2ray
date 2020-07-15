#pragma once

#include "ui_InboundOutboundWidget.h"

class InboundOutboundWidget
    : public QWidget
    , private Ui::InboundOutboundWidget
{
    Q_OBJECT

  public:
    explicit InboundOutboundWidget(QWidget *parent = nullptr);

  signals:
    void OnSizeUpdated();

  protected:
    void changeEvent(QEvent *e);
};
