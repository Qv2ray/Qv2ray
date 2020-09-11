#include "w_PluginManager.hpp"

#include "components/plugins/QvPluginHost.hpp"
#include "core/settings/SettingsBackend.hpp"
#include "ui/widgets/editors/w_JsonEditor.hpp"
#include "utils/QvHelpers.hpp"

#include <QDesktopServices>

PluginManageWindow::PluginManageWindow(QWidget *parent) : QvDialog(parent)
{
    setupUi(this);
    for (auto &plugin : PluginHost->AvailablePlugins())
    {
        const auto &info = PluginHost->GetPlugin(plugin)->metadata;
        auto item = new QListWidgetItem(pluginListWidget);
        item->setCheckState(PluginHost->IsPluginEnabled(info.InternalName) ? Qt::Checked : Qt::Unchecked);
        item->setData(Qt::UserRole, info.InternalName);
        item->setText(info.Name + " (" + (PluginHost->GetPlugin(info.InternalName)->isLoaded ? tr("Loaded") : tr("Not loaded")) + ")");
        pluginListWidget->addItem(item);
    }
    isLoading = false;
    if (pluginListWidget->count() > 0)
        on_pluginListWidget_currentItemChanged(pluginListWidget->item(0), nullptr);
}

PluginManageWindow::~PluginManageWindow()
{
    DEBUG(MODULE_UI, "Plugin window destructor.")
    on_pluginListWidget_currentItemChanged(nullptr, nullptr);
}

void PluginManageWindow::on_pluginListWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    Q_UNUSED(previous)
    if (currentPluginInfo && currentSettingsWidget)
    {
        currentPluginInfo->pluginInterface->UpdateSettings(currentSettingsWidget->GetSettings());
        pluginSettingsLayout->removeWidget(currentSettingsWidget.get());
        currentSettingsWidget.reset();
    }

    if (!current)
        return;

    currentPluginInfo = PluginHost->GetPlugin(current->data(Qt::UserRole).toString());
    auto &info = currentPluginInfo->metadata;

    pluginNameLabel->setText(info.Name);
    pluginAuthorLabel->setText(info.Author);
    pluginDescriptionLabel->setText(info.Description);
    pluginLibPathLabel->setText(currentPluginInfo->libraryPath);
    pluginStateLabel->setText(currentPluginInfo->isLoaded ? tr("Loaded") : tr("Not loaded"));
    pluginComponentsLabel->setText(GetPluginComponentsString(info.Components).join(NEWLINE));

    if (!currentPluginInfo->isLoaded)
    {
        pluginUnloadLabel->setVisible(true);
        pluginUnloadLabel->setText(tr("Plugin Not Loaded"));
        return;
    }

    const auto pluginUIInterface = currentPluginInfo->pluginInterface->GetGUIInterface();
    Q_ASSERT_X(pluginUIInterface, "PluginManager", "PluginInterface must not be nullptr");
    if (pluginUIInterface->GetComponents().contains(GUI_COMPONENT_SETTINGS))
    {
        pluginGuiComponentsLabel->setText(GetPluginComponentsString(pluginUIInterface->GetComponents()).join(NEWLINE));
        pluginIconLabel->setPixmap({});
        pluginIconLabel->setPixmap(pluginUIInterface->Icon().pixmap(pluginIconLabel->size() * devicePixelRatio()));
        currentSettingsWidget = pluginUIInterface->GetSettingsWidget();
        currentSettingsWidget->SetSettings(currentPluginInfo->pluginInterface->GetSettngs());
        pluginUnloadLabel->setVisible(false);
        pluginSettingsLayout->addWidget(currentSettingsWidget.get());
    }
    else
    {
        pluginUnloadLabel->setVisible(true);
        pluginUnloadLabel->setText(tr("Plugin does not have settings widget."));
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
    PluginHost->SetIsPluginEnabled(pluginInternalName, isEnabled);
    const auto info = PluginHost->GetPlugin(pluginInternalName);
    item->setText(info->metadata.Name + " (" + (info->isLoaded ? tr("Loaded") : tr("Not loaded")) + ")");
    //
    if (!isEnabled)
    {
        QvMessageBoxInfo(this, tr("Disabling a plugin"), tr("This plugin will keep loaded until the next time Qv2ray starts."));
    }
}

void PluginManageWindow::on_pluginEditSettingsJsonBtn_clicked()
{
    if (const auto &current = pluginListWidget->currentItem(); current != nullptr)
    {
        const auto &info = PluginHost->GetPlugin(current->data(Qt::UserRole).toString());
        if (!info->isLoaded)
        {
            QvMessageBoxWarn(this, tr("Plugin not loaded"), tr("This plugin is not loaded, please enable or reload the plugin to continue."));
            return;
        }
        JsonEditor w(info->pluginInterface->GetSettngs());
        auto newConf = w.OpenEditor();
        if (w.result() == QDialog::Accepted)
        {
            info->pluginInterface->UpdateSettings(newConf);
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
#ifdef FALL_BACK_TO_XDG_OPEN
    QProcess::execute("xdg-open", { pluginPath.absolutePath() });
#else
    QDesktopServices::openUrl(QUrl::fromLocalFile(pluginPath.absolutePath()));
#endif
}

void PluginManageWindow::on_toolButton_clicked()
{
    auto address = GlobalConfig.uiConfig.language.contains("zh") ? "https://qv2ray.net/plugins/" : "https://qv2ray.net/en/plugins/";
    QDesktopServices::openUrl(QUrl(address));
}
