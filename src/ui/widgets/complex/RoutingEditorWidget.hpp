#pragma once

#include "ui_RoutingEditorWidget.h"

class RoutingEditorWidget
    : public QWidget
    , private Ui::RoutingEditorWidget
{
    Q_OBJECT

  public:
    explicit RoutingEditorWidget(QWidget *parent = nullptr);

  protected:
    void changeEvent(QEvent *e);
};
