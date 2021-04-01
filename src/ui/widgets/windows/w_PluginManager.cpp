#include "w_PluginManager.hpp"

#include "components/plugins/QvPluginHost.hpp"
#include "core/settings/SettingsBackend.hpp"
#include "ui/widgets/editors/w_JsonEditor.hpp"
#include "utils/QvHelpers.hpp"

#include <QDesktopServices>

PluginManageWindow::PluginManageWindow(QWidget *parent) : QvDialog("PluginManager", parent)
{
    addStateOptions("width", { [&] { return width(); }, [&](QJsonValue val) { resize(val.toInt(), size().height()); } });
    addStateOptions("height", { [&] { return height(); }, [&](QJsonValue val) { resize(size().width(), val.toInt()); } });
    addStateOptions("x", { [&] { return x(); }, [&](QJsonValue val) { move(val.toInt(), y()); } });
    addStateOptions("y", { [&] { return y(); }, [&](QJsonValue val) { move(x(), val.toInt()); } });

    setupUi(this);
    for (auto &plugin : PluginHost->AllPlugins())
    {
        const auto &info = PluginHost->GetPlugin(plugin)->metadata;
        auto item = new QListWidgetItem(pluginListWidget);
        item->setCheckState(PluginHost->GetPluginEnabled(info.InternalName) ? Qt::Checked : Qt::Unchecked);
        item->setData(Qt::UserRole, info.InternalName);
        item->setText(info.Name + " (" + (PluginHost->GetPlugin(info.InternalName)->isLoaded ? tr("Loaded") : tr("Not loaded")) + ")");
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
        currentPluginInfo->pluginInterface->UpdateSettings(currentSettingsWidget->GetSettings());
        pluginSettingsLayout->removeWidget(currentSettingsWidget.get());
        currentSettingsWidget.reset();
    }
    pluginIconLabel->clear();
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

    if (currentPluginInfo->hasComponent(COMPONENT_GUI))
    {
        const auto pluginUIInterface = currentPluginInfo->pluginInterface->GetGUIInterface();
        pluginGuiComponentsLabel->setText(GetPluginComponentsString(pluginUIInterface->GetComponents()).join(NEWLINE));
        pluginIconLabel->setPixmap(pluginUIInterface->Icon().pixmap(pluginIconLabel->size() * devicePixelRatio()));
        if (pluginUIInterface->GetComponents().contains(GUI_COMPONENT_SETTINGS))
        {
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
    PluginHost->SetPluginEnabled(pluginInternalName, isEnabled);
    const auto info = PluginHost->GetPlugin(pluginInternalName);
    item->setText(info->metadata.Name + " (" + (info->isLoaded ? tr("Loaded") : tr("Not loaded")) + ")");
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
    QDesktopServices::openUrl(QUrl::fromLocalFile(pluginPath.absolutePath()));
}

void PluginManageWindow::on_toolButton_clicked()
{
    QDesktopServices::openUrl(QUrl("https://qv2ray.net/plugins/"));
}
