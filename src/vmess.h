#ifndef VMESS_H
#define VMESS_H

#include <QDialog>

namespace Ui
{
    class vmess;
}

class vmess : public QDialog
{
    Q_OBJECT

public:
    explicit vmess(QWidget *parent = nullptr);
    ~vmess();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::vmess *ui;
};

#endif // VMESS_H
