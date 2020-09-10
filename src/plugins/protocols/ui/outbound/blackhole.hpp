#pragma once

#include "QvGUIPluginInterface.hpp"
#include "ui_blackhole.h"

class BlackholeOutboundEditor
    : public Qv2rayPlugin::QvPluginEditor
    , private Ui::blackholeOutEditor
{
    Q_OBJECT

  public:
    explicit BlackholeOutboundEditor(QWidget *parent = nullptr);

    void SetHostAddress(const QString &, int) override{};
    QPair<QString, int> GetHostAddress() const override
    {
        return {};
    };

    void SetContent(const QJsonObject &content) override;
    const QJsonObject GetContent() const override
    {
        return content;
    };

  protected:
    void changeEvent(QEvent *e) override;
  private slots:
    void on_responseTypeCB_currentTextChanged(const QString &arg1);
};
