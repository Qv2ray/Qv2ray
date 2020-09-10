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
    }

    void SetContent(const QJsonObject &content) override;
    const QJsonObject GetContent() const override;

  protected:
    void changeEvent(QEvent *e) override;

  private slots:
    void on_dokoFollowRedirectCB_stateChanged(int arg1);
    void on_dokoIPAddrTxt_textEdited(const QString &arg1);
    void on_dokoPortSB_valueChanged(int arg1);
    void on_dokoTCPCB_stateChanged(int arg1);
    void on_dokoUDPCB_stateChanged(int arg1);
    void on_dokoTimeoutSB_valueChanged(int arg1);
};
