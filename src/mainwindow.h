#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#define confDir "conf/"
#define confDatabase "conf/conf.db"
#include <QMainWindow>
#include "confedit.h"

class v2Instance;
namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ConfEdit *e;
    ~MainWindow();


private slots:
    void on_actionEdit_triggered();
    void on_actionExisting_config_triggered();
    void updateConfTable();
    void delConf();
    void showMenu(QPoint pos);
    void updateLog();
    void on_startButton_clicked();
    void on_stopButton_clicked();
    void on_restartButton_clicked();
    void select_triggered();
    void on_clbutton_clicked();
    void on_rtButton_clicked();
    void geneConf(int idIntable);
    void on_actionVmess_triggered();

private:
    Ui::MainWindow *ui;
    v2Instance *v2Inst;
};

#endif // MAINWINDOW_H
