#include "socksin.hpp"

#include "BuiltinProtocolPlugin.hpp"

SocksInboundEditor::SocksInboundEditor(QWidget *parent) : Qv2rayPlugin::Gui::PluginProtocolEditor(parent)
{
    setupUi(this);
    setProperty("QV2RAY_INTERNAL_HAS_STREAMSETTINGS", true);
}

void SocksInboundEditor::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type())
    {
        case QEvent::LanguageChange: retranslateUi(this); break;
        default: break;
    }
}

void SocksInboundEditor::Load()
{
    isLoading = true;
    // SOCKS
    socksAuthCombo->setCurrentText(settings["auth"].toString());
    socksUDPCB->setChecked(settings["udp"].toBool());
    socksUDPIPAddrTxt->setText(settings["ip"].toString());

    for (auto user : settings["accounts"].toArray())
    {
        socksAccountListBox->addItem(user.toObject()["user"].toString() + ":" + user.toObject()["pass"].toString());
    }
    isLoading = false;
}

void SocksInboundEditor::on_socksRemoveUserBtn_clicked()
{
    if (isLoading)
        return;
    if (socksAccountListBox->currentRow() != -1)
    {
        auto item = socksAccountListBox->currentItem();
        auto list = settings["accounts"].toArray();

        for (int i = 0; i < list.count(); i++)
        {
            auto user = list[i].toObject();
            auto entry = user["user"].toString() + ":" + user["pass"].toString();

            if (entry == item->text().trimmed())
            {
                list.removeAt(i);
                settings["accounts"] = list;
                socksAccountListBox->takeItem(socksAccountListBox->currentRow());
                return;
            }
        }
    }
    else
    {
        InternalProtocolSupportPlugin::ShowMessageBox(tr("Removing a user"), tr("You haven't selected a user yet."));
    }
}

void SocksInboundEditor::on_socksAddUserBtn_clicked()
{
    if (isLoading)
        return;
    auto user = socksAddUserTxt->text();
    auto pass = socksAddPasswordTxt->text();
    //
    auto list = settings["accounts"].toArray();

    for (int i = 0; i < list.count(); i++)
    {
        auto _user = list[i].toObject();

        if (_user["user"].toString() == user)
        {
            InternalProtocolSupportPlugin::ShowMessageBox(tr("Add a user"), tr("This user exists already."));
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
    settings["accounts"] = list;
}

void SocksInboundEditor::on_socksUDPCB_stateChanged(int arg1)
{
    if (isLoading)
        return;
    settings["udp"] = arg1 == Qt::Checked;
}

void SocksInboundEditor::on_socksUDPIPAddrTxt_textEdited(const QString &arg1)
{
    if (isLoading)
        return;
    settings["ip"] = arg1;
}

void SocksInboundEditor::on_socksAuthCombo_currentIndexChanged(int arg1)
{
    if (isLoading)
        return;
    settings["auth"] = socksAuthCombo->itemText(arg1).toLower();
}
