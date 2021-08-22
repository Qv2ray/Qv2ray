#pragma once
#include "ConnectionModelHelper/ConnectionModelHelper.hpp"
#include "QvPlugin/Gui/QvGUIPluginInterface.hpp"
#include "LogHighlighter/LogHighlighter.hpp"
#include "MessageBus/MessageBus.hpp"
#include "SpeedWidget/SpeedWidget.hpp"
#include "ui/WidgetUIBase.hpp"
#include "ui/widgets/ConnectionInfoWidget.hpp"
#include "ui/widgets/ConnectionItemWidget.hpp"
#include "ui_w_MainWindow.h"

#include <QMainWindow>
#include <QMenu>
#include <QSystemTrayIcon>

namespace Qv2rayPlugin
{
    class PluginMainWindowWidget;
}

class MainWindow
    : public QMainWindow
    , Ui::MainWindow
{
    Q_OBJECT
  public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;
    void ProcessCommand(const QString &command, QStringList commands, const QMap<QString, QString> &args);

  private:
    QvMessageBusSlotDecl;
    static constexpr auto BUTTON_PROP_PLUGIN_MAINWIDGETITEM_INDEX = "plugin_list_index";

  public slots:
    void OnTrayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void MWToggleVisibility();
    void MWShowWindow();
    void MWHideWindow();

  private slots:
    void on_preferencesBtn_clicked();
    void on_clearlogButton_clicked();
    void on_connectionTreeView_customContextMenuRequested(QPoint pos);
    void on_importConfigButton_clicked();
    void on_subsButton_clicked();
    //
    void on_connectionFilterTxt_textEdited(const QString &arg1);
    void on_locateBtn_clicked();
    //
    void on_chartVisibilityBtn_clicked();
    void on_logVisibilityBtn_clicked();
    void on_clearChartBtn_clicked();
    void on_logBrowser_textChanged();
    //
    void on_pluginsBtn_clicked();
    void on_collapseGroupsBtn_clicked();
    void on_newConnectionBtn_clicked();
    void on_newComplexConnectionBtn_clicked();
    //
    void on_connectionTreeView_doubleClicked(const QModelIndex &index);
    void on_connectionTreeView_clicked(const QModelIndex &index);

    void on_aboutBtn_clicked();

  private:
    // Do not declare as slots, we connect them manually.
    void Action_Exit();
    void Action_Start();
    void Action_SetAutoConnection();
    void Action_Edit();
    void Action_EditJson();
    void Action_Copy_Link();
    void Action_UpdateSubscription();
    void Action_TestLatency();
    void Action_RenameConnection();
    void Action_DeleteConnections();
    void Action_DuplicateConnection();
    void Action_ResetStats();
    void Action_CopyGraphAsImage();
    void Action_CopyRecentLogs();

  private:
    void OnEditRequested(const ConnectionId &id);
    void OnEditJsonRequested(const ConnectionId &id);
    void OnConnected(const ProfileId &id);
    void OnDisconnected(const ProfileId &id);
    //
    void OnStatsAvailable(const ProfileId &id, const StatisticsObject &speed);
    void OnKernelLogAvailable(const ProfileId &id, const QString &log);
    //
    void RetranslateMenuActions();
    void OnPluginButtonClicked();

  protected:
    void keyPressEvent(QKeyEvent *e) override;
    void keyReleaseEvent(QKeyEvent *e) override;
    void closeEvent(QCloseEvent *) override;
    void changeEvent(QEvent *e) override;

  private:
    void LoadPluginMainWindowWidgets();
    void CheckForSubscriptionsUpdate();
    bool TryStartAutoConnectionEntry();
    void updateColorScheme();

  private:
    SpeedWidget *speedChartWidget;
    LogHighlighter::LogHighlighter *coreLogHighlighter;
    ConnectionInfoWidget *connectionInfoWidget;

    QMenu *connMenu = new QMenu(this);
    struct
    {
        QAction *Start;
        QAction *SetAutoConnection;
        QAction *UpdateSubscription;

        QAction *Edit;
        QMenu *EditAsMenu;
        struct
        {
            QAction *Json;
        } editAsActions;

        QMenu *CopyMenu;
        struct
        {
            QAction *Link;
        } copyActions;

        QAction *RenameConnection;
        QAction *DuplicateConnection;
        QAction *TestLatency;
        QAction *ResetStats;
        QAction *DeleteConnection;
    } connActions;

    QMenu *sortMenu = new QMenu(this);
    struct
    {

        QAction *SortByName_Asc;
        QAction *SortByName_Dsc;
        QAction *SortByPing_Asc;
        QAction *SortByPing_Dsc;
        QAction *SortByData_Asc;
        QAction *SortByData_Dsc;
    } sortActions;

    QAction *graphAction_CopyGraph;
    QAction *logAction_CopySelected;
    QAction *logAction_CopyRecentLogs;

    bool logAutoScoll = true;

    QList<Qv2rayPlugin::Gui::PluginMainWindowWidget *> pluginWidgets;
    Qv2ray::ui::widgets::models::ConnectionListHelper *connectionModelHelper;
};
