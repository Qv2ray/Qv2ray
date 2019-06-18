#include "confedit.h"
#include "ui_confedit.h"
#include <iostream>
#include "mainwindow.h"
#include <QDebug>
#include <QFile>
#include "db.h"
#include <QIntValidator>

ConfEdit::ConfEdit(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfEdit)
{
    ui->setupUi(this);
    ui->portLineEdit->setValidator(new QIntValidator());
    ui->alterLineEdit->setValidator(new QIntValidator());
}

ConfEdit::~ConfEdit()
{
    delete ui;
}

int vConfig::save()
{
    return -1;
}

void vConfig::getConfigFromDialog(Ui::ConfEdit *ui)
{
    this->host = ui->ipLineEdit->text();
    this->port = ui->portLineEdit->text();
    this->alias = ui->aliasLineEdit->text();
    this->uuid = ui->idLineEdit->text();
    this->alterid = ui->alterLineEdit->text();
    this->security = ui->securityCombo->currentText();
    this->isCustom = 0;
}
