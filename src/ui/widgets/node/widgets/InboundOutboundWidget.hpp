#pragma once

#include "ui/widgets/node/NodeBase.hpp"
#include "ui_InboundOutboundWidget.h"

class InboundOutboundWidget
    : public QvNodeWidget
    , private Ui::InboundOutboundWidget
{
    Q_OBJECT

  public:
    explicit InboundOutboundWidget(ComplexTagNodeMode mode, std::shared_ptr<NodeDispatcher> _dispatcher, QWidget *parent = nullptr);
    void setValue(std::shared_ptr<INBOUND>);
    void setValue(std::shared_ptr<OutboundObjectMeta> data);

  protected:
    void changeEvent(QEvent *e) override;

  private slots:
    void on_editBtn_clicked();
    void on_editJsonBtn_clicked();
    void on_tagTxt_textEdited(const QString &arg1);

  private:
    ComplexTagNodeMode workingMode;
    std::shared_ptr<INBOUND> inboundObject;
    std::shared_ptr<OutboundObjectMeta> outboundObject;
    bool isExternalOutbound = false;

  private:
    const QString editExternalMsg = tr("You are trying to edit an external connection config, is this what you want?");
    const QString editExternalComplexMsg = tr("You have selected an complex config as outbound.") + NEWLINE +
                                           tr("continuing editing this configuration will make you LOSS ALL INBOUND AND ROUTING settings.") +
                                           NEWLINE + tr("Is this what you want?");
};
