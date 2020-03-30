#include "w_PluginManager.hpp"

#include "common/QvHelpers.hpp"
#include "components/plugins/QvPluginHost.hpp"
#include "core/settings/SettingsBackend.hpp"
#include "ui/editors/w_JsonEditor.hpp"

PluginManageWindow::PluginManageWindow(QWidget *parent) : QDialog(parent)
{
    setupUi(this);
    for (auto &plugin : PluginHost->AvailablePlugins())
    {
        const auto &info = PluginHost->GetPluginInfo(plugin);
        auto item = new QListWidgetItem(pluginListWidget);
        item->setCheckState(PluginHost->GetPluginEnableState(info.pluginInterface->InternalName()) ? Qt::Checked : Qt::Unchecked);
        item->setData(Qt::UserRole, info.pluginInterface->InternalName());
        item->setText(info.pluginInterface->Name() + " (" + (info.isLoaded ? tr("Loaded") : tr("Not loaded")) + ")");
        pluginListWidget->addItem(item);
    }
    isLoading = false;
}

PluginManageWindow::~PluginManageWindow()
{
}

void PluginManageWindow::on_pluginListWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    Q_UNUSED(previous)
    auto &info = PluginHost->GetPluginInfo(current->data(Qt::UserRole).toString());
    if (info.pluginInterface)
    {
        pluginIconLabel->setPixmap(info.pluginInterface->Icon().pixmap(pluginIconLabel->size() * devicePixelRatio()));
        //
        pluginNameLabel->setText(info.pluginInterface->Name());
        pluginAuthorLabel->setText(info.pluginInterface->Author());
        pluginDescriptionLabel->setText(info.pluginInterface->Description());
        pluginLibPathLabel->setText(info.libraryPath);
        pluginStateLabel->setText(info.isLoaded ? tr("Loaded") : tr("Not loaded"));
        pluginTypeLabel->setText(PluginHost->GetPluginTypeString(info.pluginInterface->InternalName()));
        pluginHookTypeLabel->setText(PluginHost->GetPluginHookTypeString(info.pluginInterface->InternalName()));
        pluginErrMessageTxt->setPlainText(info.errorMessage.isEmpty() ? "OK" : info.errorMessage);
    }
}

void PluginManageWindow::on_pluginListWidget_itemClicked(QListWidgetItem *item)
{
    on_pluginListWidget_currentItemChanged(item, nullptr);
}

void PluginManageWindow::on_pluginListWidget_itemChanged(QListWidgetItem *item)
{
    if (isLoading)
        return;
    bool isEnabled = item->checkState() == Qt::Checked;
    auto pluginInternalName = item->data(Qt::UserRole).toString();
    PluginHost->SetPluginEnableState(pluginInternalName, isEnabled);
    auto &info = PluginHost->GetPluginInfo(pluginInternalName);
    item->setText(info.pluginInterface->Name() + " (" + (info.isLoaded ? tr("Loaded") : tr("Not loaded")) + ")");
    //
    if (!isEnabled)
    {
        QvMessageBoxInfo(this, tr("Disabling a plugin"), tr("This plugin will keep loaded until the next time Qv2ray starts."));
    }
}

void PluginManageWindow::on_pluginSettingsBtn_clicked()
{
    if (const auto &current = pluginListWidget->currentItem(); current != nullptr)
    {
        auto &info = PluginHost->GetPluginInfo(current->data(Qt::UserRole).toString());
        if (!info.isLoaded)
        {
            QvMessageBoxWarn(this, tr("Plugin not loaded"),
                             tr("This plugin has been unloaded or has been disabled, please enable or reload the plugin to continue."));
            return;
        }
        if (auto widget = info.pluginInterface->GetSettingsWidget(); widget != nullptr)
        {
            QDialog d;
            widget->setParent(&d);
            d.exec();
            widget->setParent(nullptr);
        }
    }
}

void PluginManageWindow::on_pluginEditSettingsJsonBtn_clicked()
{
    if (const auto &current = pluginListWidget->currentItem(); current != nullptr)
    {
        const auto &info = PluginHost->GetPluginInfo(current->data(Qt::UserRole).toString());
        if (!info.isLoaded)
        {
            QvMessageBoxWarn(this, tr("Plugin not loaded"),
                             tr("This plugin has been unloaded or has been disabled, please enable or reload the plugin to continue."));
            return;
        }
        JsonEditor w(info.pluginInterface->GetPluginSettngs());
        auto newConf = w.OpenEditor();
        if (w.result() == QDialog::Accepted)
        {
            info.pluginInterface->UpdatePluginSettings(newConf);
        }
    }
}
