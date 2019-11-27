﻿#ifndef W_INBOUNDEDITOR_H
#define W_INBOUNDEDITOR_H

#include <QDialog>
#include <QJsonObject>
#include <QListWidgetItem>
#include "ui_w_InboundEditor.h"

class InboundEditor : public QDialog, private Ui::InboundEditor
{
        Q_OBJECT

    public:
        explicit InboundEditor(QJsonObject root, QWidget *parent = nullptr);
        ~InboundEditor();
        QJsonObject OpenEditor();

    private slots:
        void on_inboundProtocolCombo_currentIndexChanged(const QString &arg1);

        void on_inboundProtocolCombo_currentIndexChanged(int index);

        void on_inboundTagTxt_textEdited(const QString &arg1);

        void on_httpTimeoutSpinBox_valueChanged(int arg1);

        void on_httpTransparentCB_stateChanged(int arg1);

        void on_httpUserLevelSB_valueChanged(int arg1);

        void on_httpRemoveUserBtn_clicked();

        void on_httpAddUserBtn_clicked();

        void on_strategyCombo_currentIndexChanged(const QString &arg1);

        void on_refreshNumberBox_valueChanged(int arg1);

        void on_concurrencyNumberBox_valueChanged(int arg1);

        void on_enableSniffingCB_stateChanged(int arg1);

        void on_destOverrideList_itemChanged(QListWidgetItem *item);

        void on_socksUDPCB_stateChanged(int arg1);

        void on_socksUDPIPAddrTxt_textEdited(const QString &arg1);

        void on_socksUserLevelSB_valueChanged(int arg1);

        void on_socksRemoveUserBtn_clicked();

        void on_socksAddUserBtn_clicked();

        void on_dokoIPAddrTxt_textEdited(const QString &arg1);

        void on_dokoPortSB_valueChanged(int arg1);

        void on_dokoTCPCB_stateChanged(int arg1);

        void on_dokoUDPCB_stateChanged(int arg1);

        void on_dokoTimeoutSB_valueChanged(int arg1);

        void on_dokoFollowRedirectCB_stateChanged(int arg1);

        void on_dokoUserLevelSB_valueChanged(int arg1);

        void on_mtEMailTxt_textEdited(const QString &arg1);

        void on_mtSecretTxt_textEdited(const QString &arg1);

        void on_mtUserLevelSB_valueChanged(int arg1);

        void on_inboundHostTxt_textEdited(const QString &arg1);

        void on_inboundPortTxt_textEdited(const QString &arg1);

        void on_socksAuthCombo_currentIndexChanged(const QString &arg1);

    private:
        QJsonObject GenerateNewRoot();
        void LoadUIData();
        QJsonObject original;
        QJsonObject root;
        //
        QJsonObject httpSettings;
        QJsonObject socksSettings;
        QJsonObject mtSettings;
        QJsonObject dokoSettings;
        //
        QJsonObject sniffing;
        QJsonObject allocate;
};

#endif // W_INBOUNDEDITOR_H
