#pragma once

#include "base/Qv2rayBase.hpp"
#include "ui/common/UIBase.hpp"
#include "ui_RoutingEditorWidget.h"

#include <nodes/FlowScene>
#include <nodes/FlowView>

class RoutingEditorWidget
    : public QWidget
    , private Ui::RoutingEditorWidget
{
    Q_OBJECT

  public:
    explicit RoutingEditorWidget(QWidget *parent = nullptr);
    auto getScene()
    {
        return scene;
    }

  protected:
    void changeEvent(QEvent *e);

  private:
    void updateColorScheme(){};

  private:
    QtNodes::FlowScene *scene;
    QtNodes::FlowView *view;
};
