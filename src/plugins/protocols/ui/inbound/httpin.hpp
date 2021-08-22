#pragma once

#include "QvPlugin/Gui/QvGUIPluginInterface.hpp"
#include "ui_httpin.h"

#include <QJsonArray>

class HTTPInboundEditor
    : public Qv2rayPlugin::Gui::PluginProtocolEditor
    , private Ui::httpInEditor
{
    Q_OBJECT

  public:
    explicit HTTPInboundEditor(QWidget *parent = nullptr);

    virtual void Load() override;
    virtual void Store() override;

  protected:
    void changeEvent(QEvent *e) override;

  private slots:
    void on_httpTimeoutSpinBox_valueChanged(int arg1);
    void on_httpTransparentCB_stateChanged(int arg1);
    void on_httpRemoveUserBtn_clicked();
    void on_httpAddUserBtn_clicked();

  private:
    bool isLoading = false;
};
