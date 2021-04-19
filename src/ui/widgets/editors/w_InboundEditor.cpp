#include "w_InboundEditor.hpp"

#include "core/CoreUtils.hpp"
#include "core/connection/ConnectionIO.hpp"
#include "plugin-interface/QvGUIPluginInterface.hpp"
#include "ui/widgets/common/WidgetUIBase.hpp"
#include "ui/widgets/widgets/StreamSettingsWidget.hpp"
#include "utils/QvHelpers.hpp"

#include <QGridLayout>

#define QV_MODULE_NAME "InboundEditor"
#define CHECKLOADING                                                                                                                                 \
    if (isLoading)                                                                                                                                   \
        return;

InboundEditor::InboundEditor(INBOUND source, QWidget *parent) : QDialog(parent), original(source)
{
    QvMessageBusConnect(InboundEditor);
    setupUi(this);
    streamSettingsWidget = new StreamSettingsWidget(this);
    streamSettingsWidget->SetStreamObject({});
    if (!transportFrame->layout())
    {
        auto l = new QGridLayout();
        l->setHorizontalSpacing(0);
        l->setVerticalSpacing(0);
        transportFrame->setLayout(l);
    }
    transportFrame->layout()->addWidget(streamSettingsWidget);
    this->current = source;
    inboundProtocol = current["protocol"].toString("http");
    allocateSettings = current["allocate"].toObject();
    sniffingSettings = current["sniffing"].toObject();

    isLoading = true;
    for (const auto &name : PluginHost->UsablePlugins())
    {
        const auto &plugin = PluginHost->GetPlugin(name);
        if (!plugin->hasComponent(COMPONENT_GUI))
            continue;

        const auto guiInterface = plugin->pluginInterface->GetGUIInterface();

        if (!guiInterface)
        {
            LOG("Found a plugin with COMPONENT_GUI but returns an invalid GUI interface: ", plugin->metadata.Name);
            continue;
        }

        if (!guiInterface->GetComponents().contains(GUI_COMPONENT_INBOUND_EDITOR))
            continue;

        const auto editors = guiInterface->GetInboundEditors();
        for (const auto &editorInfo : editors)
        {
            inboundProtocolCombo->addItem(editorInfo.first.displayName, editorInfo.first.protocol);
            stackedWidget->addWidget(editorInfo.second);
            pluginWidgets.insert(editorInfo.first.protocol, editorInfo.second);
        }
    }
    inboundProtocolCombo->model()->sort(0);
    isLoading = false;
    loadUI();
}

QvMessageBusSlotImpl(InboundEditor)
{
    switch (msg)
    {
        MBShowDefaultImpl;
        MBHideDefaultImpl;
        MBRetranslateDefaultImpl;
        case UPDATE_COLORSCHEME: break;
    }
}

INBOUND InboundEditor::OpenEditor()
{
    int resultCode = this->exec();
    return resultCode == QDialog::Accepted ? getResult() : original;
}

INBOUND InboundEditor::getResult()
{
    INBOUND newRoot = current;
    for (const auto &[protocol, widget] : pluginWidgets.toStdMap())
    {
        if (protocol == inboundProtocol)
        {
            newRoot["settings"] = INBOUNDSETTING(widget->GetContent());
            break;
        }
    }
    if (streamSettingsWidget->isEnabled())
    {
        newRoot["streamSettings"] = streamSettingsWidget->GetStreamSettings().toJson();
    }
    newRoot["protocol"] = inboundProtocol;
    newRoot["sniffing"] = sniffingSettings;
    newRoot["allocate"] = allocateSettings;
    return newRoot;
}

void InboundEditor::loadUI()
{
    isLoading = true;
    streamSettingsWidget->SetStreamObject(StreamSettingsObject::fromJson(original["streamSettings"].toObject()));
    {
        inboundTagTxt->setText(current["tag"].toString());
        inboundHostTxt->setText(current["listen"].toString());
        inboundPortTxt->setText(current["port"].toVariant().toString());
    }
    {
        const auto allocationStrategy = allocateSettings["strategy"].toString();
        allocateSettings["strategy"] = allocationStrategy.isEmpty() ? "always" : allocationStrategy;
        strategyCombo->setCurrentText(allocationStrategy);
        refreshNumberBox->setValue(allocateSettings["refresh"].toInt());
        concurrencyNumberBox->setValue(allocateSettings["concurrency"].toInt());
    }
    {
        sniffingGroupBox->setChecked(sniffingSettings["enabled"].toBool());
        sniffMetaDataOnlyCB->setChecked(sniffingSettings["metadataOnly"].toBool());
        const auto data = sniffingSettings["destOverride"].toArray();
        sniffHTTPCB->setChecked(data.contains("http"));
        sniffTLSCB->setChecked(data.contains("tls"));
        sniffFakeDNSCB->setChecked(data.contains("fakedns"));
        sniffFakeDNSOtherCB->setChecked(data.contains("fakedns+others"));
    }
    bool processed = false;
    const auto settings = current["settings"].toObject();
    for (const auto &[protocol, widget] : pluginWidgets.toStdMap())
    {
        if (protocol == inboundProtocol)
        {
            widget->SetContent(settings);
            inboundProtocolCombo->setCurrentIndex(inboundProtocolCombo->findData(protocol));
            processed = true;
            break;
        }
    }

    if (!processed)
    {
        LOG("Inbound protocol: " + inboundProtocol + " is not supported.");
        QvMessageBoxWarn(this, tr("Unknown inbound."),
                         tr("The specified inbound type is invalid, this may be caused by a plugin failure.") + NEWLINE +
                             tr("Please use the JsonEditor or reload the plugin."));
        reject();
    }
    isLoading = false;
    on_stackedWidget_currentChanged(0);
}

InboundEditor::~InboundEditor()
{
}

void InboundEditor::on_inboundProtocolCombo_currentIndexChanged(int)
{
    CHECKLOADING
    on_stackedWidget_currentChanged(0);
}

void InboundEditor::on_inboundTagTxt_textEdited(const QString &arg1)
{
    CHECKLOADING
    current["tag"] = arg1;
}
void InboundEditor::on_strategyCombo_currentIndexChanged(int arg1)
{
    CHECKLOADING
    allocateSettings["strategy"] = strategyCombo->itemText(arg1).toLower();
}

void InboundEditor::on_refreshNumberBox_valueChanged(int arg1)
{
    CHECKLOADING
    allocateSettings["refresh"] = arg1;
}

void InboundEditor::on_concurrencyNumberBox_valueChanged(int arg1)
{
    CHECKLOADING
    allocateSettings["concurrency"] = arg1;
}

void InboundEditor::on_inboundHostTxt_textEdited(const QString &arg1)
{
    CHECKLOADING
    current["listen"] = arg1;
}

void InboundEditor::on_inboundPortTxt_textEdited(const QString &arg1)
{
    CHECKLOADING
    current["port"] = arg1;
}

void InboundEditor::on_sniffingGroupBox_clicked(bool checked)
{
    CHECKLOADING
    sniffingSettings["enabled"] = checked;
}

void InboundEditor::on_sniffMetaDataOnlyCB_clicked(bool checked)
{
    CHECKLOADING
    sniffingSettings["metadataOnly"] = checked;
}

#define SET_SNIFF_DEST_OVERRIDE                                                                                                                      \
    do                                                                                                                                               \
    {                                                                                                                                                \
        const auto hasHTTP = sniffHTTPCB->isChecked();                                                                                               \
        const auto hasTLS = sniffTLSCB->isChecked();                                                                                                 \
        const auto hasFakeDNS = sniffFakeDNSCB->isChecked();                                                                                         \
        const auto hasFakeDNSOthers = sniffFakeDNSOtherCB->isChecked();                                                                              \
        QStringList list;                                                                                                                            \
        if (hasHTTP)                                                                                                                                 \
            list << "http";                                                                                                                          \
        if (hasTLS)                                                                                                                                  \
            list << "tls";                                                                                                                           \
        if (hasFakeDNS)                                                                                                                              \
            list << "fakedns";                                                                                                                       \
        if (hasFakeDNSOthers)                                                                                                                        \
            list << "fakedns+others";                                                                                                                \
        sniffingSettings["destOverride"] = QJsonArray::fromStringList(list);                                                                         \
    } while (0)

void InboundEditor::on_sniffHTTPCB_stateChanged(int)
{
    CHECKLOADING
    SET_SNIFF_DEST_OVERRIDE;
}

void InboundEditor::on_sniffTLSCB_stateChanged(int)
{
    CHECKLOADING
    SET_SNIFF_DEST_OVERRIDE;
}
void InboundEditor::on_sniffFakeDNSOtherCB_stateChanged(int)
{
    CHECKLOADING
    SET_SNIFF_DEST_OVERRIDE;
}

void InboundEditor::on_sniffFakeDNSCB_stateChanged(int)
{
    CHECKLOADING
    SET_SNIFF_DEST_OVERRIDE;
}

void InboundEditor::on_stackedWidget_currentChanged(int)
{
    CHECKLOADING
    inboundProtocol = inboundProtocolCombo->currentData().toString();
    auto widget = pluginWidgets[inboundProtocol];
    if (!widget)
        return;
    stackedWidget->setCurrentWidget(widget);
    const auto hasStreamSettings = GetProperty(widget, "QV2RAY_INTERNAL_HAS_STREAMSETTINGS");
    streamSettingsWidget->setEnabled(hasStreamSettings);
}
