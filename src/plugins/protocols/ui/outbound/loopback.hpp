#pragma once

#include "QvGUIPluginInterface.hpp"
#include "ui_loopback.h"

class LoopbackSettingsEditor
    : public Qv2rayPlugin::QvPluginEditor
    , private Ui::loopback
{
    Q_OBJECT

  public:
    explicit LoopbackSettingsEditor(QWidget *parent = nullptr);
    void SetHostAddress(const QString &, int){};
    QPair<QString, int> GetHostAddress() const
    {
        return {};
    };

    void SetContent(const QJsonObject &content)
    {
        loopbackSettings = content;
        inboundTagTxt->setText(content["inboundTag"].toString());
    }

    const QJsonObject GetContent() const
    {
        return loopbackSettings;
    }

  protected:
    void changeEvent(QEvent *e);

  private slots:
    void on_inboundTagTxt_textEdited(const QString &arg1);

  private:
    QJsonObject loopbackSettings;
};
