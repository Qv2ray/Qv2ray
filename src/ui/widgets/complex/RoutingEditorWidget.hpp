#pragma once

#include "ui_RoutingEditorWidget.h"

#include <nodes/FlowView>

class RoutingEditorWidget
    : public QtNodes::FlowView
    , private Ui::RoutingEditorWidget
{
    Q_OBJECT

  public:
    explicit RoutingEditorWidget(QWidget *parent = nullptr);
    auto scene()
    {
        return QtNodes::FlowView::scene();
    }

  protected:
    void changeEvent(QEvent *e);
};
