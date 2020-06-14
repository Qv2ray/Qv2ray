#pragma once

#include "ui/common/QvDialog.hpp"
#include "ui_w_PluginManager.h"

#include <memory>

class PluginManageWindow
    : public QvDialog
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
    void on_pluginEditSettingsJsonBtn_clicked();
    void on_pluginListWidget_itemSelectionChanged();
    void on_openPluginFolder_clicked();
    void on_toolButton_clicked();

  private:
    void processCommands(const QStringList &commands, const QMap<QString, QString> &args)
    {
        Q_UNUSED(commands)
        Q_UNUSED(args)
    }
    void updateColorScheme(){};
    std::unique_ptr<QWidget> settingsWidget;
    bool isLoading = true;
};
