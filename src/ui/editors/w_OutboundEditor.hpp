#ifndef CONFEDIT_H
#define CONFEDIT_H

#include <QtCore>
#include <QDialog>
#include "base/Qv2rayBase.hpp"
#include "ui_w_OutboundEditor.h"

class OutboundEditor : public QDialog, private Ui::OutboundEditor
{
        Q_OBJECT
    public:
        explicit OutboundEditor(QWidget *parent = nullptr);
        explicit OutboundEditor(OUTBOUND outboundEntry, QWidget *parent = nullptr);
        ~OutboundEditor();
        OUTBOUND OpenEditor();
        QString GetFriendlyName();
    signals:
        void s_reload_config(bool need_restart);
    private slots:
        void on_buttonBox_accepted();

        void on_ipLineEdit_textEdited(const QString &arg1);

        void on_portLineEdit_textEdited(const QString &arg1);

        void on_idLineEdit_textEdited(const QString &arg1);

        void on_securityCombo_currentIndexChanged(const QString &arg1);

        void on_tranportCombo_currentIndexChanged(const QString &arg1);

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

        void on_tranportCombo_currentIndexChanged(int index);

        void on_dsPathTxt_textEdited(const QString &arg1);

        void on_outBoundTypeCombo_currentIndexChanged(int index);

        void on_ss_emailTxt_textEdited(const QString &arg1);

        void on_ss_passwordTxt_textEdited(const QString &arg1);

        void on_ss_encryptionMethod_currentIndexChanged(const QString &arg1);

        void on_ss_levelSpin_valueChanged(int arg1);

        void on_ss_otaCheckBox_stateChanged(int arg1);

        void on_socks_UserNameTxt_textEdited(const QString &arg1);

        void on_socks_PasswordTxt_textEdited(const QString &arg1);

        void on_tcpRequestEditBtn_clicked();

        void on_tcpResponseEditBtn_clicked();

        void on_tagTxt_textEdited(const QString &arg1);

        void on_muxEnabledCB_stateChanged(int arg1);

        void on_muxConcurrencyTxt_valueChanged(int arg1);

        void on_alterLineEdit_valueChanged(int arg1);

        void on_useFPCB_stateChanged(int arg1);

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
        StreamSettingsObject stream;
        VMessServerObject vmess;
        ShadowSocksServerObject shadowsocks;
        SocksServerObject socks;
        //
};
#endif // CONFEDIT_H
