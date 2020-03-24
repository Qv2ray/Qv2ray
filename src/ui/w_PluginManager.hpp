#pragma once

#include "ui_w_PluginManager.h"

#include <QDialog>

class PluginManageWindow
    : public QDialog
    , private Ui::w_PluginManager
{
    Q_OBJECT

  public:
    explicit PluginManageWindow(QWidget *parent = nullptr);
    ~PluginManageWindow();
  private slots:
    void on_pluginListWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
    void on_pluginListWidget_itemClicked(QListWidgetItem *item);
    void on_pluginListWidget_itemChanged(QListWidgetItem *item);

    void on_pluginSettingsBtn_clicked();

  private:
    bool isLoading = true;
};
