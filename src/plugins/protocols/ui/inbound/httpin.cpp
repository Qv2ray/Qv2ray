#include "httpin.hpp"

#include "BuiltinProtocolPlugin.hpp"

HTTPInboundEditor::HTTPInboundEditor(QWidget *parent) : Qv2rayPlugin::Gui::PluginProtocolEditor(parent)
{
    setupUi(this);
    setProperty("QV2RAY_INTERNAL_HAS_STREAMSETTINGS", true);
}

void HTTPInboundEditor::Store()
{
    // Remove useless, misleading 'accounts' array.
    if (settings["accounts"].toArray().count() == 0)
        settings.remove("accounts");
}

void HTTPInboundEditor::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type())
    {
        case QEvent::LanguageChange: retranslateUi(this); break;
        default: break;
    }
}

void HTTPInboundEditor::Load()
{
    isLoading = true;
    httpTimeoutSpinBox->setValue(settings[u"timeout"_qs].toInt());
    httpTransparentCB->setChecked(settings[u"allowTransparent"_qs].toBool());
    httpAccountListBox->clear();

    for (const auto &user : settings[u"accounts"_qs].toArray())
    {
        httpAccountListBox->addItem(user.toObject()[u"user"_qs].toString() + ":" + user.toObject()[u"pass"_qs].toString());
    }
    isLoading = false;
}

void HTTPInboundEditor::on_httpTimeoutSpinBox_valueChanged(int arg1)
{
    if (isLoading)
        return;
    settings[u"timeout"_qs] = arg1;
}

void HTTPInboundEditor::on_httpTransparentCB_stateChanged(int arg1)
{
    if (isLoading)
        return;
    settings[u"allowTransparent"_qs] = arg1 == Qt::Checked;
}

void HTTPInboundEditor::on_httpRemoveUserBtn_clicked()
{
    if (isLoading)
        return;
    if (httpAccountListBox->currentRow() < 0)
    {
        InternalProtocolSupportPlugin::ShowMessageBox(tr("Removing a user"), tr("You haven't selected a user yet."));
        return;
    }
    const auto item = httpAccountListBox->currentItem();
    auto list = settings[u"accounts"_qs].toArray();

    for (int i = 0; i < list.count(); i++)
    {
        const auto user = list[i].toObject();
        const auto entry = user[u"user"_qs].toString() + ":" + user[u"pass"_qs].toString();
        if (entry == item->text().trimmed())
        {
            list.removeAt(i);
            settings[u"accounts"_qs] = list;
            httpAccountListBox->takeItem(httpAccountListBox->currentRow());
            return;
        }
    }
}

void HTTPInboundEditor::on_httpAddUserBtn_clicked()
{
    if (isLoading)
        return;
    const auto user = httpAddUserTxt->text();
    const auto pass = httpAddPasswordTxt->text();
    //
    auto list = settings[u"accounts"_qs].toArray();

    for (int i = 0; i < list.count(); i++)
    {
        const auto _user = list[i].toObject();
        if (_user[u"user"_qs].toString() == user)
        {
            InternalProtocolSupportPlugin::ShowMessageBox(tr("Add a user"), tr("This user exists already."));
            return;
        }
    }

    httpAddUserTxt->clear();
    httpAddPasswordTxt->clear();
    list.append(QJsonObject{ { "user", user }, { "pass", pass } });
    httpAccountListBox->addItem(user + ":" + pass);
    settings[u"accounts"_qs] = list;
}
