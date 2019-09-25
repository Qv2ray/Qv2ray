#ifndef CONFEDIT_H
#define CONFEDIT_H

#include <QtCore>
#include <QDialog>
#include "QvCoreConfigObjects.h"
#include "ui_w_ConnectionEditWindow.h"

namespace Ui
{
    class ConnectionEditWindow;
}

class ConnectionEditWindow : public QDialog
{
        Q_OBJECT

    public:
        explicit ConnectionEditWindow(QWidget *parent = nullptr);
        QJsonObject Result;
        QString Tag;
        QString Alias;
        QJsonObject OpenEditor();
        explicit ConnectionEditWindow(QJsonObject outboundEntry, QString *alias, QWidget *parent = nullptr);
        ~ConnectionEditWindow();
    signals:
        void s_reload_config(bool need_restart);
    private:
        void on_genJsonBtn_clicked();
    private slots:
        void on_buttonBox_accepted();

        void on_ipLineEdit_textEdited(const QString &arg1);

        void on_portLineEdit_textEdited(const QString &arg1);

        void on_idLineEdit_textEdited(const QString &arg1);

        void on_alterLineEdit_textEdited(const QString &arg1);

        void on_securityCombo_currentIndexChanged(const QString &arg1);

        void on_tranportCombo_currentIndexChanged(const QString &arg1);

        void on_httpPathTxt_textEdited(const QString &arg1);

        void on_httpHostTxt_textChanged();

        void on_wsHeadersTxt_textChanged();

        void on_tcpRequestDefBtn_clicked();

        void on_tcpRespDefBtn_clicked();

        void on_tcpRequestTxt_textChanged();

        void on_tcpRespTxt_textChanged();

        void on_tlsCB_stateChanged(int arg1);

        void on_soMarkSpinBox_valueChanged(int arg1);

        void on_tcpFastOpenCB_stateChanged(int arg1);

        void on_tProxyCB_currentIndexChanged(const QString &arg1);

        void on_quicSecurityCB_currentTextChanged(const QString &arg1);

        void on_quicKeyTxt_textEdited(const QString &arg1);

        void on_quicHeaderTypeCB_currentIndexChanged(const QString &arg1);

        void on_tcpRequestPrettifyBtn_clicked();

        void on_tcpRespPrettifyBtn_clicked();

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

    private:
        int rootJsonCursorPos;
        void ReLoad_GUI_JSON_ModelContent();
        QJsonObject GenerateConnectionJson();
        Ui::ConnectionEditWindow *ui;
        //
        // Connection Configs
        QString OutboundType;
        StreamSettingsObject stream;
        VMessServerObject vmess;
        ShadowSocksServer shadowsocks;
        SocksServerObject socks;
        //
};
#endif // CONFEDIT_H
