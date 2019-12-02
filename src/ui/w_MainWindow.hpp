#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QMenu>
#include <QScrollBar>
#include <QtCharts>
#include <QSystemTrayIcon>

#include "QvUtils.hpp"
#include "QvCoreInteractions.hpp"
#include "QvCoreConfigOperations.hpp"
#include "QvHTTPRequestHelper.hpp"
#include "QvPACHandler.hpp"
#include "QvLogHighlighter.hpp"

#include "ui_w_MainWindow.h"

struct ConnectionObject {
    bool isRegularConnection;
    QString subscriptionName;
    QString connectionName;
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
        void UpdateVCoreLog(const QString &log);
        void OnConfigListChanged(bool need_restart);
    private slots:
        void setMasterLogHBar();
        void on_action_RCM_ShareQR_triggered(bool checked = false);
        void on_startButton_clicked();
        void on_stopButton_clicked();
        void on_reconnectButton_clicked();
        void VersionUpdate(QByteArray &data);
        void on_activatedTray(QSystemTrayIcon::ActivationReason reason);
        void ToggleVisibility();
        void quit();
        void on_actionExit_triggered();

        void on_connectionListWidget_itemClicked(QListWidgetItem *item);

        void on_prefrencesBtn_clicked();

        void on_connectionListWidget_doubleClicked(const QModelIndex &index);

        void on_clearlogButton_clicked();

        void on_connectionListWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

        void on_connectionListWidget_customContextMenuRequested(const QPoint &pos);

        void on_connectionListWidget_itemChanged(QListWidgetItem *item);

        void on_removeConfigButton_clicked();

        void on_importConfigButton_clicked();

        void on_editConfigButton_clicked();

        void on_editJsonBtn_clicked();

        void on_pingTestBtn_clicked();

        void on_shareBtn_clicked();

        void on_duplicateBtn_clicked();

        void on_masterLogBrowser_textChanged();

    public:
        CONFIGROOT CurrentFullConfig;
        QString CurrentConnectionName = "";
        ConnectionInstance *vinstance;

    protected:

        void mouseReleaseEvent(QMouseEvent *e) override;
        void keyPressEvent(QKeyEvent *e) override;
        void timerEvent(QTimerEvent *event) override;
        void closeEvent(QCloseEvent *) override;
    private:
        // Charts
        QChartView *speedChartView;
        QChart *speedChartObj;
        QSplineSeries *uploadSerie;
        QSplineSeries *downloadSerie;
        QList<double> uploadList;
        QList<double> downloadList;
        //
        void on_action_StartThis_triggered();
        void on_action_RCM_EditJson_triggered();
        void on_action_RCM_ConvToComplex_triggered();
        void on_action_RenameConnection_triggered();
        //
        QvHttpRequestHelper HTTPRequestHelper;
        QSystemTrayIcon *hTray;
        //
        QMenu *trayMenu = new QMenu(this);
        QMenu *listMenu;
        QMap<QString, ConnectionObject> connections;
        //
        QString originalName;
        bool isRenamingInProgress;
        //
        int logTimerId;
        int speedTimerId;
        //
        void ShowAndSetConnection(QString currentText, bool SetConnection, bool Apply);
        void LoadConnections();
        //
        PACHandler *pacServer;
        Highlighter *vCoreLogHighlighter;
        Highlighter *qvAppLogHighlighter;

        QList<QTextBrowser *> logTextBrowsers;
        int currentLogBrowserId = 0;
};

static const MainWindow *mwInstance;

#endif // MAINWINDOW_H
