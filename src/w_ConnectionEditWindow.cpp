#include "w_ConnectionEditWindow.h"
#include "ui_ConnectionEditWindow.h"
#include <iostream>
#include "w_MainWindow.h"
#include <QDebug>
#include <QFile>
#include <QIntValidator>

ConnectionEditWindow::ConnectionEditWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectionEditWindow)
{
    ui->setupUi(this);
    ui->portLineEdit->setValidator(new QIntValidator());
    ui->alterLineEdit->setValidator(new QIntValidator());
}

ConnectionEditWindow::~ConnectionEditWindow()
{
    delete ui;
}
int Hv2Config::save()
{
    return -1;
}

void Hv2Config::getConfigFromDialog(Ui::ConnectionEditWindow *ui)
{
    this->host = ui->ipLineEdit->text();
    this->port = ui->portLineEdit->text();
    this->alias = ui->aliasLineEdit->text();
    this->uuid = ui->idLineEdit->text();
    this->alterid = ui->alterLineEdit->text();
    this->security = ui->securityCombo->currentText();
    this->isCustom = 0;
}
