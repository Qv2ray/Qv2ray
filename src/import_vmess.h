#ifndef VMESS_H
#define VMESS_H

#include <QDialog>

namespace Ui
{
    class import_vmess;
}

class import_vmess : public QDialog
{
    Q_OBJECT

public:
    explicit import_vmess(QWidget *parent = nullptr);
    ~import_vmess();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::import_vmess *ui;
};

#endif // VMESS_H
