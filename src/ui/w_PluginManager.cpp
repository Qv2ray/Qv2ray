#include "w_PluginManager.hpp"

#include "components/plugins/QvPluginHost.hpp"

w_PluginManager::w_PluginManager(QWidget *parent) : QDialog(parent)
{
    setupUi(this);
    for (auto &plugin : PluginHost->AvailablePlugins())
    {
        auto item = new QListWidgetItem(pluginListWidget);
        item->setCheckState(Qt::CheckState::Unchecked);
        item->setData(Qt::UserRole, PluginHost->GetPluginInfo(plugin).interface->InternalName());
        item->setText(PluginHost->GetPluginInfo(plugin).interface->Name());
        pluginListWidget->addItem(item);
    }
}

w_PluginManager::~w_PluginManager()
{
}

void w_PluginManager::on_pluginListWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    Q_UNUSED(previous)
    auto &info = PluginHost->GetPluginInfo(current->data(Qt::UserRole).toString());
    if (info.interface != nullptr)
    {
        pluginNameLabel->setText(info.interface->Name());
        pluginAuthorLabel->setText(info.interface->Author());
        pluginDescriptionLabel->setText(info.interface->Description());
        pluginIconLabel->setPixmap(info.interface->Icon().pixmap(pluginIconLabel->size() * devicePixelRatio()));
        pluginTypeLabel->setText("No impl");
    }
}

void w_PluginManager::on_pluginListWidget_itemClicked(QListWidgetItem *item)
{
    on_pluginListWidget_currentItemChanged(item, nullptr);
}
