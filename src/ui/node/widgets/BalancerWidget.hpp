#pragma once

#include "ui/node/NodeBase.hpp"
#include "ui_BalancerWidget.h"

class BalancerWidget
    : public QvNodeWidget
    , private Ui::BalancerWidget
{
    Q_OBJECT

  public:
    explicit BalancerWidget(std::shared_ptr<NodeDispatcher> _dispatcher, QWidget *parent = nullptr);
    void setValue(std::shared_ptr<OutboundObjectMeta> data);
  private slots:
    void on_balancerAddBtn_clicked();
    void on_balancerDelBtn_clicked();

  signals:
    void OnSizeUpdated();

  protected:
    void changeEvent(QEvent *e);
    std::shared_ptr<OutboundObjectMeta> outboundData;
    QStringList targetList;
};
