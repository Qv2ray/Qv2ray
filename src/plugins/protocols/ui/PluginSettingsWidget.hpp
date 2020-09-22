#pragma once

#include "QvGUIPluginInterface.hpp"
#include "ui_PluginSettingsWidget.h"
class SimplePluginSettingsWidget
    : public Qv2rayPlugin::QvPluginSettingsWidget
    , private Ui::PluginSettingsWidget
{
    Q_OBJECT

  public:
    explicit SimplePluginSettingsWidget(QWidget *parent = nullptr);
    void SetSettings(const QJsonObject &) override{};
    QJsonObject GetSettings() override
    {
        return {};
    };

  protected:
    void changeEvent(QEvent *e) override;
};
