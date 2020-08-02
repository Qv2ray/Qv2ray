#pragma once

#include "base/Qv2rayBase.hpp"
#include "base/models/QvComplexConfigModels.hpp"
#include "common/QvHelpers.hpp"
#include "ui/common/QvDialog.hpp"
#include "ui/common/UIBase.hpp"
#include "ui/messaging/QvMessageBus.hpp"
#include "ui_w_RoutesEditor.h"
enum ROUTE_EDIT_MODE
{
    RENAME_INBOUND,
    RENAME_OUTBOUND,
    RENAME_RULE,
};

class NodeDispatcher;
class ChainEditorWidget;
class RoutingEditorWidget;

namespace QtNodes
{
    class Node;
};

namespace QtNodes
{
    class FlowView;
    class FlowScene;
} // namespace QtNodes

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
    void updateColorScheme() override;
    QvMessageBusSlotDecl;

  private slots:
    void on_addBalancerBtn_clicked();
    void on_addChainBtn_clicked();
    void on_addDefaultBtn_clicked();
    void on_addInboundBtn_clicked();
    void on_addOutboundBtn_clicked();
    void on_buttonBox_accepted();
    void on_debugPainterCB_clicked(bool checked);
    void on_defaultOutboundCombo_currentTextChanged(const QString &arg1);
    void on_domainStrategyCombo_currentIndexChanged(const QString &arg1);
    void on_importExistingBtn_clicked();
    void on_importGroupBtn_currentIndexChanged(int index);
    void on_insertBlackBtn_clicked();
    void on_insertDirectBtn_clicked();
    void on_linkExistingBtn_clicked();

  private slots:
    void OnDispatcherEditChainRequested(const QString &);
    void OnDispatcherOutboundDeleted(const complex::OutboundObjectMeta &);
    void OnDispatcherOutboundCreated(std::shared_ptr<complex::OutboundObjectMeta>, QtNodes::Node &);
    void OnDispatcherRuleCreated(std::shared_ptr<RuleObject>, QtNodes::Node &);
    void OnDispatcherInboundOutboundHovered(const QString &, const ProtocolSettingsInfoObject &);
    void OnDispatcherObjectTagChanged(ComplexTagNodeMode, const QString, const QString);

  private:
    QString defaultOutboundTag;
    std::shared_ptr<NodeDispatcher> nodeDispatcher;
    ChainEditorWidget *chainWidget;
    RoutingEditorWidget *ruleWidget;
    //
    bool isLoading = false;
    QString domainStrategy;
    //
    CONFIGROOT root;
    CONFIGROOT original;
};
