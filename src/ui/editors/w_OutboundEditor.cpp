#include "w_OutboundEditor.hpp"

#include "core/connection/Generation.hpp"
#include "ui/common/UIBase.hpp"
#include "ui/editors/w_JsonEditor.hpp"
#include "ui/editors/w_RoutesEditor.hpp"

#include <QFile>
#include <QIntValidator>
#include <iostream>

OutboundEditor::OutboundEditor(QWidget *parent) : QDialog(parent), tag(OUTBOUND_TAG_PROXY)
{
    QvMessageBusConnect(OutboundEditor);
    setupUi(this);
    //
    outboundType = "vmess";
    //
    streamSettingsWidget = new StreamSettingsWidget(this);
    streamSettingsWidget->SetStreamObject({});
    transportFrame->addWidget(streamSettingsWidget);
    //
    socks.users.push_back({});
    http.users.push_back({});
    vmess.users.push_back({});
    //
    auto pluginEditorWidgetsInfo = PluginHost->GetOutboundEditorWidgets();
    for (const auto &plugin : pluginEditorWidgetsInfo)
    {
        for (const auto &_d : plugin->OutboundCapabilities())
        {
            outBoundTypeCombo->addItem(_d.displayName, _d.protocol);
            auto index = outboundTypeStackView->addWidget(plugin);
            pluginWidgets.insert(index, { _d, plugin });
        }
    }
    //
    outboundType = "vmess";
    useForwardProxy = false;
}

QvMessageBusSlotImpl(OutboundEditor)
{
    switch (msg)
    {
        case UPDATE_COLORSCHEME:
        {
            break;
        };
            MBShowDefaultImpl;
            MBHideDefaultImpl;
            MBRetranslateDefaultImpl;
    }
}

OutboundEditor::OutboundEditor(const OUTBOUND &outboundEntry, QWidget *parent) : OutboundEditor(parent)
{
    originalConfig = outboundEntry;
    ReloadGUI();
}

OutboundEditor::~OutboundEditor()
{
}

OUTBOUND OutboundEditor::OpenEditor()
{
    int resultCode = this->exec();
    return resultCode == QDialog::Accepted ? resultConfig : originalConfig;
}

QString OutboundEditor::GetFriendlyName()
{
    auto host = ipLineEdit->text().replace(":", "-").replace("/", "_").replace("\\", "_");
    auto port = portLineEdit->text().replace(":", "-").replace("/", "_").replace("\\", "_");
    auto type = outboundType;
    QString name = tag.isEmpty() ? host + "-[" + port + "]-" + type : tag;
    return name;
}

OUTBOUND OutboundEditor::GenerateConnectionJson()
{
    OUTBOUNDSETTING settings;
    auto streaming = streamSettingsWidget->GetStreamSettings().toJson();

    if (outboundType == "vmess")
    {
        // VMess is only a ServerObject, and we need an array { "vnext": [] }
        QJsonArray vnext;
        vmess.address = address;
        vmess.port = port;
        vnext.append(vmess.toJson());
        settings.insert("vnext", vnext);
    }
    else if (outboundType == "shadowsocks")
    {
        // streaming = QJsonObject();
        // LOG(MODULE_CONNECTION, "Shadowsocks outbound does not need StreamSettings.")
        QJsonArray servers;
        shadowsocks.address = address;
        shadowsocks.port = port;
        servers.append(shadowsocks.toJson());
        settings["servers"] = servers;
    }
    else if (outboundType == "socks")
    {
        if (!socks.users.isEmpty() && socks.users.first().user.isEmpty() && socks.users.first().pass.isEmpty())
        {
            LOG(MODULE_UI, "Removed empty user form SOCKS settings")
            socks.users.clear();
        }
        socks.address = address;
        socks.port = port;
        // streaming = QJsonObject();
        // LOG(MODULE_CONNECTION, "Socks outbound does not need StreamSettings.")
        QJsonArray servers;
        servers.append(socks.toJson());
        settings["servers"] = servers;
    }
    else if (outboundType == "http")
    {
        if (!http.users.isEmpty() && http.users.first().user.isEmpty() && http.users.first().pass.isEmpty())
        {
            LOG(MODULE_UI, "Removed empty user form HTTP settings")
            http.users.clear();
        }
        http.address = address;
        http.port = port;
        // streaming = QJsonObject();
        // LOG(MODULE_CONNECTION, "Http outbound does not need StreamSettings.")
        QJsonArray servers;
        servers.append(http.toJson());
        settings["servers"] = servers;
    }
    else
    {
        streaming = QJsonObject();
        bool processed = false;
        for (const auto &plugin : pluginWidgets)
        {
            if (plugin.first.protocol == outboundType)
            {
                plugin.second->SetHostInfo(address, port);
                settings = OUTBOUNDSETTING(plugin.second->GetContent());
                processed = true;
                break;
            }
        }
        if (!processed)
        {
            QvMessageBoxWarn(this, tr("Unknown outbound type."),
                             tr("The specified outbound type is not supported, this may happen due to a plugin failure."));
        }
    }

    auto root = GenerateOutboundEntry(outboundType, settings, streaming, muxConfig, "0.0.0.0", tag);
    root[QV2RAY_USE_FPROXY_KEY] = useForwardProxy;
    return root;
}

void OutboundEditor::ReloadGUI()
{
    tag = originalConfig["tag"].toString();
    tagTxt->setText(tag);
    outboundType = originalConfig["protocol"].toString("vmess");
    muxConfig = originalConfig["mux"].toObject();
    useForwardProxy = originalConfig[QV2RAY_USE_FPROXY_KEY].toBool(false);
    streamSettingsWidget->SetStreamObject(StreamSettingsObject::fromJson(originalConfig["streamSettings"].toObject()));
    //
    useFPCB->setChecked(useForwardProxy);
    muxEnabledCB->setChecked(muxConfig["enabled"].toBool());
    muxConcurrencyTxt->setValue(muxConfig["concurrency"].toInt());
    //
    const auto &settings = originalConfig["settings"].toObject();
    //
    if (outboundType == "vmess")
    {
        outBoundTypeCombo->setCurrentIndex(0);
        vmess = VMessServerObject::fromJson(settings["vnext"].toArray().first().toObject());
        if (vmess.users.empty())
        {
            vmess.users.push_back({});
        }
        address = vmess.address;
        port = vmess.port;
        idLineEdit->setText(vmess.users.front().id);
        alterLineEdit->setValue(vmess.users.front().alterId);
        securityCombo->setCurrentText(vmess.users.front().security);
        testsEnabledCombo->setCurrentText(vmess.users.front().testsEnabled);
    }
    else if (outboundType == "shadowsocks")
    {
        outBoundTypeCombo->setCurrentIndex(1);
        shadowsocks = ShadowSocksServerObject::fromJson(settings["servers"].toArray().first().toObject());
        address = shadowsocks.address;
        port = shadowsocks.port;
        // ShadowSocks Configs
        ss_emailTxt->setText(shadowsocks.email);
        ss_otaCheckBox->setChecked(shadowsocks.ota);
        ss_passwordTxt->setText(shadowsocks.password);
        ss_encryptionMethod->setCurrentText(shadowsocks.method);
    }
    else if (outboundType == "socks")
    {
        outBoundTypeCombo->setCurrentIndex(2);
        socks = SocksServerObject::fromJson(settings["servers"].toArray().first().toObject());
        address = socks.address;
        port = socks.port;
        if (socks.users.empty())
        {
            socks.users.push_back({});
        }
        socks_PasswordTxt->setText(socks.users.front().pass);
        socks_UserNameTxt->setText(socks.users.front().user);
    }
    else if (outboundType == "http")
    {
        outBoundTypeCombo->setCurrentIndex(3);
        http = HttpServerObject::fromJson(settings["servers"].toArray().first().toObject());
        address = http.address;
        port = http.port;
        if (http.users.empty())
        {
            http.users.push_back({});
        }
        http_PasswordTxt->setText(http.users.front().pass);
        http_UserNameTxt->setText(http.users.front().user);
    }
    else
    {
        bool processed = false;
        for (const auto &index : pluginWidgets.keys())
        {
            const auto &plugin = pluginWidgets.value(index);
            if (plugin.first.protocol == outboundType)
            {
                useFPCB->setEnabled(false);
                useFPCB->setToolTip(tr("Forward proxy has been disabled when using plugin outbound"));
                plugin.second->SetContent(settings);
                outBoundTypeCombo->setCurrentIndex(index);
                auto [_address, _port] = plugin.second->GetHostInfo();
                address = _address;
                port = _port;
                processed = true;
                break;
            }
        }
        if (!processed)
        {
            LOG(MODULE_UI, "Outbound type: " + outboundType + " is not supported.")
            QvMessageBoxWarn(this, tr("Unknown outbound."),
                             tr("The specified outbound type is invalid, this may be caused by a plugin failure.") + NEWLINE +
                                 tr("Please use the JsonEditor or reload the plugin."));
            reject();
        }
    }
    //
    ipLineEdit->setText(address);
    portLineEdit->setText(QSTRN(port));
}

void OutboundEditor::on_buttonBox_accepted()
{
    resultConfig = GenerateConnectionJson();
}

void OutboundEditor::on_ipLineEdit_textEdited(const QString &arg1)
{
    address = arg1;
}

void OutboundEditor::on_portLineEdit_textEdited(const QString &arg1)
{
    port = arg1.toInt();
}

void OutboundEditor::on_idLineEdit_textEdited(const QString &arg1)
{
    const static QRegularExpression regExpUUID("^[0-9a-f]{8}(-[0-9a-f]{4}){3}-[0-9a-f]{12}$",
                                               QRegularExpression::PatternOption::CaseInsensitiveOption);

    if (!regExpUUID.match(arg1).hasMatch())
    {
        RED(idLineEdit);
    }
    else
    {
        BLACK(idLineEdit);
    }

    if (vmess.users.empty())
        vmess.users.push_back({});

    vmess.users.front().id = arg1;
}

void OutboundEditor::on_securityCombo_currentIndexChanged(const QString &arg1)
{
    if (vmess.users.empty())
        vmess.users.push_back({});

    vmess.users.front().security = arg1;
}

void OutboundEditor::on_tagTxt_textEdited(const QString &arg1)
{
    tag = arg1;
}

void OutboundEditor::on_muxEnabledCB_stateChanged(int arg1)
{
    muxConfig["enabled"] = arg1 == Qt::Checked;
}

void OutboundEditor::on_muxConcurrencyTxt_valueChanged(int arg1)
{
    muxConfig["concurrency"] = arg1;
}

void OutboundEditor::on_alterLineEdit_valueChanged(int arg1)
{
    if (vmess.users.empty())
        vmess.users.push_back({});

    vmess.users.front().alterId = arg1;
}

void OutboundEditor::on_useFPCB_stateChanged(int arg1)
{
    useForwardProxy = arg1 == Qt::Checked;
    streamSettingsWidget->setEnabled(!useForwardProxy);
}

void OutboundEditor::on_outBoundTypeCombo_currentIndexChanged(int index)
{
    // 0, 1, 2, 3 as built-in vmess, ss, socks, http
    outboundTypeStackView->setCurrentIndex(index);
    if (index < 4)
    {
        outboundType = outBoundTypeCombo->currentText().toLower();
        useFPCB->setEnabled(true);
        useFPCB->setToolTip(tr(""));
        streamSettingsWidget->setEnabled(!useFPCB->isChecked());
    }
    else
    {
        outboundType = pluginWidgets.value(index).first.protocol;
        useFPCB->setChecked(false);
        useFPCB->setEnabled(false);
        useFPCB->setToolTip(tr("Forward proxy has been disabled when using plugin outbound"));
        streamSettingsWidget->setEnabled(false);
    }
    
}

void OutboundEditor::on_ss_emailTxt_textEdited(const QString &arg1)
{
    shadowsocks.email = arg1;
}

void OutboundEditor::on_ss_passwordTxt_textEdited(const QString &arg1)
{
    shadowsocks.password = arg1;
}

void OutboundEditor::on_ss_encryptionMethod_currentIndexChanged(const QString &arg1)
{
    shadowsocks.method = arg1;
}

void OutboundEditor::on_ss_otaCheckBox_stateChanged(int arg1)
{
    shadowsocks.ota = arg1 == Qt::Checked;
}

void OutboundEditor::on_socks_UserNameTxt_textEdited(const QString &arg1)
{
    if (socks.users.isEmpty())
        socks.users.push_back({});
    socks.users.front().user = arg1;
}

void OutboundEditor::on_socks_PasswordTxt_textEdited(const QString &arg1)
{
    if (socks.users.isEmpty())
        socks.users.push_back({});
    socks.users.front().pass = arg1;
}

void OutboundEditor::on_http_UserNameTxt_textEdited(const QString &arg1)
{
    if (http.users.isEmpty())
        http.users.push_back({});
    http.users.front().user = arg1;
}

void OutboundEditor::on_http_PasswordTxt_textEdited(const QString &arg1)
{
    if (http.users.isEmpty())
        http.users.push_back({});
    http.users.front().pass = arg1;
}

void OutboundEditor::on_testsEnabledCombo_currentIndexChanged(const QString &arg1)
{
    vmess.users.front().testsEnabled = arg1;
}
