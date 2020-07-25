#pragma once

#include "ui/messaging/QvMessageBus.hpp"
#include "ui_ChainEditorWidget.h"

#include <nodes/FlowScene>
#include <nodes/FlowView>

class NodeDispatcher;

class ChainEditorWidget
    : public QWidget
    , private Ui::ChainEditorWidget
{
    Q_OBJECT

  public:
    explicit ChainEditorWidget(std::shared_ptr<NodeDispatcher> dispatcher, QWidget *parent = nullptr);
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
    std::shared_ptr<NodeDispatcher> dispatcher;
};
