#pragma once

#include "base/Qv2rayBase.hpp"
#include "common/QvHelpers.hpp"
#include "ui/common/QvDialog.hpp"
#include "ui/common/UIBase.hpp"
#include "ui/messaging/QvMessageBus.hpp"
#include "ui/node/NodeBase.hpp"
#include "ui_w_RoutesEditor.h"

#include <nodes/internal/ConnectionStyle.hpp>
#include <nodes/internal/Node.hpp>
#include <nodes/internal/NodeData.hpp>

using QtNodes::ConnectionStyle;
using QtNodes::FlowScene;
using QtNodes::Node;

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
    void updateColorScheme() override
    {
        // Setup icons according to the theme settings.
        addInboundBtn->setIcon(QICON_R("add"));
        addOutboundBtn->setIcon(QICON_R("add"));
        addRouteBtn->setIcon(QICON_R("add"));
        delBtn->setIcon(QICON_R("ashbin"));
    }
    QvMessageBusSlotDecl;

  private slots:
    void on_buttonBox_accepted();
    void on_insertDirectBtn_clicked();
    void on_addRouteBtn_clicked();
    void on_addDefaultBtn_clicked();
    void on_insertBlackBtn_clicked();
    void on_addInboundBtn_clicked();
    void on_addOutboundBtn_clicked();
    void on_delBtn_clicked();
    void on_editBtn_clicked();
    void on_domainStrategyCombo_currentIndexChanged(const QString &arg1);
    void on_defaultOutboundCombo_currentTextChanged(const QString &arg1);
    void on_importExistingBtn_clicked();
    void on_importGroupBtn_currentIndexChanged(int index);

    void on_addBalancerBtn_clicked();

    void on_addChainBtn_clicked();

  public slots:
    void onNodeClicked(Node &n);
    void onConnectionCreated(QtNodes::Connection const &c);
    void onConnectionDeleted(QtNodes::Connection const &c);

  private slots:
    void OnDispatcherInboundCreated(std::shared_ptr<INBOUND>);
    void OnDispatcherOutboundCreated(std::shared_ptr<OutboundObjectMeta>);
    void OnDispatcherRuleCreated(std::shared_ptr<RuleObject>);
    void OnDispatcherInboundOutboundHovered(const QString &tag, const ProtocolSettingsInfoObject &);

  private:
    std::shared_ptr<NodeDispatcher> nodeDispatcher;
    bool isLoading = false;
    void RenameItemTag(ROUTE_EDIT_MODE mode, const QString originalTag, QString *newTag);
    //
    QString domainStrategy;
    QString defaultOutbound;
    //
    CONFIGROOT root;
    CONFIGROOT original;
    //
    // ---------------------------- Node Graph Impl --------------------------
    void SetupNodeWidget();
    //
    FlowScene *nodeScene;
    // ---------------------------- Extra Source File Headers ----------------
    QString AddNewRule();
    //
    void ResolveDefaultOutboundTag(QString original, QString newTag);
};
