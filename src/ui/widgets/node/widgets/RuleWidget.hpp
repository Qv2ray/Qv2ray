#pragma once

#include "base/Qv2rayBase.hpp"
#include "ui/widgets/node/NodeBase.hpp"
#include "ui_RuleWidget.h"

class QvNodeRuleWidget
    : public QvNodeWidget
    , private Ui::RuleWidget
{
    Q_OBJECT

  public:
    explicit QvNodeRuleWidget(std::shared_ptr<NodeDispatcher> _dispatcher, QWidget *parent = nullptr);
    void setValue(std::shared_ptr<RuleObject>);
    inline void SetDetailsVisibilityState(bool state)
    {
        settingsFrame->setVisible(state);
        adjustSize();
    }

  private slots:
    void on_toolButton_clicked();
    void on_routeProtocolHTTPCB_stateChanged(int arg1);
    void on_routeProtocolTLSCB_stateChanged(int arg1);
    void on_routeProtocolBTCB_stateChanged(int arg1);
    void on_hostList_textChanged();
    void on_ipList_textChanged();
    void on_routePortTxt_textEdited(const QString &arg1);
    void on_netUDPRB_clicked();
    void on_netTCPRB_clicked();
    void on_sourceIPList_textChanged();
    void on_ruleEnableCB_stateChanged(int arg1);
    void on_ruleTagLineEdit_textEdited(const QString &arg1);

    // void on_enableBalancerCB_stateChanged(int arg1);

  protected:
    void changeEvent(QEvent *e) override;
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
