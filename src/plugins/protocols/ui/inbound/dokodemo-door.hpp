#pragma once

#include "QvGUIPluginInterface.hpp"
#include "ui_dokodemo-door.h"

class DokodemoDoorInboundEditor
    : public Qv2rayPlugin::QvPluginEditor
    , private Ui::dokodemodoorInEditor
{
    Q_OBJECT

  public:
    explicit DokodemoDoorInboundEditor(QWidget *parent = nullptr);

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
