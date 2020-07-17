#pragma once

#include "base/Qv2rayBase.hpp"
#include "ui_RuleWidget.h"

class QvNodeRuleWidget
    : public QWidget
    , private Ui::RuleWidget
{
    Q_OBJECT

  public:
    explicit QvNodeRuleWidget(QWidget *parent = nullptr);
    void ShowCurrentRuleDetail(std::shared_ptr<RuleObject> rule);

  signals:
    void OnSizeUpdated();

  private slots:
    void on_toolButton_clicked();
    void on_routeProtocolHTTPCB_stateChanged(int arg1);
    void on_routeProtocolTLSCB_stateChanged(int arg1);
    void on_routeProtocolBTCB_stateChanged(int arg1);
    void on_hostList_textChanged();
    void on_ipList_textChanged();
    void on_routePortTxt_textEdited(const QString &arg1);
    void on_routeUserTxt_textEdited(const QString &arg1);
    void on_netUDPRB_clicked();
    void on_netTCPRB_clicked();
    void on_sourceIPList_textChanged();
    void on_enableBalancerCB_stateChanged(int arg1);
    void on_ruleRenameBtn_clicked();
    void on_ruleEnableCB_stateChanged(int arg1);

  protected:
    void changeEvent(QEvent *e);
    std::shared_ptr<RuleObject> ruleptr;
    bool isLoading;

    inline void SetNetworkProperty()
    {
        QStringList list;
        if (netUDPRB->isChecked())
            list << "udp";
        if (netTCPRB->isChecked())
            list << "tcp";
        ruleptr->network = list.join(",");
    }

    inline void SetProtocolProperty()
    {
        QStringList protocols;
        if (routeProtocolTLSCB->isChecked())
            protocols.push_back("tls");
        if (routeProtocolHTTPCB->isChecked())
            protocols.push_back("http");
        if (routeProtocolBTCB->isChecked())
            protocols.push_back("bittorrent");
        ruleptr->protocol = protocols;
    }
};
