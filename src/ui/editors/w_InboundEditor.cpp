#include "w_InboundEditor.hpp"

#include "common/QvHelpers.hpp"
#include "core/CoreUtils.hpp"
#include "core/connection/ConnectionIO.hpp"
#include "ui/common/UIBase.hpp"

static bool isLoading = false;
#define CHECKLOADING                                                                                                                            \
    if (isLoading)                                                                                                                              \
        return;

InboundEditor::InboundEditor(INBOUND root, QWidget *parent) : QDialog(parent), original(root)
{
    QvMessageBusConnect(InboundEditor);
    setupUi(this);
    this->root = root;
    auto inboundType = root["protocol"].toString();
    allocate = root["allocate"].toObject();
    sniffing = root["sniffing"].toObject();

    if (inboundType == "http")
    {
        httpSettings = root["settings"].toObject();
    }
    else if (inboundType == "socks")
    {
        socksSettings = root["settings"].toObject();
    }
    else if (inboundType == "dokodemo-door")
    {
        dokoSettings = root["settings"].toObject();
        dokotproxy = QJsonIO ::GetValue(root, "streamSettings", "sockopt", "tproxy").toString();
        if (dokotproxy.isEmpty())
            dokotproxy = "off";
    }
    else if (inboundType == "mtproto")
    {
        mtSettings = root["settings"].toObject();
    }
    else
    {
        if (!root["protocol"].toString().isEmpty())
        {
            LOG(MODULE_UI, "Unsupported inbound type: " + inboundType)
            QvMessageBoxWarn(this, tr("Inbound type not supported"),
                             tr("The inbound type is not supported by Qv2ray (yet). Please use JsonEditor to change the settings") + "\r\n" +
                                 tr("Inbound: ") + inboundType);
        }
        else
        {
            LOG(MODULE_UI, "Creating new inbound config")
            root["protocol"] = inboundType = "http";
        }
    }

    LoadUIData();
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
    return resultCode == QDialog::Accepted ? GenerateNewRoot() : original;
}

INBOUND InboundEditor::GenerateNewRoot()
{
    INBOUND _newRoot = root;
    auto inboundType = root["protocol"].toString();

    if (inboundType.isNull() || inboundType.isEmpty())
    {
        inboundType = "http";
    }

    if (inboundType == "http")
    {
        // Remove useless, misleading 'accounts' array.
        if (httpAccountListBox->count() == 0)
        {
            httpSettings.remove("accounts");
        }

        _newRoot["settings"] = httpSettings;
    }
    else if (inboundType == "socks")
    {
        // See above
        if (socksAccountListBox->count() == 0)
        {
            socksSettings.remove("accounts");
        }

        _newRoot["settings"] = socksSettings;
    }
    else if (inboundType == "dokodemo-door")
    {
        _newRoot["settings"] = dokoSettings;
        QJsonIO::SetValue(_newRoot, dokotproxy, "streamSettings", "sockopt", "tproxy");
    }
    else if (inboundType == "mtproto")
    {
        _newRoot["settings"] = mtSettings;
    }

    _newRoot["protocol"] = inboundType;
    _newRoot["sniffing"] = sniffing;
    _newRoot["allocate"] = allocate;
    return _newRoot;
}

void InboundEditor::LoadUIData()
{
    isLoading = true;
    auto x = allocate["strategy"].toString();
    allocate["strategy"] = x.isEmpty() ? "always" : x;
    strategyCombo->setCurrentText(x);
    //
    refreshNumberBox->setValue(allocate["refresh"].toInt());
    concurrencyNumberBox->setValue(allocate["concurrency"].toInt());
    enableSniffingCB->setChecked(sniffing["enabled"].toBool());
    //
    destOverrideList->setEnabled(sniffing["enabled"].toBool());

    for (auto item : sniffing["destOverride"].toArray())
    {
        if (item.toString().toLower() == "http")
            destOverrideList->item(0)->setCheckState(Qt::Checked);

        if (item.toString().toLower() == "tls")
            destOverrideList->item(1)->setCheckState(Qt::Checked);
    }

    inboundTagTxt->setText(root["tag"].toString());
    inboundHostTxt->setText(root["listen"].toString());
    inboundPortTxt->setText(root["port"].toVariant().toString());
    inboundProtocolCombo->setCurrentText(root["protocol"].toString());
    // HTTP
    httpTimeoutSpinBox->setValue(httpSettings["timeout"].toInt());
    httpTransparentCB->setChecked(httpSettings["allowTransparent"].toBool());
    httpUserLevelSB->setValue(httpSettings["userLevel"].toInt());
    httpAccountListBox->clear();

    for (auto user : httpSettings["accounts"].toArray())
    {
        httpAccountListBox->addItem(user.toObject()["user"].toString() + ":" + user.toObject()["pass"].toString());
    }

    // SOCKS
    socksAuthCombo->setCurrentText(socksSettings["auth"].toString());
    socksUDPCB->setChecked(socksSettings["udp"].toBool());
    socksUDPIPAddrTxt->setText(socksSettings["ip"].toString());
    socksUserLevelSB->setValue(socksSettings["userLevel"].toInt());

    for (auto user : socksSettings["accounts"].toArray())
    {
        socksAccountListBox->addItem(user.toObject()["user"].toString() + ":" + user.toObject()["pass"].toString());
    }

    // Dokodemo-Door
    dokoFollowRedirectCB->setChecked(dokoSettings["followRedirect"].toBool());
    dokoIPAddrTxt->setText(dokoSettings["address"].toString());
    dokoPortSB->setValue(dokoSettings["port"].toInt());
    dokoTimeoutSB->setValue(dokoSettings["timeout"].toInt());
    dokoUserLevelSB->setValue(dokoSettings["userLevel"].toInt());
    dokoTCPCB->setChecked(dokoSettings["network"].toString().contains("tcp"));
    dokoUDPCB->setChecked(dokoSettings["network"].toString().contains("udp"));
    dokotproxyCombo->setCurrentText(dokotproxy);
    // MTProto
    mtEMailTxt->setText(mtSettings["users"].toArray().first().toObject()["email"].toString());
    mtUserLevelSB->setValue(mtSettings["users"].toArray().first().toObject()["level"].toInt());
    mtSecretTxt->setText(mtSettings["users"].toArray().first().toObject()["secret"].toString());
    isLoading = false;
}

InboundEditor::~InboundEditor()
{
}

void InboundEditor::on_inboundProtocolCombo_currentIndexChanged(const QString &arg1)
{
    CHECKLOADING
    root["protocol"] = arg1.toLower();
}

void InboundEditor::on_inboundProtocolCombo_currentIndexChanged(int index)
{
    stackedWidget->setCurrentIndex(index);
}

void InboundEditor::on_inboundTagTxt_textEdited(const QString &arg1)
{
    CHECKLOADING
    root["tag"] = arg1;
}

void InboundEditor::on_httpTimeoutSpinBox_valueChanged(int arg1)
{
    CHECKLOADING
    httpSettings["timtout"] = arg1;
}

void InboundEditor::on_httpTransparentCB_stateChanged(int arg1)
{
    CHECKLOADING
    httpSettings["allowTransparent"] = arg1 == Qt::Checked;
}

void InboundEditor::on_httpUserLevelSB_valueChanged(int arg1)
{
    CHECKLOADING
    httpSettings["userLevel"] = arg1;
}

void InboundEditor::on_httpRemoveUserBtn_clicked()
{
    CHECKLOADING

    if (httpAccountListBox->currentRow() != -1)
    {
        auto item = httpAccountListBox->currentItem();
        auto list = httpSettings["accounts"].toArray();

        for (int i = 0; i < list.count(); i++)
        {
            auto user = list[i].toObject();
            auto entry = user["user"].toString() + ":" + user["pass"].toString();

            if (entry == item->text().trimmed())
            {
                list.removeAt(i);
                httpSettings["accounts"] = list;
                LOG(MODULE_UI, "Removed http inbound user " + entry)
                httpAccountListBox->takeItem(httpAccountListBox->currentRow());
            }
        }

        // QvMessageBox(this, tr("Removing a user"), tr("No user has been
        // removed. Why?"));
    }
    else
    {
        QvMessageBoxWarn(this, tr("Removing a user"), tr("You haven't selected a user yet."));
    }
}

void InboundEditor::on_httpAddUserBtn_clicked()
{
    CHECKLOADING
    auto user = httpAddUserTxt->text();
    auto pass = httpAddPasswordTxt->text();
    //
    auto list = httpSettings["accounts"].toArray();

    for (int i = 0; i < list.count(); i++)
    {
        auto _user = list[i].toObject();

        if (_user["user"].toString() == user)
        {
            QvMessageBoxWarn(this, tr("Add a user"), tr("This user exists already."));
            return;
        }
    }

    httpAddUserTxt->clear();
    httpAddPasswordTxt->clear();
    QJsonObject entry;
    entry["user"] = user;
    entry["pass"] = pass;
    list.append(entry);
    httpAccountListBox->addItem(user + ":" + pass);
    httpSettings["accounts"] = list;
}

void InboundEditor::on_socksRemoveUserBtn_clicked()
{
    CHECKLOADING

    if (socksAccountListBox->currentRow() != -1)
    {
        auto item = socksAccountListBox->currentItem();
        auto list = socksSettings["accounts"].toArray();

        for (int i = 0; i < list.count(); i++)
        {
            auto user = list[i].toObject();
            auto entry = user["user"].toString() + ":" + user["pass"].toString();

            if (entry == item->text().trimmed())
            {
                list.removeAt(i);
                socksSettings["accounts"] = list;
                LOG(MODULE_UI, "Removed http inbound user " + entry)
                socksAccountListBox->takeItem(socksAccountListBox->currentRow());
                return;
            }
        }
    }
    else
    {
        QvMessageBoxWarn(this, tr("Removing a user"), tr("You haven't selected a user yet."));
    }
}

void InboundEditor::on_socksAddUserBtn_clicked()
{
    CHECKLOADING
    auto user = socksAddUserTxt->text();
    auto pass = socksAddPasswordTxt->text();
    //
    auto list = socksSettings["accounts"].toArray();

    for (int i = 0; i < list.count(); i++)
    {
        auto _user = list[i].toObject();

        if (_user["user"].toString() == user)
        {
            QvMessageBoxWarn(this, tr("Add a user"), tr("This user exists already."));
            return;
        }
    }

    socksAddUserTxt->clear();
    socksAddPasswordTxt->clear();
    QJsonObject entry;
    entry["user"] = user;
    entry["pass"] = pass;
    list.append(entry);
    socksAccountListBox->addItem(user + ":" + pass);
    socksSettings["accounts"] = list;
}

void InboundEditor::on_strategyCombo_currentIndexChanged(const QString &arg1)
{
    CHECKLOADING
    allocate["strategy"] = arg1.toLower();
}

void InboundEditor::on_refreshNumberBox_valueChanged(int arg1)
{
    CHECKLOADING
    allocate["refresh"] = arg1;
}

void InboundEditor::on_concurrencyNumberBox_valueChanged(int arg1)
{
    CHECKLOADING
    allocate["concurrency"] = arg1;
}

void InboundEditor::on_enableSniffingCB_stateChanged(int arg1)
{
    CHECKLOADING
    sniffing["enabled"] = arg1 == Qt::Checked;
    destOverrideList->setEnabled(arg1 == Qt::Checked);
}

void InboundEditor::on_destOverrideList_itemChanged(QListWidgetItem *item)
{
    CHECKLOADING
    Q_UNUSED(item)
    QJsonArray list;

    for (int i = 0; i < destOverrideList->count(); i++)
    {
        auto _item = destOverrideList->item(i);

        if (_item->checkState() == Qt::Checked)
        {
            list.append(_item->text().toLower());
        }
    }

    sniffing["destOverride"] = list;
}

void InboundEditor::on_socksUDPCB_stateChanged(int arg1)
{
    CHECKLOADING
    socksSettings["udp"] = arg1 == Qt::Checked;
}

void InboundEditor::on_socksUDPIPAddrTxt_textEdited(const QString &arg1)
{
    CHECKLOADING
    socksSettings["ip"] = arg1;
}

void InboundEditor::on_socksUserLevelSB_valueChanged(int arg1)
{
    CHECKLOADING
    socksSettings["userLevel"] = arg1;
}

void InboundEditor::on_dokoIPAddrTxt_textEdited(const QString &arg1)
{
    CHECKLOADING
    dokoSettings["address"] = arg1;
}

void InboundEditor::on_dokoPortSB_valueChanged(int arg1)
{
    CHECKLOADING
    dokoSettings["port"] = arg1;
}

void InboundEditor::on_dokoTCPCB_stateChanged(int arg1)
{
    CHECKLOADING
    Q_UNUSED(arg1)
    bool hasTCP = dokoTCPCB->checkState() == Qt::Checked;
    bool hasUDP = dokoUDPCB->checkState() == Qt::Checked;
    QString str = "";
    str += hasTCP ? "tcp" : "";
    str += (hasTCP && hasUDP) ? "," : "";
    str += hasUDP ? "udp" : "";
    dokoSettings["network"] = str;
}

void InboundEditor::on_dokoUDPCB_stateChanged(int arg1)
{
    CHECKLOADING
    Q_UNUSED(arg1)
    bool hasTCP = dokoTCPCB->checkState() == Qt::Checked;
    bool hasUDP = dokoUDPCB->checkState() == Qt::Checked;
    QString str = "";
    str += hasTCP ? "tcp" : "";
    str += (hasTCP && hasUDP) ? "," : "";
    str += hasUDP ? "udp" : "";
    dokoSettings["network"] = str;
}

void InboundEditor::on_dokoTimeoutSB_valueChanged(int arg1)
{
    CHECKLOADING
    dokoSettings["timeout"] = arg1;
}

void InboundEditor::on_dokoFollowRedirectCB_stateChanged(int arg1)
{
    CHECKLOADING
    dokoSettings["followRedirect"] = arg1 == Qt::Checked;
}

void InboundEditor::on_dokoUserLevelSB_valueChanged(int arg1)
{
    CHECKLOADING
    dokoSettings["userLevel"] = arg1;
}

void InboundEditor::on_dokotproxyCombo_currentIndexChanged(const QString &arg1)
{
    CHECKLOADING
    dokotproxy = arg1;
}

void InboundEditor::on_mtEMailTxt_textEdited(const QString &arg1)
{
    CHECKLOADING

    if (!mtSettings.contains("users"))
        mtSettings["users"] = QJsonArray();

    QJsonObject user = mtSettings["users"].toArray().empty() ? QJsonObject() : mtSettings["users"].toArray().first().toObject();
    user["email"] = arg1;
    QJsonArray list;
    list.append(user);
    mtSettings["users"] = list;
}

void InboundEditor::on_mtSecretTxt_textEdited(const QString &arg1)
{
    CHECKLOADING

    if (!mtSettings.contains("users"))
        mtSettings["users"] = QJsonArray();

    QJsonObject user = mtSettings["users"].toArray().empty() ? QJsonObject() : mtSettings["users"].toArray().first().toObject();
    user["secret"] = arg1;
    QJsonArray list;
    list.append(user);
    mtSettings["users"] = list;
}

void InboundEditor::on_mtUserLevelSB_valueChanged(int arg1)
{
    CHECKLOADING

    if (!mtSettings.contains("users"))
        mtSettings["users"] = QJsonArray();

    QJsonObject user = mtSettings["users"].toArray().empty() ? QJsonObject() : mtSettings["users"].toArray().first().toObject();
    user["userLevel"] = arg1;
    QJsonArray list;
    list.append(user);
    mtSettings["users"] = list;
}

void InboundEditor::on_inboundHostTxt_textEdited(const QString &arg1)
{
    CHECKLOADING
    root["listen"] = arg1;
}

void InboundEditor::on_inboundPortTxt_textEdited(const QString &arg1)
{
    CHECKLOADING
    root["port"] = arg1;
}

void InboundEditor::on_socksAuthCombo_currentIndexChanged(const QString &arg1)
{
    CHECKLOADING
    socksSettings["auth"] = arg1.toLower();
}
