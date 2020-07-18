#pragma once

#include "ui/node/NodeBase.hpp"
#include "ui_OutboundBalancerWidget.h"

class OutboundBalancerWidget
    : public QvNodeWidget
    , private Ui::OutboundBalancerWidget
{
    Q_OBJECT

  public:
    explicit OutboundBalancerWidget(QWidget *parent = nullptr);
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
