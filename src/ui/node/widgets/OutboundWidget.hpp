#pragma once

#include "ui/common/UIBase.hpp"
#include "ui/node/NodeBase.hpp"
#include "ui_OutboundWidget.h"

class OutboundWidget
    : public QvNodeWidget
    , private Ui::OutboundWidget
{
    Q_OBJECT

  public:
    explicit OutboundWidget(QWidget *parent = nullptr);
    void setValue(std::shared_ptr<OutboundObjectMeta> data);

  protected:
    void changeEvent(QEvent *e) override;

  private slots:
    void on_editBtn_clicked();
    void on_editJsonBtn_clicked();
    void on_tagTxt_textEdited(const QString &arg1);

  private:
    std::shared_ptr<OutboundObjectMeta> outboundObject;
};
