#pragma once
#include "base/Qv2rayBase.hpp"
#include "components/plugins/QvPluginHost.hpp"
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
    explicit OutboundEditor(const OUTBOUND &outboundEntry, QWidget *parent = nullptr);
    ~OutboundEditor();
    OUTBOUND OpenEditor();
    QString GetFriendlyName();

  private:
    explicit OutboundEditor(QWidget *parent = nullptr);
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
    void on_ss_otaCheckBox_stateChanged(int arg1);
    void on_socks_UserNameTxt_textEdited(const QString &arg1);
    void on_socks_PasswordTxt_textEdited(const QString &arg1);
    void on_http_UserNameTxt_textEdited(const QString &arg1);
    void on_http_PasswordTxt_textEdited(const QString &arg1);
    void on_securityCombo_currentIndexChanged(const QString &arg1);

    void on_testsEnabledCombo_currentIndexChanged(const QString &arg1);

    void on_vLessIDTxt_textEdited(const QString &arg1);

    void on_vLessSecurityCombo_currentTextChanged(const QString &arg1);

  private:
    QString tag;
    void ReloadGUI();
    bool useForwardProxy;
    OUTBOUND GenerateConnectionJson();
    OUTBOUND originalConfig;
    OUTBOUND resultConfig;
    QJsonObject muxConfig;
    //
    // Connection Configs
    QString outboundType;
    QString serverAddress;
    int serverPort;
    //
    VMessServerObject vmess;
    VLessServerObject vless;
    ShadowSocksServerObject shadowsocks;
    SocksServerObject socks;
    HttpServerObject http;
    //
    StreamSettingsWidget *streamSettingsWidget;
    //
    int builtInOutboundTypes;
    //
    QMap<int, QPair<QvPluginOutboundProtocolObject, QvPluginEditor *>> pluginWidgets;
};
