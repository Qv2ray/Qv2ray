#include "w_InboundEditor.h"
#include "ui_w_InboundEditor.h"
#include "QvUtils.h"
#include "QvCoreConfigOperations.h"

static bool isLoading = false;
#define PREPARE_RETURN if(isLoading) return;

InboundEditor::InboundEditor(QJsonObject root, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InboundEditor)
{
    original = root;
    this->root = root;
    auto inboundType = root["protocol"].toString();
    allocate = root["allocate"].toObject();
    sniffing  = root["sniffing"].toObject();

    if (inboundType == "http") {
        httpSettings = root["settings"].toObject();
    } else if (inboundType == "socks") {
        socksSettings = root["settings"].toObject();
    } else if (inboundType == "dokodemo-door") {
        dokoSettings = root["settings"].toObject();
    } else if (inboundType == "mtproto") {
        mtSettings = root["settings"].toObject();
    } else {
        LOG(MODULE_UI, "Unsupported inbound type: " + inboundType.toStdString() + ", decisions should be made if to open JSONEDITOR")
        QvMessageBox(this, tr("Inbound type not supported"), tr("The inbound type is not supported by Qv2ray (yet). Please use JsonEditor to change the settings") + "\r\n" +
                     tr("Inbound: ") + inboundType);
    }

    ui->setupUi(this);
    LoadUIData();
}

QJsonObject InboundEditor::OpenEditor()
{
    int resultCode = this->exec();
    return resultCode == QDialog::Accepted ? GenerateNewRoot() : original;
}

QJsonObject InboundEditor::GenerateNewRoot()
{
    QJsonObject _newRoot = root;
    auto inboundType = root["protocol"].toString();

    if (inboundType == "http") {
        _newRoot["settings"] = httpSettings;
    } else if (inboundType == "socks") {
        _newRoot["settings"] = socksSettings;
    } else if (inboundType == "dokodemo-door") {
        _newRoot["settings"] = dokoSettings;
    } else if (inboundType == "mtproto") {
        _newRoot["settings"] = mtSettings;
    }

    _newRoot["sniffing"] = sniffing;
    _newRoot["allocate"] = allocate;
    return _newRoot;
}

void InboundEditor::LoadUIData()
{
    isLoading = true;
    ui->strategyCombo->setCurrentText(allocate["strategy"].toString());
    ui->refreshNumberBox->setValue(allocate["refresh"].toInt());
    ui->concurrencyNumberBox->setValue(allocate["concurrency"].toInt());
    ui->enableSniffingCB->setChecked(sniffing["enabled"].toBool());

    foreach (auto item, sniffing["destOverride"].toArray()) {
        if (item.toString().toLower() == "http") ui->destOverrideList->item(0)->setCheckState(Qt::Checked);

        if (item.toString().toLower() == "tls") ui->destOverrideList->item(1)->setCheckState(Qt::Checked);
    }

    //
    ui->inboundTagTxt->setText(root["tag"].toString());
    ui->inboundHostTxt->setText(root["listen"].toString());
    ui->inboundPortTxt->setText(root["port"].toVariant().toString());
    ui->inboundProtocolCombo->setCurrentText(root["protocol"].toString());
    // HTTP
    ui->httpTimeoutSpinBox->setValue(httpSettings["timeout"].toInt());
    ui->httpTransparentCB->setChecked(httpSettings["allowTransparent"].toBool());
    ui->httpUserLevelSB->setValue(httpSettings["userLevel"].toInt());
    ui->httpAccountListBox->clear();

    for (auto user : httpSettings["accounts"].toArray()) {
        ui->httpAccountListBox->addItem(user.toObject()["user"].toString() +  ":" + user.toObject()["pass"].toString());
    }

    // SOCKS
    ui->socksAuthCombo->setCurrentText(socksSettings["auth"].toString());
    ui->socksUDPCB->setChecked(socksSettings["udp"].toBool());
    ui->socksUDPIPAddrTxt->setText(socksSettings["ip"].toString());
    ui->socksUserLevelSB->setValue(socksSettings["userLevel"].toInt());

    for (auto user : socksSettings["accounts"].toArray()) {
        ui->socksAccountListBox->addItem(user.toObject()["user"].toString() +  ":" + user.toObject()["pass"].toString());
    }

    // Dokodemo-Door
    ui->dokoFollowRedirectCB->setChecked(dokoSettings["followRedirect"].toBool());
    ui->dokoIPAddrTxt->setText(dokoSettings["address"].toString());
    ui->dokoPortSB->setValue(dokoSettings["port"].toInt());
    ui->dokoTimeoutSB->setValue(dokoSettings["timeout"].toInt());
    ui->dokoUserLevelSB->setValue(dokoSettings["userLevel"].toInt());
    ui->dokoTCPCB->setChecked(dokoSettings["network"].toString().contains("tcp"));
    ui->dokoUDPCB->setChecked(dokoSettings["network"].toString().contains("udp"));
    // MTProto
    ui->mtEMailTxt->setText(mtSettings["users"].toArray().first()["email"].toString());
    ui->mtUserLevelSB->setValue(mtSettings["users"].toArray().first()["level"].toInt());
    ui->mtSecretTxt->setText(mtSettings["users"].toArray().first()["secret"].toString());
    isLoading = false;
}

InboundEditor::~InboundEditor()
{
    delete ui;
}

void InboundEditor::on_inboundProtocolCombo_currentIndexChanged(const QString &arg1)
{
    PREPARE_RETURN
    root["protocol"] = arg1.toLower();
}

void InboundEditor::on_inboundProtocolCombo_currentIndexChanged(int index)
{
    ui->stackedWidget->setCurrentIndex(index);
}

void InboundEditor::on_inboundTagTxt_textEdited(const QString &arg1)
{
    PREPARE_RETURN
    root["tag"] = arg1;
}

void InboundEditor::on_httpTimeoutSpinBox_valueChanged(int arg1)
{
    PREPARE_RETURN
    httpSettings["timtout"] = arg1;
}

void InboundEditor::on_httpTransparentCB_stateChanged(int arg1)
{
    PREPARE_RETURN
    httpSettings["allowTransparent"] = arg1 == Qt::Checked;
}

void InboundEditor::on_httpUserLevelSB_valueChanged(int arg1)
{
    PREPARE_RETURN
    httpSettings["userLevel"] = arg1;
}

void InboundEditor::on_httpRemoveUserBtn_clicked()
{
    PREPARE_RETURN

    if (ui->httpAccountListBox->currentRow() != -1) {
        auto userpass = ui->httpAccountListBox->currentItem();
        auto list = httpSettings["accounts"].toArray();

        for (int i = 0 ; i < list.count(); i++) {
            auto user = list[i].toObject();

            if (user["user"].toString() + ":" + user["pass"].toString() == userpass->text()) {
                list.removeAt(i);
                ui->httpAccountListBox->removeItemWidget(userpass);
                return;
            }
        }
    } else {
        QvMessageBox(this, tr("Removing a user"), tr("You haven't selected a user yet,"));
    }
}

void InboundEditor::on_httpAddUserBtn_clicked()
{
    PREPARE_RETURN
    auto user = ui->httpAddUserTxt->text();
    auto pass = ui->httpAddPasswordTxt->text();
    //
    auto list = httpSettings["accounts"].toArray();

    for (int i = 0 ; i < list.count(); i++) {
        auto _user = list[i].toObject();

        if (_user["user"].toString() == user) {
            QvMessageBox(this, tr("Add a user"), tr("This user exists already."));
            return;
        }
    }

    QJsonObject entry;
    entry["user"] = user;
    entry["pass"] = pass;
    list.append(entry);
    httpSettings["accounts"] = list;
}

void InboundEditor::on_strategyCombo_currentIndexChanged(const QString &arg1)
{
    PREPARE_RETURN
    allocate["strategy"] = arg1.toLower();
}

void InboundEditor::on_refreshNumberBox_valueChanged(int arg1)
{
    PREPARE_RETURN
    allocate["refresh"] = arg1;
}

void InboundEditor::on_concurrencyNumberBox_valueChanged(int arg1)
{
    PREPARE_RETURN
    allocate["concurrency"] = arg1;
}

void InboundEditor::on_enableSniffingCB_stateChanged(int arg1)
{
    PREPARE_RETURN
    sniffing["enabled"] = arg1 == Qt::Checked;
}

void InboundEditor::on_destOverrideList_itemChanged(QListWidgetItem *item)
{
    PREPARE_RETURN
    Q_UNUSED(item)
    QJsonArray list;

    for (int i = 0; i < ui->destOverrideList->count(); i++) {
        auto _item = ui->destOverrideList->item(i);

        if (item->checkState() == Qt::Checked) {
            list.append(_item->text().toLower());
        }
    }

    sniffing["destOverride"] = list;
}

void InboundEditor::on_socksUDPCB_stateChanged(int arg1)
{
    PREPARE_RETURN
    socksSettings["udp"] = arg1 == Qt::Checked;
}

void InboundEditor::on_socksUDPIPAddrTxt_textEdited(const QString &arg1)
{
    PREPARE_RETURN
    socksSettings["ip"] = arg1;
}

void InboundEditor::on_socksUserLevelSB_valueChanged(int arg1)
{
    PREPARE_RETURN
    socksSettings["userLevel"] = arg1;
}

void InboundEditor::on_socksRemoveUserBtn_clicked()
{
    PREPARE_RETURN

    if (ui->socksAccountListBox->currentRow() != -1) {
        auto userpass = ui->socksAccountListBox->currentItem();
        auto list = socksSettings["accounts"].toArray();

        for (int i = 0 ; i < list.count(); i++) {
            auto user = list[i].toObject();

            if (user["user"].toString() + ":" + user["pass"].toString() == userpass->text()) {
                list.removeAt(i);
                ui->socksAccountListBox->removeItemWidget(userpass);
                return;
            }
        }
    } else {
        QvMessageBox(this, tr("Removing a user"), tr("You haven't selected a user yet,"));
    }
}

void InboundEditor::on_socksAddUserBtn_clicked()
{
    PREPARE_RETURN
    auto user = ui->socksAddUserTxt->text();
    auto pass = ui->socksAddPasswordTxt->text();
    //
    auto list = socksSettings["accounts"].toArray();

    for (int i = 0 ; i < list.count(); i++) {
        auto _user = list[i].toObject();

        if (_user["user"].toString() == user) {
            QvMessageBox(this, tr("Add a user"), tr("This user exists already."));
            return;
        }
    }

    QJsonObject entry;
    entry["user"] = user;
    entry["pass"] = pass;
    list.append(entry);
    socksSettings["accounts"] = list;
}

void InboundEditor::on_dokoIPAddrTxt_textEdited(const QString &arg1)
{
    PREPARE_RETURN
    dokoSettings["address"] = arg1;
}

void InboundEditor::on_dokoPortSB_valueChanged(int arg1)
{
    PREPARE_RETURN
    dokoSettings["port"]  = arg1;
}

void InboundEditor::on_dokoTCPCB_stateChanged(int arg1)
{
    PREPARE_RETURN
    Q_UNUSED(arg1)
    bool hasTCP = ui->dokoTCPCB->checkState() == Qt::Checked;
    bool hasUDP = ui->dokoUDPCB->checkState() == Qt::Checked;
    QString str = "";
    str += hasTCP ? "tcp" : "";
    str += (hasTCP && hasUDP) ? "," : "";
    str += hasUDP ? "udp" : "";
    dokoSettings["network"] = str;
}

void InboundEditor::on_dokoUDPCB_stateChanged(int arg1)
{
    PREPARE_RETURN
    Q_UNUSED(arg1)
    bool hasTCP = ui->dokoTCPCB->checkState() == Qt::Checked;
    bool hasUDP = ui->dokoUDPCB->checkState() == Qt::Checked;
    QString str = "";
    str += hasTCP ? "tcp" : "";
    str += (hasTCP && hasUDP) ? "," : "";
    str += hasUDP ? "udp" : "";
    dokoSettings["network"] = str;
}

void InboundEditor::on_dokoTimeoutSB_valueChanged(int arg1)
{
    PREPARE_RETURN
    dokoSettings["timeout"] = arg1;
}

void InboundEditor::on_dokoFollowRedirectCB_stateChanged(int arg1)
{
    PREPARE_RETURN
    dokoSettings["followRedirect"] = arg1 == Qt::Checked;
}

void InboundEditor::on_dokoUserLevelSB_valueChanged(int arg1)
{
    PREPARE_RETURN
    dokoSettings["userLevel"] = arg1;
}

void InboundEditor::on_mtEMailTxt_textEdited(const QString &arg1)
{
    PREPARE_RETURN

    if (!mtSettings.contains("users")) mtSettings["users"] = QJsonArray();

    QJsonObject user = mtSettings["users"].toArray().empty() ? QJsonObject() : mtSettings["users"].toArray().first().toObject();
    user["email"] = arg1;
    QJsonArray list;
    list.append(user);
    mtSettings["users"] = list;
}

void InboundEditor::on_mtSecretTxt_textEdited(const QString &arg1)
{
    PREPARE_RETURN

    if (!mtSettings.contains("users")) mtSettings["users"] = QJsonArray();

    QJsonObject user = mtSettings["users"].toArray().empty() ? QJsonObject() : mtSettings["users"].toArray().first().toObject();
    user["secret"] = arg1;
    QJsonArray list;
    list.append(user);
    mtSettings["users"] = list;
}

void InboundEditor::on_mtUserLevelSB_valueChanged(int arg1)
{
    PREPARE_RETURN

    if (!mtSettings.contains("users")) mtSettings["users"] = QJsonArray();

    QJsonObject user = mtSettings["users"].toArray().empty() ? QJsonObject() : mtSettings["users"].toArray().first().toObject();
    user["userLevel"] = arg1;
    QJsonArray list;
    list.append(user);
    mtSettings["users"] = list;
}

void InboundEditor::on_inboundHostTxt_textEdited(const QString &arg1)
{
    PREPARE_RETURN
    root["listen"] = arg1;
}

void InboundEditor::on_inboundPortTxt_textEdited(const QString &arg1)
{
    PREPARE_RETURN
    root["port"] = arg1;
}

void InboundEditor::on_socksAuthCombo_currentIndexChanged(const QString &arg1)
{
    PREPARE_RETURN
    socksSettings["auth"] =  arg1.toLower();
}
