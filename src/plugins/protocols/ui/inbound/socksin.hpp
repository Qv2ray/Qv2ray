#pragma once

#include "QvGUIPluginInterface.hpp"
#include "ui_socksin.h"

#include <QJsonArray>

class SocksInboundEditor
    : public Qv2rayPlugin::QvPluginEditor
    , private Ui::socksInEditor
{
    Q_OBJECT

  public:
    explicit SocksInboundEditor(QWidget *parent = nullptr);

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

  private slots:

    void on_socksUDPCB_stateChanged(int arg1);

    void on_socksUDPIPAddrTxt_textEdited(const QString &arg1);

    void on_socksRemoveUserBtn_clicked();

    void on_socksAddUserBtn_clicked();

    void on_socksAuthCombo_currentIndexChanged(int arg1);

  protected:
    void changeEvent(QEvent *e) override;
};
