#pragma once

#include "ui/common/UIBase.hpp"
#include "ui/node/NodeBase.hpp"
#include "ui_InboundOutboundWidget.h"

class InboundOutboundWidget
    : public QvNodeWidget
    , private Ui::InboundOutboundWidget
{
    Q_OBJECT

  public:
    enum WidgetMode
    {
        MODE_INBOUND,
        MODE_OUTBOUND
    };

    explicit InboundOutboundWidget(WidgetMode mode, std::shared_ptr<NodeDispatcher> _dispatcher, QWidget *parent = nullptr);
    void setValue(std::shared_ptr<INBOUND>);
    void setValue(std::shared_ptr<OutboundObjectMeta> data);

  protected:
    void changeEvent(QEvent *e) override;

  private slots:
    void on_editBtn_clicked();
    void on_editJsonBtn_clicked();
    void on_tagTxt_textEdited(const QString &arg1);

  private:
    WidgetMode workingMode;
    std::shared_ptr<INBOUND> inboundObject;
    std::shared_ptr<OutboundObjectMeta> outboundObject;
};
