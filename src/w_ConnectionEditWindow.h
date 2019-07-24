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
        explicit ConnectionEditWindow(QJsonObject editRootObject, QString alias, QWidget *parent = nullptr);
        ~ConnectionEditWindow();
    signals:
        void s_reload_config(bool need_restart);
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

        void on_genJsonBtn_clicked();

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

    private:
        QString _alias;
        void LoadGUIContents();
        QJsonObject GenerateConnectionJson();
        QJsonObject original;
        StreamSettingsObject stream;
        VMessOut::ServerObject vmess;
        Ui::ConnectionEditWindow *ui;
};
#endif // CONFEDIT_H
