#pragma once

#include "ui/messaging/QvMessageBus.hpp"
#include "ui/node/NodeBase.hpp"
#include "ui_ChainEditorWidget.h"

#include <nodes/FlowScene>
#include <nodes/FlowView>

class ChainEditorWidget
    : public QWidget
    , private Ui::ChainEditorWidget
{
    Q_OBJECT

  public:
    explicit ChainEditorWidget(QWidget *parent = nullptr);
    auto getScene()
    {
        return scene;
    }

  protected:
    void changeEvent(QEvent *e);

  private:
    void updateColorScheme(){};
    QvMessageBusSlotDecl;

  private:
    QtNodes::FlowScene *scene;
    QtNodes::FlowView *view;
};
