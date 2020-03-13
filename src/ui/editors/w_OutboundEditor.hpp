#pragma once
#include "base/Qv2rayBase.hpp"
#include "ui/messaging/QvMessageBus.hpp"
#include "ui/widgets/StreamSettingsWidget.hpp"
#include "ui_w_OutboundEditor.h"

#include <QDialog>
#include <QtCore>

class OutboundEditor
    : public QDialog
    , private Ui::OutboundEditor
{
    Q_OBJECT
  public:
    explicit OutboundEditor(QWidget *parent = nullptr);
    explicit OutboundEditor(OUTBOUND outboundEntry, QWidget *parent = nullptr);
    ~OutboundEditor();
    OUTBOUND OpenEditor();
    QString GetFriendlyName();

  private:
    QvMessageBusSlotDecl;
  signals:
    void s_reload_config(bool need_restart);
  private slots:
    void on_buttonBox_accepted();

    void on_ipLineEdit_textEdited(const QString &arg1);

    void on_portLineEdit_textEdited(const QString &arg1);

    void on_idLineEdit_textEdited(const QString &arg1);

    void on_tagTxt_textEdited(const QString &arg1);

    void on_muxEnabledCB_stateChanged(int arg1);

    void on_muxConcurrencyTxt_valueChanged(int arg1);

    void on_alterLineEdit_valueChanged(int arg1);

    void on_useFPCB_stateChanged(int arg1);

    void on_outBoundTypeCombo_currentIndexChanged(int index);

    void on_ss_emailTxt_textEdited(const QString &arg1);

    void on_ss_passwordTxt_textEdited(const QString &arg1);

    void on_ss_encryptionMethod_currentIndexChanged(const QString &arg1);

    void on_ss_levelSpin_valueChanged(int arg1);

    void on_ss_otaCheckBox_stateChanged(int arg1);

    void on_socks_UserNameTxt_textEdited(const QString &arg1);

    void on_socks_PasswordTxt_textEdited(const QString &arg1);

    void on_securityCombo_currentIndexChanged(const QString &arg1);

  private:
    QString Tag;
    void ReloadGUI();
    bool useFProxy;
    OUTBOUND GenerateConnectionJson();
    OUTBOUND Original;
    OUTBOUND Result;
    QJsonObject Mux;
    //
    // Connection Configs
    QString OutboundType;
    //
    VMessServerObject vmess;
    ShadowSocksServerObject shadowsocks;
    SocksServerObject socks;
    //
    StreamSettingsWidget *ssWidget;
};
