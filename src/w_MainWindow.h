#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "w_ConnectionEditWindow.h"
#include <QMainWindow>
#include <QMenu>
#include <QScrollBar>
#include <QSystemTrayIcon>

#include "QvUtils.h"
#include "ui_w_MainWindow.h"
#include "QvCoreInteractions.h"
#include "QvCoreConfigOperations.h"
#include "w_ConnectionEditWindow.h"
#include "w_ImportConfig.h"
#include "w_PrefrencesWindow.h"
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
        void reload_config();
        void UpdateLog();
    private slots:
        void on_restartButton_clicked();
        void on_actionEdit_triggered();
        void on_actionExisting_config_triggered();
        void showMenu(QPoint pos);
        void on_startButton_clicked();
        void on_stopButton_clicked();
        void on_clbutton_clicked();
        void on_activatedTray(QSystemTrayIcon::ActivationReason reason);
        void toggleMainWindowVisibility();
        void quit();
        void on_actionExit_triggered();
        void scrollToBottom();
        void on_actionPreferences_triggered();

        void on_pushButton_clicked();

        void on_connectionListWidget_currentRowChanged(int currentRow);

    private:
        void LoadConnections();
        QString CurrentConnection;
        Ui::MainWindow *ui;
        QSystemTrayIcon *hTray;
        QMenu *trayMenu = new QMenu(this);
        Qv2Instance *vinstance;
        //
        ConnectionEditWindow *connectionEditWindow;
        ImportConfigWindow *importConfigWindow;
        PrefrencesWindow *prefrenceWindow;
        void closeEvent(QCloseEvent *);
        QScrollBar *bar;
        QMap<QString, QJsonObject> connections;
};

#endif // MAINWINDOW_H
