#pragma once

#include "QvGUIPluginInterface.hpp"
#include "ui_MainWindowWidget.h"

class MainWindowWidget
    : public Qv2rayPlugin::PluginMainWindowWidget
    , private Ui::MainWindowWidget
{
    Q_OBJECT

  public:
    explicit MainWindowWidget(QWidget *parent = nullptr);

  protected:
    void changeEvent(QEvent *e) override;
};
