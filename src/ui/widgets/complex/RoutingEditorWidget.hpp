#pragma once

#include "base/Qv2rayBase.hpp"
#include "ui/messaging/QvMessageBus.hpp"
#include "ui/node/NodeBase.hpp"
#include "ui_RoutingEditorWidget.h"

#include <nodes/FlowScene>
#include <nodes/FlowView>

class NodeDispatcher;

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
    std::shared_ptr<NodeDispatcher> GetDispatcher() const
    {
        return dispatcher;
    }

  protected:
    void changeEvent(QEvent *e);

  private slots:
    void OnDispatcherInboundCreated(std::shared_ptr<INBOUND>, QtNodes::Node &);
    void OnDispatcherOutboundCreated(std::shared_ptr<complex::OutboundObjectMeta>, QtNodes::Node &);
    void OnDispatcherRuleCreated(std::shared_ptr<RuleObject>, QtNodes::Node &);

    void on_addRouteBtn_clicked();

    void on_delBtn_clicked();

  private:
    void updateColorScheme();
    QvMessageBusSlotDecl;

  private:
    QtNodes::FlowScene *scene;
    QtNodes::FlowView *view;
    std::shared_ptr<NodeDispatcher> dispatcher;
};
