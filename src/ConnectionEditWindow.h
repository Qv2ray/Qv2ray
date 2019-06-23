#ifndef CONFEDIT_H
#define CONFEDIT_H

#include <QDialog>

namespace Ui
{
    class ConnectionEditWindow;
}

class Hv2Config
{
public:
    QString host;
    QString port;
    QString alias;
    QString uuid;
    QString alterid;
    QString security;
    int isCustom;
    int save();
    void getConfigFromDialog(Ui::ConnectionEditWindow *ui);
private:

};

class ConnectionEditWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ConnectionEditWindow(QWidget *parent = nullptr);
    ~ConnectionEditWindow();
private:
    Ui::ConnectionEditWindow *ui;

};


#endif // CONFEDIT_H
