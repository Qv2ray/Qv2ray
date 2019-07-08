#include "w_ConnectionEditWindow.h"
#include "w_MainWindow.h"
#include <QDebug>
#include <QFile>
#include <QIntValidator>
#include <iostream>


ConnectionEditWindow::ConnectionEditWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ConnectionEditWindow)
{
    ui->setupUi(this);
    ui->portLineEdit->setValidator(new QIntValidator());
    ui->alterLineEdit->setValidator(new QIntValidator());
}

ConnectionEditWindow::ConnectionEditWindow(QJsonObject editRootObject, QWidget *parent)
    : ConnectionEditWindow(parent)
{
    editRootJson = editRootObject;
    // TEST
    vmess = ConvertOutBoundJSONToStruct(editRootJson);
}


ConnectionEditWindow::~ConnectionEditWindow()
{
    delete ui;
}

void ConnectionEditWindow::on_buttonBox_accepted()
{
}
