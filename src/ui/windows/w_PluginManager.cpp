#include "w_PluginManager.hpp"

#include "GuiPluginHost/GuiPluginHost.hpp"
#include "Qv2rayBase/Common/Utils.hpp"
#include "Qv2rayBase/Interfaces/IStorageProvider.hpp"
#include "Qv2rayBase/Plugin/PluginAPIHost.hpp"

PluginManageWindow::PluginManageWindow(QWidget *parent) : QvDialog(u"PluginManager"_qs, parent)
{
    setupUi(this);
    userPluginDirLabel->setText(QvStorageProvider->GetUserPluginDirectory().absolutePath());
    for (const auto &plugin : QvPluginManagerCore->AllPlugins())
    {
        plugins[plugin->metadata().InternalID] = plugin;
        auto item = new QListWidgetItem(pluginListWidget);
        item->setCheckState(QvPluginManagerCore->GetPluginEnabled(plugin->id()) ? Qt::Checked : Qt::Unchecked);
        item->setData(Qt::UserRole, plugin->metadata().InternalID.toString());
        item->setText(plugin->metadata().Name);

        item->setIcon(QvApp->Qv2rayLogo);
        if (plugin->hasComponent(Qv2rayPlugin::COMPONENT_GUI))
            if (const auto icon = plugin->pinterface->GetGUIInterface()->Icon(); !icon.isNull())
                item->setIcon(icon);

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
        currentSettingsWidget->Store();
        QvPluginManagerCore->SetPluginSettings(currentPluginInfo->id(), currentSettingsWidget->settings);
        pluginSettingsLayout->removeWidget(currentSettingsWidget.get());
        currentSettingsWidget.reset();
    }

    if (!current)
        return;

    const auto pid = PluginId{ current->data(Qt::UserRole).toString() };
    if (!plugins.contains(pid))
        return;
    currentPluginInfo = plugins[pid];
    const auto &metadata = currentPluginInfo->metadata();

    pluginNameLabel->setText(metadata.Name);
    pluginAuthorLabel->setText(metadata.Author);
    pluginDescriptionLabel->setText(metadata.Description);
    pluginLibPathLabel->setText(currentPluginInfo->libraryPath);
    auto components = Qv2rayBase::Plugin::GetPluginComponentsString(metadata.Components);

    bool hasSettings = false;

    if (currentPluginInfo->hasComponent(Qv2rayPlugin::COMPONENT_GUI))
    {
        const auto pluginUIInterface = currentPluginInfo->pinterface->GetGUIInterface();
        components << Qv2rayBase::Plugin::GetPluginComponentsString(pluginUIInterface->GetComponents());
        if (pluginUIInterface->GetComponents().contains(Qv2rayPlugin::GUI_COMPONENT_SETTINGS))
        {
            currentSettingsWidget = pluginUIInterface->GetSettingsWidget();
            currentSettingsWidget->settings = currentPluginInfo->pinterface->GetSettings();
            currentSettingsWidget->Load();
            pluginSettingsLayout->addWidget(currentSettingsWidget.get());
            hasSettings = true;
        }
    }

    pluginUnloadLabel->setVisible(!hasSettings);
    if (!hasSettings)
        pluginUnloadLabel->setText(currentPluginInfo->metadata().Name + NEWLINE NEWLINE +       //
                                   tr("This plugin does not have tunable options.") + NEWLINE + //
                                   tr("Try something else?"));

    pluginComponentsLabel->setText(components.join('\n'));
}

void PluginManageWindow::on_pluginListWidget_itemChanged(QListWidgetItem *item)
{
    if (isLoading)
        return;
    bool isEnabled = item->checkState() == Qt::Checked;
    const auto pluginInternalName = PluginId{ item->data(Qt::UserRole).toString() };
    QvPluginManagerCore->SetPluginEnabled(pluginInternalName, isEnabled);
}

void PluginManageWindow::on_openPluginFolder_clicked()
{
    const auto dir = QvStorageProvider->GetUserPluginDirectory();
    QvBaselib->OpenURL(QUrl::fromLocalFile(dir.absolutePath()));
}
