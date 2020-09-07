#pragma once

#include "QvGUIPluginInterface.hpp"
#include "ui_httpin.h"

class HTTPInboundEditor
    : public Qv2rayPlugin::QvPluginEditor
    , private Ui::httpInEditor
{
    Q_OBJECT

  public:
    explicit HTTPInboundEditor(QWidget *parent = nullptr);

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
