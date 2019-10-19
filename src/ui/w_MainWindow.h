#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QMenu>
#include <QScrollBar>
#include <QSystemTrayIcon>

#include "QvUtils.h"
#include "QvCoreInteractions.h"
#include "QvCoreConfigOperations.h"
#include "QvHTTPRequestHelper.h"

#include "ui_w_MainWindow.h"

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
        Q_OBJECT
    public:
        explicit MainWindow(QWidget *parent = nullptr);
        ~MainWindow();
    signals:
        void Connect();
        void DisConnect();
        void ReConnect();
    public slots:
        void UpdateLog();
        void OnConfigListChanged(bool need_restart);
    private slots:
        void on_startButton_clicked();
        void on_stopButton_clicked();
        void on_reconnectButton_clicked();
        void VersionUpdate(QByteArray &data);
        void on_activatedTray(QSystemTrayIcon::ActivationReason reason);
        void ToggleVisibility();
        void quit();
        void on_actionExit_triggered();
        void QTextScrollToBottom();

        void on_connectionListWidget_itemClicked(QListWidgetItem *item);

        void on_prefrencesBtn_clicked();

        void on_connectionListWidget_doubleClicked(const QModelIndex &index);

        void on_clearlogButton_clicked();

        void on_connectionListWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

        void on_connectionListWidget_customContextMenuRequested(const QPoint &pos);

        void on_connectionListWidget_itemChanged(QListWidgetItem *item);

        void on_removeConfigButton_clicked();

        void on_importConfigButton_clicked();

        void on_addConfigButton_clicked();

        void on_editConfigButton_clicked();

        void on_editJsonBtn_clicked();

        void on_pingTestBtn_clicked();

        void on_shareQRButton_clicked();

        void on_shareVMessButton_clicked();
    public:
        QJsonObject CurrentFullConfig;
        QString CurrentConnectionName = "";
        Qv2Instance *vinstance;
        QString totalDataUp;
        QString totalDataDown;
        QString totalSpeedUp;
        QString totalSpeedDown;

    protected:

        void timerEvent(QTimerEvent *event);
    private:
        void on_action_StartThis_triggered();
        void on_action_RCM_EditJson_triggered();
        void on_action_RenameConnection_triggered();
        Ui::MainWindow *ui;
        QvHttpRequestHelper HTTPRequestHelper;
        QSystemTrayIcon *hTray;
        QMenu *trayMenu = new QMenu(this);
        QMenu listMenu;
        QMap<QString, QJsonObject> connections;
        //
        QString originalName;
        bool isRenamingInProgress;
        //
        int speedTimerId;
        //
        void ShowAndSetConnection(QString currentText, bool SetConnection, bool Apply);
        void LoadConnections();
        void closeEvent(QCloseEvent *);
};

#endif // MAINWINDOW_H
