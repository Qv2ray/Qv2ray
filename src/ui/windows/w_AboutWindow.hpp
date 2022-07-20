#pragma once

#include <QDialog>

namespace Ui {
    class w_AboutWindow;
}

class AboutWindow : public QDialog
{
    Q_OBJECT

  public:
    explicit AboutWindow(QWidget *parent = nullptr);
    ~AboutWindow();

  private slots:
    void on_openConfigDirCB_clicked();
    void on_aboutQt_clicked();

  private:
    Ui::w_AboutWindow *ui;
};
