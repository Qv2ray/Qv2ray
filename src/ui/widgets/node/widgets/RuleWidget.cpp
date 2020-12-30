#include "RuleWidget.hpp"

#include "base/Qv2rayBase.hpp"
#include "utils/QvHelpers.hpp"

#define LOAD_FLAG_END isLoading = false;
#define LOAD_FLAG_BEGIN isLoading = true;
#define LOADINGCHECK                                                                                                                                 \
    if (isLoading)                                                                                                                                   \
        return;

#define rule (*(this->ruleptr))

const static auto Split_RemoveDuplicate_Sort = [](const QString &in) {
    auto entries = SplitLines(in);
    entries.removeDuplicates();
    entries.sort();
    return entries;
};

QvNodeRuleWidget::QvNodeRuleWidget(std::shared_ptr<NodeDispatcher> _dispatcher, QWidget *parent) : QvNodeWidget(_dispatcher, parent)
{
    setupUi(this);
    settingsFrame->setVisible(false);
    adjustSize();
}

void QvNodeRuleWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type())
    {
        case QEvent::LanguageChange:
        {
            retranslateUi(this);
            break;
        }
        default: break;
    }
}

void QvNodeRuleWidget::setValue(std::shared_ptr<RuleObject> _ruleptr)
{
    this->ruleptr = _ruleptr;
    // Switch to the detailed page.
    ruleEnableCB->setEnabled(true);
    ruleEnableCB->setChecked(rule.QV2RAY_RULE_ENABLED);
    ruleTagLineEdit->setEnabled(true);
    LOAD_FLAG_BEGIN
    ruleTagLineEdit->setText(rule.QV2RAY_RULE_TAG);
    isLoading = false;
    // Networks
    auto network = rule.network.toLower();
    netUDPRB->setChecked(network.contains("udp"));
    netTCPRB->setChecked(network.contains("tcp"));
    //
    // Set protocol checkboxes.
    auto protocol = rule.protocol;
    routeProtocolHTTPCB->setChecked(protocol.contains("http"));
    routeProtocolTLSCB->setChecked(protocol.contains("tls"));
    routeProtocolBTCB->setChecked(protocol.contains("bittorrent"));
    //
    // Port
    routePortTxt->setText(rule.port);
    //
    // Users
    const auto sourcePorts = rule.sourcePort;
    //
    // Incoming Sources
    const auto sources = rule.source.join(NEWLINE);
    sourceIPList->setPlainText(sources);
    //
    // Domains
    QString domains = rule.domain.join(NEWLINE);
    hostList->setPlainText(domains);
    //
    // Outcoming IPs
    QString ips = rule.ip.join(NEWLINE);
    ipList->setPlainText(ips);
    LOAD_FLAG_END
}

void QvNodeRuleWidget::on_routeProtocolHTTPCB_stateChanged(int)
{
    LOADINGCHECK
    SetProtocolProperty();
}

void QvNodeRuleWidget::on_routeProtocolTLSCB_stateChanged(int)
{
    LOADINGCHECK
    SetProtocolProperty();
}

void QvNodeRuleWidget::on_routeProtocolBTCB_stateChanged(int)
{
    LOADINGCHECK
    SetProtocolProperty();
}

void QvNodeRuleWidget::on_hostList_textChanged()
{
    LOADINGCHECK
    rule.domain = Split_RemoveDuplicate_Sort(hostList->toPlainText());
}

void QvNodeRuleWidget::on_ipList_textChanged()
{
    LOADINGCHECK
    rule.ip = Split_RemoveDuplicate_Sort(ipList->toPlainText());
}

void QvNodeRuleWidget::on_routePortTxt_textEdited(const QString &arg1)
{
    LOADINGCHECK
    rule.port = arg1;
}

void QvNodeRuleWidget::on_netUDPRB_clicked()
{
    LOADINGCHECK
    SetNetworkProperty();
}

void QvNodeRuleWidget::on_netTCPRB_clicked()
{
    LOADINGCHECK
    SetNetworkProperty();
}

void QvNodeRuleWidget::on_sourceIPList_textChanged()
{
    LOADINGCHECK
    rule.source = Split_RemoveDuplicate_Sort(sourceIPList->toPlainText());
}

void QvNodeRuleWidget::on_ruleEnableCB_stateChanged(int arg1)
{
    bool _isEnabled = arg1 == Qt::Checked;
    rule.QV2RAY_RULE_ENABLED = _isEnabled;
    settingsFrame->setEnabled(_isEnabled);
}

void QvNodeRuleWidget::on_toolButton_clicked()
{
    settingsFrame->setVisible(!settingsFrame->isVisible());
    adjustSize();
    emit OnSizeUpdated();
}

void QvNodeRuleWidget::on_ruleTagLineEdit_textEdited(const QString &arg1)
{
    const auto originalTag = rule.QV2RAY_RULE_TAG;
    if (originalTag == arg1 || dispatcher->RenameTag<NODE_RULE>(originalTag, arg1))
    {
        BLACK(ruleTagLineEdit);
        rule.QV2RAY_RULE_TAG = arg1;
        return;
    }
    RED(ruleTagLineEdit);
}
