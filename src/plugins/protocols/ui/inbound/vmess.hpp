#pragma once

#include "QvGUIPluginInterface.hpp"
#include "ui_PluginInboundEditor.h"

class SimplePluginInboundEditor
    : public Qv2rayPlugin::QvPluginEditor
    , private Ui::PluginInboundEditor
{
    Q_OBJECT

  public:
    explicit SimplePluginInboundEditor(QWidget *parent = nullptr);

    void SetHostAddress(const QString &, int) override{};
    QPair<QString, int> GetHostAddress() const override
    {
        return {};
    };

    void SetContent(const QJsonObject &content) override
    {
        this->content = content;
    };
    const QJsonObject GetContent() const override
    {
        return content;
    };

  protected:
    void changeEvent(QEvent *e) override;
};
