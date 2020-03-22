#pragma once

#include "ui_w_PluginManager.h"

#include <QDialog>

class w_PluginManager
    : public QDialog
    , private Ui::w_PluginManager
{
    Q_OBJECT

  public:
    explicit w_PluginManager(QWidget *parent = nullptr);
    ~w_PluginManager();
  private slots:
    void on_pluginListWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
    void on_pluginListWidget_itemClicked(QListWidgetItem *item);
};
