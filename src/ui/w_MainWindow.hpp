#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "QvTCPing.hpp"

#include <QMainWindow>
#include <QMenu>
#include <QScrollBar>
#include <QtCharts>
#include <QSystemTrayIcon>

#include "ui_w_MainWindow.h"

#include "QvUtils.hpp"
#include "QvCoreInteractions.hpp"
#include "QvCoreConfigOperations.hpp"
#include "QvHTTPRequestHelper.hpp"
#include "QvPACHandler.hpp"
#include "QvLogHighlighter.hpp"
enum TREENODEOBJECT_TYPE {
    CON_REGULAR = 1,
    CON_SUBSCRIPTION = 2
};
//
struct ConnectionObject {
    TREENODEOBJECT_TYPE configType;
    QString subscriptionName;
    QString connectionName;
    double latency;
    CONFIGROOT config;
};

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
        void onPingFinished(QvTCPingData data);
        void UpdateVCoreLog(const QString &log);
        void OnConfigListChanged(bool need_restart);
    private slots:
        void setMasterLogHBar();
        void on_action_RCM_ShareQR_triggered();
        void on_startButton_clicked();
        void on_stopButton_clicked();
        void on_reconnectButton_clicked();
        void VersionUpdate(QByteArray &data);
        void on_activatedTray(QSystemTrayIcon::ActivationReason reason);
        void ToggleVisibility();
        void quit();
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

    public:
        static MainWindow *mwInstance;
        CONFIGROOT CurrentFullConfig;
        QString CurrentConnectionName = "";
        ConnectionInstance *vinstance;

    protected:
        void mouseReleaseEvent(QMouseEvent *e) override;
        void keyPressEvent(QKeyEvent *e) override;
        void timerEvent(QTimerEvent *event) override;
        void closeEvent(QCloseEvent *) override;

    private slots:
        void on_action_StartThis_triggered();
        void on_action_RCM_EditJson_triggered();
        void on_action_RCM_ConvToComplex_triggered();
        void on_action_RCM_RenameConnection_triggered();
        void on_connectionListWidget_itemSelectionChanged();

    private:
        //
        void SetEditWidgetEnable(bool enabled);
        // Charts
        QChartView *speedChartView;
        QChart *speedChartObj;
        QSplineSeries *uploadSerie;
        QSplineSeries *downloadSerie;
        QList<double> uploadList;
        QList<double> downloadList;
        //
        //
        QMenu *trayMenu = new QMenu(this);
        QMenu *listMenu;

        /// Key --> ListWidget.item.text
        QMap<QString, ConnectionObject> connections;
        //
        QString originalName;
        bool isRenamingInProgress;
        //
        int logTimerId;
        int speedTimerId;
        int pingTimerId;
        //
        void ShowAndSetConnection(QString currentText, bool SetConnection, bool Apply);
        void ReloadConnections();
        //
        //
        QvHttpRequestHelper *requestHelper;
        QSystemTrayIcon *hTray;
        PACServer *pacServer;
        QvTCPingModel *tcpingModel;
        SyntaxHighlighter *vCoreLogHighlighter;
        SyntaxHighlighter *qvAppLogHighlighter;
        //
        Qv2rayConfig currentConfig;

        QList<QTextBrowser *> logTextBrowsers;
        int currentLogBrowserId = 0;
        QString currentGUIShownConnectionName;
};

#endif // MAINWINDOW_H
