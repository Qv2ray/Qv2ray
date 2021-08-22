#include "w_OutboundEditor.hpp"

#include "Qv2rayBase/Qv2rayBaseLibrary.hpp"
#include "ui/widgets/editors/StreamSettingsWidget.hpp"

#include <QIntValidator>

constexpr auto OUTBOUND_TAG_PROXY = "Proxy";

OutboundEditor::OutboundEditor(QWidget *parent) : QDialog(parent), outboundTag(OUTBOUND_TAG_PROXY)
{
    QvMessageBusConnect();
    setupUi(this);
    streamSettingsWidget = new StreamSettingsWidget(this);
    streamSettingsWidget->SetStreamObject({});
    transportFrame->addWidget(streamSettingsWidget);

    for (const auto &[_, plugin] : GUIPluginHost->QueryByGuiComponent(Qv2rayPlugin::GUI_COMPONENT_OUTBOUND_EDITOR))
    {
        const auto editors = plugin->GetOutboundEditors();
        for (const auto &editorInfo : editors)
        {
            outBoundTypeCombo->addItem(editorInfo.first.DisplayName, editorInfo.first.Protocol);
            outboundTypeStackView->addWidget(editorInfo.second);
            pluginWidgets.insert(editorInfo.first.Protocol, editorInfo.second);
        }
    }

    outBoundTypeCombo->model()->sort(0);
}

QvMessageBusSlotImpl(OutboundEditor)
{
    switch (msg)
    {

        case Qv2ray::components::MessageBus::UPDATE_COLORSCHEME: break;
    }
}

OutboundEditor::OutboundEditor(const OutboundObject &out, QWidget *parent) : OutboundEditor(parent)
{
    originalConfig = out;
    reloadGUI();
}

OutboundEditor::~OutboundEditor()
{
}

OutboundObject OutboundEditor::OpenEditor()
{
    int resultCode = this->exec();
    return resultCode == QDialog::Accepted ? resultConfig : originalConfig;
}

QString OutboundEditor::GetFriendlyName()
{
    const auto host = ipLineEdit->text().replace(':', '-').replace('/', '_').replace('\\', '_');
    const auto port = portLineEdit->text().replace(':', '-').replace('/', '_').replace('\\', '_');
    return outboundTag.isEmpty() ? outboundProtocol + "@" + host + ":" + port : outboundTag;
}

OutboundObject OutboundEditor::generateConnectionJson()
{
    IOProtocolSettings settings;
    auto streaming = streamSettingsWidget->GetStreamSettings();
    bool processed = false;
    for (const auto &[protocol, widget] : pluginWidgets.toStdMap())
    {
        if (protocol == outboundProtocol)
        {
            widget->Store();
            settings = widget->settings;
            const auto hasStreamSettings = widget->property("QV2RAY_INTERNAL_HAS_STREAMSETTINGS").toBool();
            if (!hasStreamSettings)
                streaming = {};
            processed = true;
            break;
        }
    }
    if (!processed)
    {
        QvBaselib->Warn(tr("Unknown outbound protocol."), tr("The specified protocol is not supported, this may happen due to a plugin failure."));
    }

    OutboundObject out;
    out.outboundSettings = IOConnectionSettings{ outboundProtocol, serverAddress, serverPort, settings, streaming, muxConfig };
    out.name = outboundTag;
    return out;
}

void OutboundEditor::reloadGUI()
{
    outboundTag = originalConfig.name;
    tagTxt->setText(outboundTag);
    outboundProtocol = originalConfig.outboundSettings.protocol;

    muxConfig = originalConfig.outboundSettings.muxSettings;
    streamSettingsWidget->SetStreamObject(Qv2ray::Models::StreamSettingsObject::fromJson(originalConfig.outboundSettings.streamSettings));

    muxEnabledCB->setChecked(muxConfig.enabled);
    muxConcurrencyTxt->setValue(muxConfig.concurrency);

    serverAddress = originalConfig.outboundSettings.address;
    serverPort = originalConfig.outboundSettings.port.from;

    const auto &settings = originalConfig.outboundSettings.protocolSettings;
    bool processed = false;
    for (auto it = pluginWidgets.constKeyValueBegin(); it != pluginWidgets.constKeyValueEnd(); it++)
    {
        if (it->first == outboundProtocol)
        {
            outBoundTypeCombo->setCurrentIndex(outBoundTypeCombo->findData(it->first));
            it->second->settings = settings;
            it->second->Load();
            ipLineEdit->setText(serverAddress);
            portLineEdit->setText(QString::number(serverPort));
            processed = true;
            break;
        }
    }

    if (!processed)
    {
        qInfo() << "Outbound type:" << outboundProtocol << " is not supported.";
        QvBaselib->Warn(tr("Unknown outbound."), tr("The specified outbound type is invalid, this may be caused by a plugin failure.") + NEWLINE +
                                                     tr("Please use the JsonEditor or reload the plugin."));
        reject();
    }
}

void OutboundEditor::on_buttonBox_accepted()
{
    resultConfig = generateConnectionJson();
}

void OutboundEditor::on_ipLineEdit_textEdited(const QString &arg1)
{
    serverAddress = arg1;
}

void OutboundEditor::on_portLineEdit_textEdited(const QString &arg1)
{
    serverPort = arg1.toInt();
}

void OutboundEditor::on_tagTxt_textEdited(const QString &arg1)
{
    outboundTag = arg1;
}

void OutboundEditor::on_muxEnabledCB_stateChanged(int arg1)
{
    muxConfig.enabled = arg1 == Qt::Checked;
}

void OutboundEditor::on_muxConcurrencyTxt_valueChanged(int arg1)
{
    muxConfig.concurrency = arg1;
}

void OutboundEditor::on_outBoundTypeCombo_currentIndexChanged(int)
{
    outboundProtocol = outBoundTypeCombo->currentData().toString();
    auto newWidget = pluginWidgets[outboundProtocol];
    if (!newWidget)
        return;
    outboundTypeStackView->setCurrentWidget(newWidget);
    const auto hasStreamSettings = newWidget->property("QV2RAY_INTERNAL_HAS_STREAMSETTINGS").toBool();
    streamSettingsWidget->setEnabled(hasStreamSettings);
}
