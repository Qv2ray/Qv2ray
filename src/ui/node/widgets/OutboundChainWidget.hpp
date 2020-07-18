#pragma once

#include "ui/node/NodeBase.hpp"
#include "ui_OutboundChainWidget.h"

class OutboundChainWidget
    : public QvNodeWidget
    , private Ui::OutboundChainWidget
{
    Q_OBJECT

  public:
    explicit OutboundChainWidget(QWidget *parent = nullptr);
    void setValue(std::shared_ptr<OutboundObjectMeta> data);

  signals:
    void OnSizeUpdated();

  protected:
    void changeEvent(QEvent *e);
    QStringList targetList;
};
