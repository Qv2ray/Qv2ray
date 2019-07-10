#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QMenu>
#include <QScrollBar>
#include <QSystemTrayIcon>

#include "QvUtils.h"
#include "ui_w_MainWindow.h"
#include "QvCoreInteractions.h"
#include "QvCoreConfigOperations.h"
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
        void on_startButton_clicked();
        void on_stopButton_clicked();
        void on_clbutton_clicked();
        void on_activatedTray(QSystemTrayIcon::ActivationReason reason);
        void ToggleVisibility();
        void quit();
        void on_actionExit_triggered();
        void QTextScrollToBottom();

        void on_connectionListWidget_itemClicked(QListWidgetItem *item);

        void on_importConfigBtn_clicked();

        void on_addConfigBtn_clicked();

        void on_delConfigBtn_clicked();

        void on_prefrencesBtn_clicked();

        void on_connectionListWidget_doubleClicked(const QModelIndex &index);

        void on_editConnectionSettingsBtn_clicked();

        void on_clearlogButton_clicked();

    private:
        void ShowAndSetConnection(int index, bool SetConnection, bool Apply);
        void LoadConnections();
        QString CurrentConnectionName;
        Ui::MainWindow *ui;
        QSystemTrayIcon *hTray;
        QMenu *trayMenu = new QMenu(this);
        Qv2Instance *vinstance;
        //
        void closeEvent(QCloseEvent *);
        QMap<QString, QJsonObject> connections;
};

#endif // MAINWINDOW_H
