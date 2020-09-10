#pragma once
#include "base/Qv2rayBase.hpp"
#include "components/plugins/QvPluginHost.hpp"
#include "ui/common/QvMessageBus.hpp"
#include "ui/widgets/widgets/StreamSettingsWidget.hpp"
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

  private slots:
    void on_alterLineEdit_valueChanged(int arg1);
    void on_buttonBox_accepted();
    void on_idLineEdit_textEdited(const QString &arg1);
    void on_ipLineEdit_textEdited(const QString &arg1);
    void on_muxConcurrencyTxt_valueChanged(int arg1);
    void on_muxEnabledCB_stateChanged(int arg1);
    void on_outBoundTypeCombo_currentIndexChanged(int index);
    void on_portLineEdit_textEdited(const QString &arg1);
    void on_securityCombo_currentIndexChanged(const QString &arg1);
    void on_ss_emailTxt_textEdited(const QString &arg1);
    void on_ss_encryptionMethod_currentIndexChanged(const QString &arg1);
    void on_ss_otaCheckBox_stateChanged(int arg1);
    void on_ss_passwordTxt_textEdited(const QString &arg1);
    void on_tagTxt_textEdited(const QString &arg1);
    void on_testsEnabledCombo_currentIndexChanged(const QString &arg1);
    void on_useFPCB_stateChanged(int arg1);
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
    VLESSServerObject vless;
    ShadowSocksServerObject shadowsocks;
    //
    StreamSettingsWidget *streamSettingsWidget;
    //
    int builtInOutboundTypes;
    //
    QMap<int, QPair<ProtocolInfoObject, QvPluginEditor *>> pluginWidgets;
};
