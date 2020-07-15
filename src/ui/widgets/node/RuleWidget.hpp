#pragma once

#include "ui_RuleWidget.h"

class QvNodeRuleWidget
    : public QWidget
    , private Ui::RuleWidget
{
    Q_OBJECT

  public:
    explicit QvNodeRuleWidget(QWidget *parent = nullptr);

  protected:
    void changeEvent(QEvent *e);
};
