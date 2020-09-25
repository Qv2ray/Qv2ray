#pragma once

#include "CommonTypes.hpp"
#include "QvGUIPluginInterface.hpp"
#include "ui_vmess.h"

class VmessOutboundEditor
    : public Qv2rayPlugin::QvPluginEditor
    , private Ui::vmessOutEditor
{
    Q_OBJECT

  public:
    explicit VmessOutboundEditor(QWidget *parent = nullptr);

    void SetHostAddress(const QString &addr, int port) override
    {
        vmess.address = addr;
        vmess.port = port;
    }

    QPair<QString, int> GetHostAddress() const override
    {
        return { vmess.address, vmess.port };
    }

    void SetContent(const QJsonObject &content) override
    {
        this->content = content;
        PLUGIN_EDITOR_LOADING_SCOPE({
            if (content["vnext"].toArray().isEmpty())
                content["vnext"] = QJsonArray{ QJsonObject{} };
            vmess = VMessServerObject::fromJson(content["vnext"].toArray().first().toObject());
            if (vmess.users.empty())
                vmess.users.push_back({});
            const auto &user = vmess.users.front();
            idLineEdit->setText(user.id);
            alterLineEdit->setValue(user.alterId);
            securityCombo->setCurrentText(user.security);
        })
    }
    const QJsonObject GetContent() const override
    {
        auto result = content;
        QJsonArray vnext;
        vnext.append(vmess.toJson());
        result.insert("vnext", vnext);
        return result;
    }

  private:
    VMessServerObject vmess;

  protected:
    void changeEvent(QEvent *e) override;

  private slots:
    void on_idLineEdit_textEdited(const QString &arg1);
    void on_securityCombo_currentIndexChanged(const QString &arg1);
    void on_alterLineEdit_valueChanged(int arg1);
};
