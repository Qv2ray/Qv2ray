#include "formwidget.h"
#include "QMessageBox"
#include "ui_formwidget.h"

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
