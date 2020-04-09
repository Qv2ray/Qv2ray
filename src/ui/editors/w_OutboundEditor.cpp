#include "w_OutboundEditor.hpp"

#include "components/plugins/QvPluginHost.hpp"
#include "core/connection/Generation.hpp"
#include "ui/editors/w_JsonEditor.hpp"
#include "ui/editors/w_RoutesEditor.hpp"
#include "ui/w_MainWindow.hpp"

#include <QFile>
#include <QIntValidator>
#include <iostream>

OutboundEditor::OutboundEditor(QWidget *parent) : QDialog(parent), tag(""), Mux(), vmess(), shadowsocks()
{
    QvMessageBusConnect(OutboundEditor);
    setupUi(this);
    //
    streamSettingsWidget = new StreamSettingsWidget(this);
    streamSettingsWidget->SetStreamObject({});
    transportFrame->addWidget(streamSettingsWidget);
    //
    socks.users.push_back(SocksServerObject::UserObject());
    vmess.users.push_back(VMessServerObject::UserObject());
    //
    auto data = PluginHost->GetOutboundEditorWidgets();
    //
    outboundType = "vmess";
    tag = OUTBOUND_TAG_PROXY;
    useForwardProxy = false;
    ReloadGUI();
    Result = GenerateConnectionJson();
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
    Original = outboundEntry;
    tag = outboundEntry["tag"].toString();
    tagTxt->setText(tag);
    outboundType = outboundEntry["protocol"].toString();
    Mux = outboundEntry["mux"].toObject();
    useForwardProxy = outboundEntry[QV2RAY_USE_FPROXY_KEY].toBool(false);
    streamSettingsWidget->SetStreamObject(StructFromJsonString<StreamSettingsObject>(JsonToString(outboundEntry["streamSettings"].toObject())));

    if (outboundType == "vmess")
    {
        vmess =
            StructFromJsonString<VMessServerObject>(JsonToString(outboundEntry["settings"].toObject()["vnext"].toArray().first().toObject()));
        shadowsocks.port = vmess.port;
        shadowsocks.address = vmess.address;
        socks.address = vmess.address;
        socks.port = vmess.port;
    }
    else if (outboundType == "shadowsocks")
    {
        shadowsocks = StructFromJsonString<ShadowSocksServerObject>(
            JsonToString(outboundEntry["settings"].toObject()["servers"].toArray().first().toObject()));
        vmess.address = shadowsocks.address;
        vmess.port = shadowsocks.port;
        socks.address = shadowsocks.address;
        socks.port = shadowsocks.port;
    }
    else if (outboundType == "socks")
    {
        socks =
            StructFromJsonString<SocksServerObject>(JsonToString(outboundEntry["settings"].toObject()["servers"].toArray().first().toObject()));
        vmess.address = socks.address;
        vmess.port = socks.port;
        shadowsocks.address = socks.address;
        shadowsocks.port = socks.port;
    }

    ReloadGUI();
    Result = GenerateConnectionJson();
}

OutboundEditor::~OutboundEditor()
{
}

OUTBOUND OutboundEditor::OpenEditor()
{
    int resultCode = this->exec();
    return resultCode == QDialog::Accepted ? Result : Original;
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
    auto streaming = GetRootObject(streamSettingsWidget->GetStreamSettings());

    if (outboundType == "vmess")
    {
        // VMess is only a ServerObject, and we need an array { "vnext": [] }
        QJsonArray vnext;
        vnext.append(GetRootObject(vmess));
        settings.insert("vnext", vnext);
    }
    else if (outboundType == "shadowsocks")
    {
        streaming = QJsonObject();
        LOG(MODULE_CONNECTION, "Shadowsocks outbound does not need StreamSettings.")
        QJsonArray servers;
        servers.append(GetRootObject(shadowsocks));
        settings["servers"] = servers;
    }
    else if (outboundType == "socks")
    {
        if (!socks.users.isEmpty() && socks.users.first().user.isEmpty() && socks.users.first().pass.isEmpty())
        {
            LOG(MODULE_UI, "Removed empty user form SOCKS settings")
            socks.users.clear();
        }
        streaming = QJsonObject();
        LOG(MODULE_CONNECTION, "Socks outbound does not need StreamSettings.")
        QJsonArray servers;
        servers.append(GetRootObject(socks));
        settings["servers"] = servers;
    }

    auto root = GenerateOutboundEntry(outboundType, settings, streaming, Mux, "0.0.0.0", tag);
    root[QV2RAY_USE_FPROXY_KEY] = useForwardProxy;
    return root;
}

void OutboundEditor::ReloadGUI()
{
    if (outboundType == "vmess")
    {
        outBoundTypeCombo->setCurrentIndex(0);
        ipLineEdit->setText(vmess.address);
        portLineEdit->setText(QSTRN(vmess.port));
        idLineEdit->setText(vmess.users.front().id);
        alterLineEdit->setValue(vmess.users.front().alterId);
        securityCombo->setCurrentText(vmess.users.front().security);
    }
    else if (outboundType == "shadowsocks")
    {
        outBoundTypeCombo->setCurrentIndex(1);
        // ShadowSocks Configs
        ipLineEdit->setText(shadowsocks.address);
        portLineEdit->setText(QSTRN(shadowsocks.port));
        ss_emailTxt->setText(shadowsocks.email);
        ss_levelSpin->setValue(shadowsocks.level);
        ss_otaCheckBox->setChecked(shadowsocks.ota);
        ss_passwordTxt->setText(shadowsocks.password);
        ss_encryptionMethod->setCurrentText(shadowsocks.method);
    }
    else if (outboundType == "socks")
    {
        outBoundTypeCombo->setCurrentIndex(2);
        ipLineEdit->setText(socks.address);
        portLineEdit->setText(QSTRN(socks.port));

        if (socks.users.empty())
            socks.users.push_back(SocksServerObject::UserObject());

        socks_PasswordTxt->setText(socks.users.front().pass);
        socks_UserNameTxt->setText(socks.users.front().user);
    }

    useFPCB->setChecked(useForwardProxy);
    muxEnabledCB->setChecked(Mux["enabled"].toBool());
    muxConcurrencyTxt->setValue(Mux["concurrency"].toInt());
}

void OutboundEditor::on_buttonBox_accepted()
{
    Result = GenerateConnectionJson();
}

void OutboundEditor::on_ipLineEdit_textEdited(const QString &arg1)
{
    vmess.address = arg1;
    shadowsocks.address = arg1;
    socks.address = arg1;
}

void OutboundEditor::on_portLineEdit_textEdited(const QString &arg1)
{
    if (arg1 != "")
    {
        vmess.port = arg1.toInt();
        shadowsocks.port = arg1.toInt();
        socks.port = arg1.toInt();
    }
}

void OutboundEditor::on_idLineEdit_textEdited(const QString &arg1)
{
    if (vmess.users.empty())
        vmess.users.push_back(VMessServerObject::UserObject());

    vmess.users.front().id = arg1;
}

void OutboundEditor::on_securityCombo_currentIndexChanged(const QString &arg1)
{
    if (vmess.users.empty())
        vmess.users.push_back(VMessServerObject::UserObject());

    vmess.users.front().security = arg1;
}

void OutboundEditor::on_tagTxt_textEdited(const QString &arg1)
{
    tag = arg1;
}

void OutboundEditor::on_muxEnabledCB_stateChanged(int arg1)
{
    Mux["enabled"] = arg1 == Qt::Checked;
}

void OutboundEditor::on_muxConcurrencyTxt_valueChanged(int arg1)
{
    Mux["concurrency"] = arg1;
}

void OutboundEditor::on_alterLineEdit_valueChanged(int arg1)
{
    if (vmess.users.empty())
        vmess.users.push_back(VMessServerObject::UserObject());

    vmess.users.front().alterId = arg1;
}

void OutboundEditor::on_useFPCB_stateChanged(int arg1)
{
    useForwardProxy = arg1 == Qt::Checked;
}

void OutboundEditor::on_outBoundTypeCombo_currentIndexChanged(int index)
{
    outboundTypeStackView->setCurrentIndex(index);
    outboundType = outBoundTypeCombo->currentText().toLower();
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

void OutboundEditor::on_ss_levelSpin_valueChanged(int arg1)
{
    shadowsocks.level = arg1;
}

void OutboundEditor::on_ss_otaCheckBox_stateChanged(int arg1)
{
    shadowsocks.ota = arg1 == Qt::Checked;
}

void OutboundEditor::on_socks_UserNameTxt_textEdited(const QString &arg1)
{
    if (socks.users.isEmpty())
        socks.users.push_back(SocksServerObject::UserObject());
    socks.users.front().user = arg1;
}

void OutboundEditor::on_socks_PasswordTxt_textEdited(const QString &arg1)
{
    if (socks.users.isEmpty())
        socks.users.push_back(SocksServerObject::UserObject());
    socks.users.front().pass = arg1;
}
