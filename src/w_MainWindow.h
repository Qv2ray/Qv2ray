#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "w_ConnectionEditWindow.h"
#include <QMainWindow>
#include <QMenu>
#include <QScrollBar>
#include <QSystemTrayIcon>

#include "ui_w_MainWindow.h"
#include "V2ConfigObjects.hpp"
#include "vinteract.hpp"
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
        void reload_config();
    private slots:
        void on_restartButton_clicked();
        void on_actionEdit_triggered();
        void on_actionExisting_config_triggered();
        void showMenu(QPoint pos);
        void UpdateLog();
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

    private:
        Ui::MainWindow *ui;
        QSystemTrayIcon *hTray;
        QMenu *trayMenu = new QMenu(this);
        v2Instance *vinstance;
        ConnectionEditWindow *connectionEditWindow;
        ImportConfigWindow *importConfigWindow;
        PrefrencesWindow *prefrenceWindow;
        void closeEvent(QCloseEvent *);
        QScrollBar *bar;
};

#endif // MAINWINDOW_H
