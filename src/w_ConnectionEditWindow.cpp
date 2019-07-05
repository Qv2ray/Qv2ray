#include "w_ConnectionEditWindow.h"
#include "w_MainWindow.h"
#include <QDebug>
#include <QFile>
#include <QIntValidator>
#include <iostream>

namespace Ui
{
    ConnectionEditWindow::ConnectionEditWindow(QWidget *parent)
        : QDialog(parent)
        , ui(new Ui_WConnectionEdit)
    {
        ui->setupUi(this);
        ui->portLineEdit->setValidator(new QIntValidator());
        ui->alterLineEdit->setValidator(new QIntValidator());
    }

    ConnectionEditWindow::~ConnectionEditWindow()
    {
        delete ui;
    }

    //void ConnectionEditWindow::getConfigFromDialog(Ui::ConnectionEditWindow *ui)
    //{
    //this->host = ui->ipLineEdit->text();
    //this->port = ui->portLineEdit->text();
    //this->alias = ui->aliasLineEdit->text();
    //this->uuid = ui->idLineEdit->text();
    //this->alterid = ui->alterLineEdit->text();
    //this->security = ui->securityCombo->currentText();
    //this->isCustom = 0;
    //}
}
