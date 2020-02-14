#include "ConnectionWidget.hpp"
#include "QMessageBox"
#include "ui_ConnectionWidget.h"

Formwidget::Formwidget(int id, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Formwidget)
{
    _id = id;
    ui->setupUi(this);
}

Formwidget::~Formwidget()
{
    delete ui;
}

void Formwidget::on_pushButton_clicked()
{
    QMessageBox::information(this, "d", QString::number(_id));
}
