#pragma once

#include "base/Qv2rayBase.hpp"
#include "ui/common/QvMessageBus.hpp"
#include "ui_StreamSettingsWidget.h"

#include <QWidget>

class StreamSettingsWidget
    : public QWidget
    , private Ui::StreamSettingsWidget
{
    Q_OBJECT

  public:
    explicit StreamSettingsWidget(QWidget *parent = nullptr);
    void SetStreamObject(const StreamSettingsObject &sso);
    StreamSettingsObject GetStreamSettings() const;

  private slots:
    void on_transportCombo_currentIndexChanged(int arg1);

    // Domain Socket
    void on_dsPathTxt_textEdited(const QString &arg1);

    // HTTP
    void on_httpHostTxt_textChanged();
    void on_httpPathTxt_textEdited(const QString &arg1);
    void on_httpMethodCB_currentTextChanged(const QString &arg1);
    void on_httpHeadersEditBtn_clicked();
    void on_httpHeadersDefBtn_clicked();

    // KCP
    void on_kcpCongestionCB_stateChanged(int arg1);
    void on_kcpDownCapacitySB_valueChanged(int arg1);
    void on_kcpHeaderType_currentIndexChanged(int arg1);
    void on_kcpMTU_valueChanged(int arg1);
    void on_kcpReadBufferSB_valueChanged(int arg1);
    void on_kcpSeedTxt_textEdited(const QString &arg1);
    void on_kcpTTI_valueChanged(int arg1);
    void on_kcpUploadCapacSB_valueChanged(int arg1);
    void on_kcpWriteBufferSB_valueChanged(int arg1);

    // QUIC
    void on_quicHeaderTypeCB_currentIndexChanged(int arg1);
    void on_quicKeyTxt_textEdited(const QString &arg1);
    void on_quicSecurityCB_currentIndexChanged(int arg1);

    // TLS/XTLS
    void on_allowInsecureCB_stateChanged(int arg1);
    void on_alpnTxt_textEdited(const QString &arg1);
    void on_enableSessionResumptionCB_stateChanged(int arg1);
    void on_securityTypeCB_currentIndexChanged(int arg1);
    void on_serverNameTxt_textEdited(const QString &arg1);
    void on_disableSystemRoot_stateChanged(int arg1);
    void on_openCertEditorBtn_clicked();

    // TCP
    void on_tcpFastOpenCB_stateChanged(int arg1);
    void on_tcpHeaderTypeCB_currentIndexChanged(int arg1);
    void on_tcpRequestDefBtn_clicked();
    void on_tcpRequestEditBtn_clicked();
    void on_tcpRespDefBtn_clicked();
    void on_tcpResponseEditBtn_clicked();

    // SOCKOPT
    void on_tProxyCB_currentIndexChanged(int arg1);
    void on_soMarkSpinBox_valueChanged(int arg1);
    void on_tcpKeepAliveIntervalSpinBox_valueChanged(int arg1);

    // WebSocket
    void on_wsHeadersTxt_textChanged();
    void on_wsPathTxt_textEdited(const QString &arg1);
    void on_wsEarlyDataSB_valueChanged(int arg1);
    void on_wsBrowserForwardCB_stateChanged(int arg1);
    void on_wsEarlyDataHeaderNameCB_currentIndexChanged(int arg1);

    // gRPC
    void on_grpcServiceNameTxt_textEdited(const QString &arg1);
    void on_grpcModeCB_currentIndexChanged(int arg1);
    //
    void on_pinnedPeerCertificateChainSha256Btn_clicked();

  private:
    QvMessageBusSlotDecl;
    StreamSettingsObject stream;
};
