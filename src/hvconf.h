#ifndef HVCONF_H
#define HVCONF_H

#include <QDialog>
#include <QJsonObject>

namespace Ui
{
    class hvConf;
}

class hvConf : public QDialog
{
    Q_OBJECT

public:
    explicit hvConf(QWidget *parent = nullptr);
    ~hvConf();
    QJsonObject getInbound();
    QJsonObject *rootObj;

private slots:
    void on_buttonBox_accepted();

private:
    Ui::hvConf *ui;
};

#endif // HVCONF_H
