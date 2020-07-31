#pragma once

#include "ui/messaging/QvMessageBus.hpp"
#include "ui/node/NodeBase.hpp"
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

  private slots:
    void on_chainComboBox_currentIndexChanged(const QString &arg1);
    void OnDispatcherOutboundCreated(std::shared_ptr<OutboundObjectMeta>, QtNodes::Node &);

  private:
    void updateColorScheme(){};
    QvMessageBusSlotDecl;

  private:
    QMap<QString, QUuid> chainOutbounds;
    QtNodes::FlowScene *scene;
    QtNodes::FlowView *view;
    std::shared_ptr<NodeDispatcher> dispatcher;
};
