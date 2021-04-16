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
        if (content["vnext"].toArray().isEmpty())
            content["vnext"] = QJsonArray{ QJsonObject{} };
        QJS_BINDING_CLEAR
        vless.loadJson(content["vnext"].toArray().first().toObject());
        if (vless.users->isEmpty())
            vless.users->push_back({});
        QJS_RWBINDING(vless.users->first().encryption, vLessSecurityCombo, "currentText", &QComboBox::currentIndexChanged)
        QJS_RWBINDING(vless.users->first().flow, flowCombo, "currentText", &QComboBox::currentIndexChanged)
        QJS_RWBINDING(vless.users->first().id, vLessIDTxt, "text", &QLineEdit::textEdited)
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
    QJS_BINDING_HELPERS
};
