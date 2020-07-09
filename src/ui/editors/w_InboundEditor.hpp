#pragma once

#include "base/Qv2rayBase.hpp"
#include "ui/messaging/QvMessageBus.hpp"
#include "ui_w_InboundEditor.h"

#include <QDialog>
#include <QListWidgetItem>

class InboundEditor
    : public QDialog
    , private Ui::InboundEditor
{
    Q_OBJECT

  public:
    explicit InboundEditor(INBOUND root, QWidget *parent = nullptr);
    ~InboundEditor();
    INBOUND OpenEditor();

  private:
    QvMessageBusSlotDecl;

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

    void on_dokotproxyCombo_currentIndexChanged(const QString &arg1);

    void on_mtEMailTxt_textEdited(const QString &arg1);

    void on_mtSecretTxt_textEdited(const QString &arg1);

    void on_mtUserLevelSB_valueChanged(int arg1);

    void on_inboundHostTxt_textEdited(const QString &arg1);

    void on_inboundPortTxt_textEdited(const QString &arg1);

    void on_socksAuthCombo_currentIndexChanged(const QString &arg1);

  private:
    INBOUND GenerateNewRoot();
    void LoadUIData();
    INBOUND original;
    INBOUND root;
    //
    QJsonObject httpSettings;
    QJsonObject socksSettings;
    QJsonObject mtSettings;
    QJsonObject dokoSettings;
    QString dokotproxy;
    //
    QJsonObject sniffing;
    QJsonObject allocate;
};
