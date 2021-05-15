#include "w_PluginManager.hpp"

#include "components/plugins/PluginAPIHost.hpp"
#include "core/settings/SettingsBackend.hpp"
#include "ui/widgets/editors/w_JsonEditor.hpp"
#include "utils/QvHelpers.hpp"

#include <QDesktopServices>

PluginManageWindow::PluginManageWindow(QWidget *parent) : QvDialog("PluginManager", parent)
{
    setupUi(this);
    for (const auto &plugin : PluginHost->AllPlugins())
    {
        plugins[plugin->metadata().InternalName] = plugin;
        auto item = new QListWidgetItem(pluginListWidget);
        item->setCheckState(plugin->isEnabled() ? Qt::Checked : Qt::Unchecked);
        item->setData(Qt::UserRole, plugin->metadata().InternalName);
        item->setText(plugin->metadata().Name);
        pluginListWidget->addItem(item);
    }
    pluginListWidget->sortItems();
    isLoading = false;
    if (pluginListWidget->count() > 0)
        on_pluginListWidget_currentItemChanged(pluginListWidget->item(0), nullptr);
}

QvMessageBusSlotImpl(PluginManageWindow){ Q_UNUSED(msg) }

PluginManageWindow::~PluginManageWindow()
{
    on_pluginListWidget_currentItemChanged(nullptr, nullptr);
}

void PluginManageWindow::on_pluginListWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    Q_UNUSED(previous)
    if (currentPluginInfo && currentSettingsWidget)
    {
        PluginHost->SetPluginSettings(currentPluginInfo->metadata().InternalName, currentSettingsWidget->GetSettings());
        pluginSettingsLayout->removeWidget(currentSettingsWidget.get());
        currentSettingsWidget.reset();
    }
    pluginIconLabel->clear();
    if (!current)
        return;

    currentPluginInfo = plugins[current->data(Qt::UserRole).toString()];
    const auto &metadata = currentPluginInfo->metadata();

    pluginNameLabel->setText(metadata.Name);
    pluginAuthorLabel->setText(metadata.Author);
    pluginDescriptionLabel->setText(metadata.Description);
    pluginLibPathLabel->setText(currentPluginInfo->libraryPath);
    pluginStateLabel->setText(currentPluginInfo->isEnabled() ? tr("Enabled") : tr("Disabled"));
    pluginComponentsLabel->setText(GetPluginComponentsString(metadata.Components).join(NEWLINE));

    if (currentPluginInfo->hasComponent(COMPONENT_GUI))
    {
        const auto pluginUIInterface = currentPluginInfo->pinterface->GetGUIInterface();
        pluginGuiComponentsLabel->setText(GetPluginComponentsString(pluginUIInterface->GetComponents()).join(NEWLINE));
        pluginIconLabel->setPixmap(pluginUIInterface->Icon().pixmap(pluginIconLabel->size() * devicePixelRatio()));
        if (pluginUIInterface->GetComponents().contains(GUI_COMPONENT_SETTINGS))
        {
            currentSettingsWidget = pluginUIInterface->GetSettingsWidget();
            currentSettingsWidget->SetSettings(currentPluginInfo->pinterface->GetSettings());
            pluginUnloadLabel->setVisible(false);
            pluginSettingsLayout->addWidget(currentSettingsWidget.get());
        }
        else
        {
            pluginUnloadLabel->setVisible(true);
            pluginUnloadLabel->setText(tr("Plugin does not have settings widget."));
        }
    }
    else
    {
        pluginGuiComponentsLabel->setText(tr("None"));
    }
}

void PluginManageWindow::on_pluginListWidget_itemClicked(QListWidgetItem *item)
{
    Q_UNUSED(item)
    // on_pluginListWidget_currentItemChanged(item, nullptr);
}

void PluginManageWindow::on_pluginListWidget_itemChanged(QListWidgetItem *item)
{
    if (isLoading)
        return;
    bool isEnabled = item->checkState() == Qt::Checked;
    const auto pluginInternalName = item->data(Qt::UserRole).toString();
    plugins[pluginInternalName]->setEnabled(isEnabled);
}

void PluginManageWindow::on_pluginEditSettingsJsonBtn_clicked()
{
    if (const auto &current = pluginListWidget->currentItem(); current != nullptr)
    {
        const auto pluginName = current->data(Qt::UserRole).toString();

        JsonEditor w(plugins[pluginName]->pinterface->GetSettings());
        auto newConf = w.OpenEditor();
        if (w.result() == QDialog::Accepted)
        {
            PluginHost->SetPluginSettings(pluginName, newConf);
        }
    }
}

void PluginManageWindow::on_pluginListWidget_itemSelectionChanged()
{
    auto needEnable = !pluginListWidget->selectedItems().isEmpty();
    pluginEditSettingsJsonBtn->setEnabled(needEnable);
}

void PluginManageWindow::on_openPluginFolder_clicked()
{
    QDir pluginPath(QV2RAY_CONFIG_DIR + "plugins/");
    if (!pluginPath.exists())
    {
        pluginPath.mkpath(QV2RAY_CONFIG_DIR + "plugins/");
    }
    QDesktopServices::openUrl(QUrl::fromLocalFile(pluginPath.absolutePath()));
}

void PluginManageWindow::on_toolButton_clicked()
{
    QDesktopServices::openUrl(QUrl("https://qv2ray.net/plugins/"));
}
