#pragma once

#include "ui/node/NodeBase.hpp"
#include "ui_ChainEditorWidget.h"

#include <nodes/FlowView>

class ChainEditorWidget
    : public QtNodes::FlowView
    , private Ui::ChainEditorWidget
{
    Q_OBJECT

  public:
    explicit ChainEditorWidget(QWidget *parent = nullptr);
    auto scene()
    {
        return QtNodes::FlowView::scene();
    }

  protected:
    void changeEvent(QEvent *e);
};
