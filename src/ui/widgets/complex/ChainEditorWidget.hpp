#pragma once

#include "ui/node/NodeBase.hpp"
#include "ui_ChainEditorWidget.h"

namespace QtNodes
{
    class FlowView;
}

class ChainEditorWidget
    : public QWidget
    , private Ui::ChainEditorWidget
{
    Q_OBJECT

  public:
    explicit ChainEditorWidget(QWidget *parent = nullptr);

  protected:
    void changeEvent(QEvent *e);

  private:
    //
    QtNodes::FlowScene *chainScene;
    QtNodes::FlowView *chainView;
};
