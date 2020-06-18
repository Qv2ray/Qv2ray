#pragma once

#include "base/Qv2rayBase.hpp"
#include "ui/messaging/QvMessageBus.hpp"
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
    void on_httpPathTxt_textEdited(const QString &arg1);

    void on_httpHostTxt_textChanged();

    void on_wsHeadersTxt_textChanged();

    void on_tcpRequestDefBtn_clicked();

    void on_tcpRespDefBtn_clicked();

    void on_tlsCB_stateChanged(int arg1);

    void on_soMarkSpinBox_valueChanged(int arg1);

    void on_tcpFastOpenCB_stateChanged(int arg1);

    void on_tProxyCB_currentIndexChanged(const QString &arg1);

    void on_quicSecurityCB_currentTextChanged(const QString &arg1);

    void on_quicKeyTxt_textEdited(const QString &arg1);

    void on_quicHeaderTypeCB_currentIndexChanged(const QString &arg1);

    void on_tcpHeaderTypeCB_currentIndexChanged(const QString &arg1);

    void on_wsPathTxt_textEdited(const QString &arg1);

    void on_kcpMTU_valueChanged(int arg1);

    void on_kcpTTI_valueChanged(int arg1);

    void on_kcpUploadCapacSB_valueChanged(int arg1);

    void on_kcpCongestionCB_stateChanged(int arg1);

    void on_kcpDownCapacitySB_valueChanged(int arg1);

    void on_kcpReadBufferSB_valueChanged(int arg1);

    void on_kcpWriteBufferSB_valueChanged(int arg1);

    void on_kcpHeaderType_currentTextChanged(const QString &arg1);

    void on_dsPathTxt_textEdited(const QString &arg1);

    void on_tcpRequestEditBtn_clicked();

    void on_tcpResponseEditBtn_clicked();

    void on_transportCombo_currentIndexChanged(int index);

    void on_transportCombo_currentIndexChanged(const QString &arg1);

    void on_serverNameTxt_textEdited(const QString &arg1);

    void on_allowInsecureCB_stateChanged(int arg1);

    void on_alpnTxt_textChanged();

    void on_allowInsecureCiphersCB_stateChanged(int arg1);

    void on_disableSessionResumptionCB_stateChanged(int arg1);

    void on_kcpSeedTxt_textEdited(const QString &arg1);

  private:
    QvMessageBusSlotDecl;
    StreamSettingsObject stream;
};
