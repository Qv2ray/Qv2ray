#ifndef IMPORTCONF_H
#define IMPORTCONF_H

#include <QDialog>

namespace Ui
{
    class importConf;
}

class importConf : public QDialog
{
    Q_OBJECT

public:
    explicit importConf(QWidget *parent = nullptr);
    void savefromFile(QString path, QString alias);
    ~importConf();

private slots:
    void on_pushButton_clicked();
    void on_buttonBox_accepted();

private:
    Ui::importConf *ui;
};

#endif // IMPORTCONF_H
