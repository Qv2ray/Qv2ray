#pragma once

#include "base/Qv2rayBase.hpp"
#include "ui/common/QvMessageBus.hpp"
#include "ui_w_ChainSha256Editor.h"

#include <QDialog>

class ChainSha256Editor
    : public QDialog
    , private Ui::ChainSha256Editor
{
    Q_OBJECT

  public:
    explicit ChainSha256Editor(QString raw, QWidget *parent = nullptr);
    ~ChainSha256Editor();
    QString OpenEditor();

  private:
    QvMessageBusSlotDecl;
  private slots:
    void on_chainSha256Edit_textChanged();

  private:
    QString original;
    QString final;
};
