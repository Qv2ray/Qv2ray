#pragma once

#include "ui_InboundWidget.h"

class InboundWidget
    : public QWidget
    , private Ui::InboundWidget
{
    Q_OBJECT

  public:
    explicit InboundWidget(QWidget *parent = nullptr);

  signals:
    void OnSizeUpdated();

  protected:
    void changeEvent(QEvent *e);
};
