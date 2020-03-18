#include "w_OutboundEditor.hpp"

#include "core/connection/Generation.hpp"
#include "ui/editors/w_JsonEditor.hpp"
#include "ui/editors/w_RoutesEditor.hpp"
#include "ui/w_MainWindow.hpp"

#include <QFile>
#include <QIntValidator>
#include <iostream>

OutboundEditor::OutboundEditor(QWidget *parent) : QDialog(parent), Tag(""), Mux(), vmess(), shadowsocks(), shadowsocksR()
{
    QvMessageBusConnect(OutboundEditor);
    setupUi(this);
    //
    streamWidget = new StreamSettingsWidget(this);
    transportFrame->addWidget(streamWidget);
    //
    socks.users.push_back(SocksServerObject::UserObject());
    vmess.users.push_back(VMessServerObject::UserObject());
    //
    auto stream = StreamSettingsObject();
    streamWidget->SetStreamObject(stream);
    //
    outboundType = "vmess";
    Tag = OUTBOUND_TAG_PROXY;
    useFProxy = false;
    ReloadGUI();
    Result = GenerateConnectionJson();
}

QvMessageBusSlotImpl(OutboundEditor)
{
    switch (msg)
    {
        MBShowDefaultImpl MBHideDefaultImpl MBRetranslateDefaultImpl
    }
}

OutboundEditor::OutboundEditor(const OUTBOUND &outboundEntry, QWidget *parent) : OutboundEditor(parent)
{
    original = outboundEntry;
    Tag = outboundEntry["tag"].toString();
    tagTxt->setText(Tag);
    outboundType = outboundEntry["protocol"].toString();
    Mux = outboundEntry["mux"].toObject();
    useFProxy = outboundEntry[QV2RAY_USE_FPROXY_KEY].toBool(false);
    //
    streamWidget->SetStreamObject(StructFromJsonString<StreamSettingsObject>(JsonToString(outboundEntry["streamSettings"].toObject())));

    if (outboundType == "vmess")
    {
        vmess =
            StructFromJsonString<VMessServerObject>(JsonToString(outboundEntry["settings"].toObject()["vnext"].toArray().first().toObject()));
    }
    else if (outboundType == "shadowsocks")
    {
        shadowsocks = StructFromJsonString<ShadowSocksServerObject>(
            JsonToString(outboundEntry["settings"].toObject()["servers"].toArray().first().toObject()));
    }
    else if (outboundType == "socks")
    {
        socks =
            StructFromJsonString<SocksServerObject>(JsonToString(outboundEntry["settings"].toObject()["servers"].toArray().first().toObject()));
    }
    else if (outboundType == "shadowsocksr")
    {
        shadowsocksR = StructFromJsonString<ShadowSocksRServerObject>(
            JsonToString(outboundEntry["settings"].toObject()["servers"].toArray().first().toObject()));
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
    return resultCode == QDialog::Accepted ? Result : original;
}

QString OutboundEditor::GetFriendlyName()
{
    auto host = ipLineEdit->text().replace(":", "-").replace("/", "_").replace("\\", "_");
    auto port = portLineEdit->text().replace(":", "-").replace("/", "_").replace("\\", "_");
    auto type = outboundType;
    QString name = Tag.isEmpty() ? host + "-[" + port + "]-" + type : Tag;
    return name;
}

OUTBOUND OutboundEditor::GenerateConnectionJson()
{
    OUTBOUNDSETTING settings;
    auto streaming = streamSettingsGroupBox->isChecked() ? GetRootObject(streamWidget->GetStreamSettings()) : QJsonObject();
    if (outboundType == "vmess")
    {
        // VMess is only a ServerObject, and we need an array { "vnext": [] }
        QJsonArray vnext;
        vmess.address = address;
        vmess.port = port.toInt();
        vnext.append(GetRootObject(vmess));
        settings["vnext"] = vnext;
    }
    else if (outboundType == "shadowsocks")
    {
        QJsonArray servers;
        shadowsocks.address = address;
        shadowsocks.port = port.toInt();
        servers.append(GetRootObject(shadowsocks));
        settings["servers"] = servers;
    }
    else if (outboundType == "socks")
    {
        QJsonArray servers;
        socks.address = address;
        socks.port = port.toInt();
        servers.append(GetRootObject(socks));
        settings["servers"] = servers;
    }
    else if (outboundType == "shadowsocksr")
    {
        QJsonArray servers;
        shadowsocksR.address = address;
        shadowsocksR.port = port.toInt();
        servers.append(GetRootObject(shadowsocksR));
        settings["servers"] = servers;
    }

    auto root = GenerateOutboundEntry(outboundType, settings, streaming, Mux, "0.0.0.0", Tag);
    root[QV2RAY_USE_FPROXY_KEY] = useFProxy;
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
    else if (outboundType == "shadowsocksr")
    {
        outBoundTypeCombo->setCurrentIndex(3);
        ipLineEdit->setText(shadowsocksR.address);
        ssrPasswordTxt->setText(shadowsocksR.password);
        portLineEdit->setText(QSTRN(shadowsocksR.port));
        ssrObfsCombo->setCurrentText(shadowsocksR.obfs);
        ssrProtocolCombo->setCurrentText(shadowsocksR.protocol);
        ssrProtocolParamsTxt->setText(shadowsocksR.protocol_param);
        ssrObfsParamsTxt->setText(shadowsocksR.obfs_param);
    }

    useFPCB->setChecked(useFProxy);
    muxEnabledCB->setChecked(Mux["enabled"].toBool());
    muxConcurrencyTxt->setValue(Mux["concurrency"].toInt());
}

void OutboundEditor::on_buttonBox_accepted()
{
    Result = GenerateConnectionJson();
}

void OutboundEditor::on_ipLineEdit_textEdited(const QString &arg1)
{
    address = arg1;
}

void OutboundEditor::on_portLineEdit_textEdited(const QString &arg1)
{
    port = arg1;
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
    Tag = arg1;
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
    useFProxy = arg1 == Qt::Checked;
}

void OutboundEditor::on_outBoundTypeCombo_currentIndexChanged(int index)
{
    outboundTypeStackView->setCurrentIndex(index);
    outboundType = outBoundTypeCombo->currentText().toLower();
    streamSettingsGroupBox->setChecked(index == 0);
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
    socks.users.front().user = arg1;
}

void OutboundEditor::on_socks_PasswordTxt_textEdited(const QString &arg1)
{
    socks.users.front().pass = arg1;
}

void OutboundEditor::on_ssrPasswordTxt_textEdited(const QString &arg1)
{
    shadowsocksR.password = arg1;
}

void OutboundEditor::on_ssrMethodCombo_currentTextChanged(const QString &arg1)
{
    shadowsocksR.method = arg1;
}

void OutboundEditor::on_ssrProtocolCombo_currentTextChanged(const QString &arg1)
{
    shadowsocksR.protocol = arg1;
}

void OutboundEditor::on_ssrObfsCombo_currentTextChanged(const QString &arg1)
{
    shadowsocksR.obfs = arg1;
}

void OutboundEditor::on_ssrProtocolParamsTxt_textEdited(const QString &arg1)
{
    shadowsocksR.protocol_param = arg1;
}

void OutboundEditor::on_ssrObfsParamsTxt_textEdited(const QString &arg1)
{
    shadowsocksR.obfs_param = arg1;
}
