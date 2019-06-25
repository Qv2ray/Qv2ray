#ifndef IMPORTCONF_H
#define IMPORTCONF_H

#include <QDialog>

namespace Ui
{
    class ImportConfig;
}

class ImportConfig : public QDialog
{
    Q_OBJECT

public:
    explicit ImportConfig(QWidget *parent = nullptr);
    void savefromFile(QString path, QString alias);
    ~ImportConfig();

private slots:
    void on_pushButton_clicked();
    void on_buttonBox_accepted();
signals:
    void updateConfTable();

private:
    Ui::ImportConfig *ui;
};

#endif // IMPORTCONF_H
