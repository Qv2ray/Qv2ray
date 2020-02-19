#pragma once

#include <QMainWindow>
#include <QMenu>
#include <QScrollBar>
#include <QSystemTrayIcon>

#include "ui_w_MainWindow.h"

#include "common/LogHighlighter.hpp"
#include "common/HTTPRequestHelper.hpp"
#include "components/speedchart/speedwidget.hpp"
#include "core/handler/ConnectionHandler.hpp"
#include "ui/messaging/QvMessageBus.hpp"

// ==========================================================================================
#include "ui/widgets/ConnectionWidget.hpp"
#include "ui/widgets/ConnectionInfoWidget.hpp"

class MainWindow : public QMainWindow, Ui::MainWindow
{
        Q_OBJECT
    public:
        explicit MainWindow(QWidget *parent = nullptr);
        ~MainWindow() override;
    signals:
        void Connect() const;
        void DisConnect() const;
        void ReConnect() const;
    public slots:
        QvMessageBusSlotHeader
        //void onPingFinished(QvTCPingData data);
        void UpdateVCoreLog(const QString &log);
        void OnConfigListChanged(bool need_restart);
    private slots:
        void on_action_RCM_ShareQR_triggered();
        void on_startButton_clicked();
        void on_stopButton_clicked();
        void on_reconnectButton_clicked();
        void on_activatedTray(QSystemTrayIcon::ActivationReason reason);
        void on_actionExit_triggered();
        void on_preferencesBtn_clicked();
        void on_connectionListWidget_doubleClicked(const QModelIndex &index);
        void on_clearlogButton_clicked();
        void on_connectionListWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
        void on_connectionListWidget_customContextMenuRequested(const QPoint &pos);
        void on_connectionListWidget_itemChanged(QTreeWidgetItem *item, int column);
        void on_removeConfigButton_clicked();
        void on_importConfigButton_clicked();
        void on_editConfigButton_clicked();
        void on_editJsonBtn_clicked();
        void on_pingTestBtn_clicked();
        void on_shareBtn_clicked();
        void on_duplicateBtn_clicked();
        void on_subsButton_clicked();
        //
        void ToggleVisibility();
        void VersionUpdate(QByteArray &data);
        //
        void OnConnected(const ConnectionId &id);

    public:
        static MainWindow *mwInstance;

    protected:
        void mouseReleaseEvent(QMouseEvent *e) override;
        void keyPressEvent(QKeyEvent *e) override;
        void timerEvent(QTimerEvent *event) override;
        void closeEvent(QCloseEvent *) override;

    private slots:
        //
        void onConnectionWidgetFocusRequested(const ConnectionWidget *widget);
        //
        void on_action_StartThis_triggered();
        void on_action_RCM_EditJson_triggered();
        void on_action_RCM_ConvToComplex_triggered();
        void on_action_RCM_RenameConnection_triggered();
        void on_connectionListWidget_itemSelectionChanged();
        void on_connectionListWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);

        void on_connectionFilterTxt_textEdited(const QString &arg1);

        void on_connectionListWidget_itemClicked(QTreeWidgetItem *item, int column);

    private:
        QTreeWidgetItem *CurrentItem;
        //
        void SetEditWidgetEnable(bool enabled);
        //CONFIGROOT currentFullConfig;
        //
        // Charts
        SpeedWidget *speedChartView;
        //
        QMenu *connectionListMenu;
        //
        //QvConnectionObject renameOriginalIdentifier;
        bool isRenamingInProgress;
        //
        QvHttpRequestHelper *requestHelper;
        QSystemTrayIcon hTray;
        //PACServer pacServer;
        //QvTCPingModel tcpingHelper;
        SyntaxHighlighter *vCoreLogHighlighter;
        ConnectionInfoWidget *infoWidget;
        //
        // Actions in the system tray menu
        //
        QMenu *tray_RootMenu = new QMenu(this);
        QAction *action_Tray_ShowHide;
        QAction *action_Tray_ShowPreferencesWindow;
        QAction *action_Tray_Quit;
        // --> Connectivities
        QAction *action_Tray_Start;
        QAction *action_Tray_Reconnect ;
        QAction *action_Tray_Stop;
        // --> System proxy settings
        QMenu *tray_SystemProxyMenu = new QMenu(this);
        QAction *action_Tray_SetSystemProxy;
        QAction *action_Tray_ClearSystemProxy;
        //
        // ----------------------------------- Extra Headers For w_MainWindow_extra.cpp Handling V2ray Connectivities.
        bool systemProxyEnabled;
        void MWFindAndStartAutoConfig();
        void MWStopConnection();
        void MWSetSystemProxy();
        void MWClearSystemProxy(bool);
        void CheckSubscriptionsUpdate();
};
