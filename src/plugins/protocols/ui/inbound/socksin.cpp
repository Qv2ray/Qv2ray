#include "socksin.hpp"

#include "QvPluginInterface.hpp"

#define CHECKLOADING                                                                                                                            \
    if (isLoading)                                                                                                                              \
        return;

SocksInboundEditor::SocksInboundEditor(QWidget *parent) : Qv2rayPlugin::QvPluginEditor(parent)
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

void SocksInboundEditor::SetContent(const QJsonObject &content)
{
    this->content = content;
    // SOCKS
    socksAuthCombo->setCurrentText(content["auth"].toString());
    socksUDPCB->setChecked(content["udp"].toBool());
    socksUDPIPAddrTxt->setText(content["ip"].toString());

    for (auto user : content["accounts"].toArray())
    {
        socksAccountListBox->addItem(user.toObject()["user"].toString() + ":" + user.toObject()["pass"].toString());
    }
}

void SocksInboundEditor::on_socksRemoveUserBtn_clicked()
{
    CHECKLOADING

    if (socksAccountListBox->currentRow() != -1)
    {
        auto item = socksAccountListBox->currentItem();
        auto list = content["accounts"].toArray();

        for (int i = 0; i < list.count(); i++)
        {
            auto user = list[i].toObject();
            auto entry = user["user"].toString() + ":" + user["pass"].toString();

            if (entry == item->text().trimmed())
            {
                list.removeAt(i);
                content["accounts"] = list;
                socksAccountListBox->takeItem(socksAccountListBox->currentRow());
                return;
            }
        }
    }
    else
    {
        Qv2rayPlugin::pluginInstance->PluginErrorMessageBox(tr("Removing a user"), tr("You haven't selected a user yet."));
    }
}

void SocksInboundEditor::on_socksAddUserBtn_clicked()
{
    CHECKLOADING
    auto user = socksAddUserTxt->text();
    auto pass = socksAddPasswordTxt->text();
    //
    auto list = content["accounts"].toArray();

    for (int i = 0; i < list.count(); i++)
    {
        auto _user = list[i].toObject();

        if (_user["user"].toString() == user)
        {
            Qv2rayPlugin::pluginInstance->PluginErrorMessageBox(tr("Add a user"), tr("This user exists already."));
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
    content["accounts"] = list;
}

void SocksInboundEditor::on_socksUDPCB_stateChanged(int arg1)
{
    CHECKLOADING
    content["udp"] = arg1 == Qt::Checked;
}

void SocksInboundEditor::on_socksUDPIPAddrTxt_textEdited(const QString &arg1)
{
    CHECKLOADING
    content["ip"] = arg1;
}

void SocksInboundEditor::on_socksAuthCombo_currentIndexChanged(const QString &arg1)
{
    CHECKLOADING
    content["auth"] = arg1.toLower();
}
