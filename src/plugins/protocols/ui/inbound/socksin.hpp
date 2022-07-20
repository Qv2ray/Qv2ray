#pragma once

#include "QvPlugin/Gui/QvGUIPluginInterface.hpp"
#include "ui_socksin.h"

#include <QJsonArray>

class SocksInboundEditor
    : public Qv2rayPlugin::Gui::PluginProtocolEditor
    , private Ui::socksInEditor
{
    Q_OBJECT

  public:
    explicit SocksInboundEditor(QWidget *parent = nullptr);

    void Load() override;
    void Store() override{};

  private slots:
    void on_socksUDPCB_stateChanged(int arg1);
    void on_socksUDPIPAddrTxt_textEdited(const QString &arg1);
    void on_socksRemoveUserBtn_clicked();
    void on_socksAddUserBtn_clicked();
    void on_socksAuthCombo_currentIndexChanged(int arg1);

  protected:
    void changeEvent(QEvent *e) override;

  private:
    bool isLoading = false;
};
