#ifndef W_QVOUTBOUNDEDITOR_H
#define W_QVOUTBOUNDEDITOR_H

#include <list>
#include <QDialog>
#include <QJsonArray>
#include <QJsonObject>
#include <QListWidgetItem>
#include "QvUtils.hpp"

#include "Node.hpp"
#include "FlowScene.hpp"
#include "FlowView.hpp"
#include "ConnectionStyle.hpp"

#include "Node.hpp"
#include "NodeData.hpp"
#include "FlowScene.hpp"
#include "FlowView.hpp"
#include "DataModelRegistry.hpp"

#include "routeNodeModels/QvRuleNodeModel.hpp"
#include "routeNodeModels/QvInboundNodeModel.hpp"
#include "routeNodeModels/QvOutboundNodeModel.hpp"

using QtNodes::DataModelRegistry;
using QtNodes::FlowScene;
using QtNodes::FlowView;
using QtNodes::Node;
using QtNodes::ConnectionStyle;
using QtNodes::TypeConverter;
using QtNodes::TypeConverterId;

using namespace Qv2ray::Components::Nodes;

#include "ui_w_RoutesEditor.h"

enum ROUTE_EDIT_MODE {
    RENAME_INBOUND,
    RENAME_OUTBOUND,
    RENAME_RULE,
};

class RouteEditor : public QDialog, private Ui::RouteEditor
{
        Q_OBJECT

    public:
        explicit RouteEditor(QJsonObject connection, QWidget *parent = nullptr);
        ~RouteEditor();
        CONFIGROOT OpenEditor();

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
    public slots:
        void onNodeClicked(QtNodes::Node &n);
        void onConnectionCreated(QtNodes::Connection const &c);
        void onConnectionDeleted(QtNodes::Connection const &c);

    private:
        void RenameItemTag(ROUTE_EDIT_MODE mode, const QString &originalTag, const QString &newTag);
        void ShowCurrentRuleDetail();
        //
        QString currentRuleTag;
        QString currentInboundOutboundTag;
        QMap<QString, QStringList> Balancers;
        QString DomainStrategy;
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
        QMap<QString, Node *> inboundNodes;
        QMap<QString, Node *> outboundNodes;
        QMap<QString, Node *> ruleNodes;
        //
        FlowScene *nodeScene;
        // ---------------------------- Extra Source File Headers ----------------
        void AddNewInbound(INBOUND in);
        void AddNewOutbound(OUTBOUND out);
        void AddNewRule(RuleObject rule);
};

#endif // W_QVOUTBOUNDEDITOR_H
