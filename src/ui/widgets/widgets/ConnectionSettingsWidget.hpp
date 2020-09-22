#pragma once

#include "ui_ConnectionSettingsWidget.h"

class ConnectionSettingsWidget
    : public QWidget
    , private Ui::ConnectionSettingsWidget
{
    Q_OBJECT

  public:
    explicit ConnectionSettingsWidget(QWidget *parent = nullptr);

  protected:
    void changeEvent(QEvent *e);
};
