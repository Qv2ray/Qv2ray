#ifndef CONFEDIT_H
#define CONFEDIT_H

#include <QDialog>

namespace Ui
{
    class ConfEdit;
}

class vConfig
{
public:
    QString host;
    QString port;
    QString alias;
    QString uuid;
    QString alterid;
    QString security;
    int isCustom;
    vConfig *query(int id);
    void *save();
    void getConfigFromDialog(Ui::ConfEdit *ui);
    void getConfigFromCustom(QString path);

private:

};

class ConfEdit : public QDialog
{
    Q_OBJECT

public:
    explicit ConfEdit(QWidget *parent = nullptr);


    ~ConfEdit();

private slots:

    void on_ConfEdit_accepted();

private:
    Ui::ConfEdit *ui;
};


#endif // CONFEDIT_H
