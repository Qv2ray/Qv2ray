#pragma once

#include "CommonTypes.hpp"
#include "QvGUIPluginInterface.hpp"
#include "ui_vless.h"

class VlessOutboundEditor
    : public Qv2rayPlugin::QvPluginEditor
    , private Ui::vlessOutEditor
{
    Q_OBJECT

  public:
    explicit VlessOutboundEditor(QWidget *parent = nullptr);

    void SetHostAddress(const QString &addr, int port) override
    {
        vless.address = addr;
        vless.port = port;
    }
    QPair<QString, int> GetHostAddress() const override
    {
        return { vless.address, vless.port };
    }

    void SetContent(const QJsonObject &content) override
    {
        this->content = content;
        PLUGIN_EDITOR_LOADING_SCOPE({
            if (content["vnext"].toArray().isEmpty())
                content["vnext"] = QJsonArray{ QJsonObject{} };
            vless = VLESSServerObject::fromJson(content["vnext"].toArray().first().toObject());
            if (vless.users.isEmpty())
                vless.users.push_back({});
            const auto &user = vless.users.front();
            vLessIDTxt->setText(user.id);
            vLessSecurityCombo->setCurrentText(user.encryption);
            flowCombo->setCurrentText(user.flow);
        })
    }

    const QJsonObject GetContent() const override
    {
        auto result = content;
        QJsonArray vnext;
        vnext.append(vless.toJson());
        result.insert("vnext", vnext);
        return result;
    }

  protected:
    void changeEvent(QEvent *e) override;

  private:
    VLESSServerObject vless;

  private slots:
    void on_flowCombo_currentTextChanged(const QString &arg1);
    void on_vLessIDTxt_textEdited(const QString &arg1);
    void on_vLessSecurityCombo_currentTextChanged(const QString &arg1);
};
