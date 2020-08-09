#pragma once

#include "common/HTTPRequestHelper.hpp"
#include "src/components/speedchart/speedwidget.hpp"
#include "ui/common/LogHighlighter.hpp"
#include "ui/messaging/QvMessageBus.hpp"
#include "ui_w_MainWindow.h"

#include <QMainWindow>

// ==========================================================================================
#include "ui/widgets/ConnectionInfoWidget.hpp"
#include "ui/widgets/ConnectionItemWidget.hpp"

enum MW_ITEM_COL
{
    MW_ITEM_COL_NAME = 1,
    MW_ITEM_COL_PING = 2,
    MW_ITEM_COL_IMPORTTIME = 3,
    MW_ITEM_COL_LASTCONNETED = 4,
    MW_ITEM_COL_DATA = 5
};

class MainWindow
    : public QMainWindow
    , Ui::MainWindow
{
    Q_OBJECT
  public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;
    void ProcessCommand(QString command, QStringList commands, QMap<QString, QString> args);

  signals:
    void StartConnection() const;
    void StopConnection() const;
    void RestartConnection() const;

  private:
    QvMessageBusSlotDecl;
  private slots:
    void on_activatedTray(QSystemTrayIcon::ActivationReason reason);
    void on_preferencesBtn_clicked();
    void on_clearlogButton_clicked();
    void on_connectionListWidget_customContextMenuRequested(const QPoint &pos);
    void on_importConfigButton_clicked();
    void on_subsButton_clicked();
    //
    void on_connectionListWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);
    void on_connectionFilterTxt_textEdited(const QString &arg1);
    void on_connectionListWidget_itemClicked(QTreeWidgetItem *item, int column);
    void on_locateBtn_clicked();
    //
    void on_chartVisibilityBtn_clicked();
    void on_logVisibilityBtn_clicked();
    void on_clearChartBtn_clicked();
    void on_connectionListWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    void on_masterLogBrowser_textChanged();
    //
    void on_pluginsBtn_clicked();
    void on_collapseGroupsBtn_clicked();
    void on_newConnectionBtn_clicked();
    void on_newComplexConnectionBtn_clicked();

  private:
    // Do not declare as slots, we connect them manually.

    void Action_Exit();
    void Action_Start();
    void Action_SetAutoConnection();
    void Action_Edit();
    void Action_EditJson();
    void Action_EditComplex();
    void Action_UpdateSubscription();
    void Action_TestLatency();
    void on_action_RCM_RealLatencyTest_triggered();
    void Action_RenameConnection();
    void Action_DeleteConnections();
    void Action_DuplicateConnection();
    void Action_ResetStats();
    void Action_CopyGraphAsImage();
    void Action_CopyRecentLogs();

    void OnConnectionWidgetFocusRequested(const ConnectionItemWidget *widget);

  private:
    void ToggleVisibility();
    void OnEditRequested(const ConnectionId &id);
    void OnEditJsonRequested(const ConnectionId &id);
    void OnConnected(const ConnectionGroupPair &id);
    void OnDisconnected(const ConnectionGroupPair &id);
    //
    void OnStatsAvailable(const ConnectionGroupPair &id, const QMap<StatisticsType, QvStatsSpeedData> &data);
    void OnVCoreLogAvailable(const ConnectionGroupPair &id, const QString &log);
    //
    void OnConnectionCreated(const ConnectionGroupPair &Id, const QString &displayName);
    void OnConnectionDeleted(const ConnectionGroupPair &Id);
    void OnConnectionLinkedWithGroup(const ConnectionGroupPair &id);
    //
    void OnGroupCreated(const GroupId &id, const QString &displayName);
    void OnGroupDeleted(const GroupId &id, const QList<ConnectionId> &connections);
    //
    void SortConnectionList(MW_ITEM_COL byCol, bool asending);
    //
    void ReloadRecentConnectionList();
    void OnRecentConnectionsMenuReadyToShow();
    //
    void OnLogScrollbarValueChanged(int value);

  protected:
    void timerEvent(QTimerEvent *event) override;
    void keyPressEvent(QKeyEvent *e) override;
    void keyReleaseEvent(QKeyEvent *e) override;
    void closeEvent(QCloseEvent *) override;

  private:
    QHash<GroupId, std::shared_ptr<QTreeWidgetItem>> groupNodes;
    QHash<ConnectionGroupPair, std::shared_ptr<QTreeWidgetItem>> connectionNodes;
    // Charts
    SpeedWidget *speedChartWidget;
    SyntaxHighlighter *vCoreLogHighlighter;
    ConnectionInfoWidget *infoWidget;
    void UpdateActionTranslations()
    {
        tray_SystemProxyMenu->setTitle(tr("System Proxy"));
        tray_RecentConnectionsMenu->setTitle(tr("Recent Connections"));
        tray_ClearRecentConnectionsAction->setText(tr("Clear Recent Connections"));
        //
        tray_action_ToggleVisibility->setText(tr("Hide"));
        tray_action_Preferences->setText(tr("Preferences"));
        tray_action_Quit->setText(tr("Quit"));
        tray_action_Start->setText(tr("Connect"));
        tray_action_Restart->setText(tr("Reconnect"));
        tray_action_Stop->setText(tr("Disconnect"));
        tray_action_SetSystemProxy->setText(tr("Enable System Proxy"));
        tray_action_ClearSystemProxy->setText(tr("Disable System Proxy"));
        //
        action_RCM_Start->setText(tr("Connect to this"));
        action_RCM_SetAutoConnection->setText(tr("Set as automatically connected"));
        action_RCM_EditJson->setText(tr("Edit as JSON"));
        action_RCM_UpdateSubscription->setText(tr("Update Subscription"));
        action_RCM_EditComplex->setText(tr("Edit as Complex Config"));
        action_RCM_RenameConnection->setText(tr("Rename"));
        action_RCM_Edit->setText(tr("Edit"));
        action_RCM_DuplicateConnection->setText(tr("Duplicate to the Same Group"));
        action_RCM_TestLatency->setText(tr("Test Latency"));
        action_RCM_ResetStats->setText(tr("Clear Usage Data"));
        action_RCM_DeleteConnection->setText(tr("Delete Connection"));
        sortMenu->setTitle(tr("Sort connection list."));
        sortAction_SortByName_Asc->setText(tr("By connection name, A-Z"));
        sortAction_SortByName_Dsc->setText(tr("By connection name, Z-A"));
        sortAction_SortByPing_Asc->setText(tr("By latency, Ascending"));
        sortAction_SortByPing_Dsc->setText(tr("By latency, Descending"));
        sortAction_SortByData_Asc->setText(tr("By data, Ascending"));
        sortAction_SortByData_Dsc->setText(tr("By data, Descending"));
        //
        action_RCM_SwitchCoreLog->setText(tr("Switch to Core log"));
        action_RCM_SwitchQv2rayLog->setText(tr("Switch to Qv2ray log"));
        //
        graph_action_CopyAsImage->setText(tr("Copy graph as image."));
        action_RCM_CopyRecentLogs->setText(tr("Copy latest logs."));
    }

    //
    // Declare Actions
#define DECL_ACTION(parent, name) QAction *name = new QAction(parent)
    QMenu *tray_RootMenu = new QMenu(this);
    QMenu *tray_SystemProxyMenu = new QMenu(this);
    QMenu *tray_RecentConnectionsMenu = new QMenu(this);
    QMenu *sortMenu = new QMenu(this);
    QMenu *logRCM_Menu = new QMenu(this);
    QMenu *connectionListRCM_Menu = new QMenu(this);

    QMenu *graphWidgetMenu = new QMenu(this);
    // Do not set parent=tray_RecentConnectionsMenu
    // Calling clear() will cause this QAction being deleted.
    DECL_ACTION(this, tray_ClearRecentConnectionsAction);
    DECL_ACTION(tray_RootMenu, tray_action_ToggleVisibility);
    DECL_ACTION(tray_RootMenu, tray_action_Preferences);
    DECL_ACTION(tray_RootMenu, tray_action_Quit);
    DECL_ACTION(tray_RootMenu, tray_action_Start);
    DECL_ACTION(tray_RootMenu, tray_action_Restart);
    DECL_ACTION(tray_RootMenu, tray_action_Stop);
    DECL_ACTION(tray_RootMenu, tray_action_SetSystemProxy);
    DECL_ACTION(tray_RootMenu, tray_action_ClearSystemProxy);
    DECL_ACTION(graphWidgetMenu, graph_action_CopyAsImage);
    DECL_ACTION(connectionListRCM_Menu, action_RCM_Start);
    DECL_ACTION(connectionListRCM_Menu, action_RCM_SetAutoConnection);
    DECL_ACTION(connectionListRCM_Menu, action_RCM_UpdateSubscription);
    DECL_ACTION(connectionListRCM_Menu, action_RCM_Edit);
    DECL_ACTION(connectionListRCM_Menu, action_RCM_EditJson);
    DECL_ACTION(connectionListRCM_Menu, action_RCM_EditComplex);
    DECL_ACTION(connectionListRCM_Menu, action_RCM_RenameConnection);
    DECL_ACTION(connectionListRCM_Menu, action_RCM_DuplicateConnection);
    DECL_ACTION(connectionListRCM_Menu, action_RCM_TestLatency);
    QAction *action_RCM_RealLatencyTest = new QAction(tr("Test Real Latency"), this);
    DECL_ACTION(connectionListRCM_Menu, action_RCM_ResetStats);
    DECL_ACTION(connectionListRCM_Menu, action_RCM_DeleteConnection);
    DECL_ACTION(sortMenu, sortAction_SortByName_Asc);
    DECL_ACTION(sortMenu, sortAction_SortByName_Dsc);
    DECL_ACTION(sortMenu, sortAction_SortByPing_Asc);
    DECL_ACTION(sortMenu, sortAction_SortByPing_Dsc);
    DECL_ACTION(sortMenu, sortAction_SortByData_Asc);
    DECL_ACTION(sortMenu, sortAction_SortByData_Dsc);
    DECL_ACTION(logRCM_Menu, action_RCM_SwitchCoreLog);
    DECL_ACTION(logRCM_Menu, action_RCM_SwitchQv2rayLog);
    DECL_ACTION(logRCM_Menu, action_RCM_CopyRecentLogs);
#undef DECL_ACTION

    //
    QTextDocument *vCoreLogDocument = new QTextDocument(this);
    QTextDocument *qvLogDocument = new QTextDocument(this);
    //
    int qvLogTimerId = -1;
    bool qvLogAutoScoll = true;
    //
    ConnectionGroupPair lastConnectedIdentifier;
    void MWSetSystemProxy();
    void MWClearSystemProxy();
    void CheckSubscriptionsUpdate();
    bool StartAutoConnectionEntry();
    //
    void updateColorScheme();
    //
    void MWAddConnectionItem_p(const ConnectionGroupPair &id);
    void MWAddGroupItem_p(const GroupId &groupId);
};
