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
    public slots:
        void save_reload_globalconfig(bool need_restart);
        void UpdateLog();
    private slots:
        void VersionUpdate(QByteArray &data);
        void on_startButton_clicked();
        void on_stopButton_clicked();
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

        void on_pushButton_clicked();

        void on_reconnectButton_clicked();

    private:
        void on_action_StartThis_triggered();
        void on_action_RenameConnection_triggered();
        Ui::MainWindow *ui;
        QvHttpRequestHelper HTTPRequestHelper;
        QSystemTrayIcon *hTray;
        QMenu *trayMenu = new QMenu(this);
        Qv2Instance *vinstance;
        QMenu listMenu;
        QMap<QString, QJsonObject> connections;
        QString CurrentConnectionName;
        //
        QString originalName;
        bool isRenamingInProgress;
        //
        void ShowAndSetConnection(QString currentText, bool SetConnection, bool Apply);
        void LoadConnections();
        void closeEvent(QCloseEvent *);
};

#endif // MAINWINDOW_H
