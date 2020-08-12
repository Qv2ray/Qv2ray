#pragma once

#include "ui/widgets/node/NodeBase.hpp"
#include "ui_ChainWidget.h"

class ChainWidget
    : public QvNodeWidget
    , private Ui::ChainWidget
{
    Q_OBJECT

  public:
    explicit ChainWidget(std::shared_ptr<NodeDispatcher> _dispatcher, QWidget *parent = nullptr);
    void setValue(std::shared_ptr<OutboundObjectMeta> data);

  signals:
    void OnSizeUpdated();
    void OnEditChainRequested(const QString &id);

  protected:
    void changeEvent(QEvent *e);
    QStringList targetList;
  private slots:
    void on_displayNameTxt_textEdited(const QString &arg1);

    void on_editChainBtn_clicked();

  private:
    std::shared_ptr<OutboundObjectMeta> dataptr;
};
