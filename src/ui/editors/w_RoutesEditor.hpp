#pragma once

#include "base/Qv2rayBase.hpp"
#include "common/QvHelpers.hpp"
#include "ui/common/QvDialog.hpp"
#include "ui/messaging/QvMessageBus.hpp"
#include "ui_w_RoutesEditor.h"

#include <nodes/internal/ConnectionStyle.hpp>
#include <nodes/internal/Node.hpp>
#include <nodes/internal/NodeData.hpp>

using QtNodes::ConnectionStyle;
using QtNodes::FlowScene;
using QtNodes::Node;

#define GRAPH_GLOBAL_OFFSET_X -80
#define GRAPH_GLOBAL_OFFSET_Y -10
enum ROUTE_EDIT_MODE
{
    RENAME_INBOUND,
    RENAME_OUTBOUND,
    RENAME_RULE,
};

class RouteEditor
    : public QvDialog
    , private Ui::RouteEditor
{
    Q_OBJECT

  public:
    explicit RouteEditor(QJsonObject connection, QWidget *parent = nullptr);
    ~RouteEditor();
    CONFIGROOT OpenEditor();
    void processCommands(QString, QStringList, QMap<QString, QString>) override{};

  private:
    void updateColorScheme() override{};
    QvMessageBusSlotDecl;

  private slots:
    void on_buttonBox_accepted();

    void on_insertDirectBtn_clicked();

    void on_routeProtocolHTTPCB_stateChanged(int arg1);

    void on_routeProtocolTLSCB_stateChanged(int arg1);

    void on_routeProtocolBTCB_stateChanged(int arg1);

    void on_balancerAddBtn_clicked();

    void on_balancerDelBtn_clicked();

    void on_hostList_textChanged();

    void on_ipList_textChanged();

    void on_routePortTxt_textEdited(const QString &arg1);

    void on_routeUserTxt_textEdited(const QString &arg1);

    void on_addRouteBtn_clicked();

    void on_netBothRB_clicked();

    void on_netUDPRB_clicked();

    void on_netTCPRB_clicked();

    void on_routeUserTxt_textChanged();

    void on_sourceIPList_textChanged();

    void on_enableBalancerCB_stateChanged(int arg1);

    void on_addDefaultBtn_clicked();

    void on_insertBlackBtn_clicked();

    void on_addInboundBtn_clicked();

    void on_addOutboundBtn_clicked();

    void on_ruleEnableCB_stateChanged(int arg1);

    void on_delBtn_clicked();

    void on_editBtn_clicked();

    void on_domainStrategyCombo_currentIndexChanged(const QString &arg1);

    void on_ruleRenameBtn_clicked();

    void on_defaultOutboundCombo_currentTextChanged(const QString &arg1);

    void on_importExistingBtn_clicked();

    void on_importGroupBtn_currentIndexChanged(int index);

  public slots:
    void onNodeClicked(Node &n);
    void onConnectionCreated(QtNodes::Connection const &c);
    void onConnectionDeleted(QtNodes::Connection const &c);

  private:
    bool isLoading = false;
    void RenameItemTag(ROUTE_EDIT_MODE mode, const QString originalTag, QString *newTag);
    void ShowCurrentRuleDetail();
    //
    QString currentRuleTag;
    QString currentInboundOutboundTag;
    QMap<QString, QStringList> balancers;
    QString domainStrategy;
    QString defaultOutbound;
    //
    QMap<QString, INBOUND> inbounds;
    QMap<QString, OUTBOUND> outbounds;
    QMap<QString, RuleObject> rules;
    //
    CONFIGROOT root;
    CONFIGROOT original;
    //
    // ---------------------------- Node Graph Impl --------------------------
    void SetupNodeWidget();
    QHash<QString, QUuid> inboundNodes;
    QHash<QString, QUuid> outboundNodes;
    QHash<QString, QUuid> ruleNodes;
    //
    FlowScene *nodeScene;
    // ---------------------------- Extra Source File Headers ----------------
    void AddInbound(INBOUND in);
    void AddOutbound(OUTBOUND out);
    void AddRule(RuleObject rule);
    QString AddNewRule();
    void ResolveDefaultOutboundTag(QString original, QString newTag);
};
