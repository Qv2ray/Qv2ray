#include "w_RouteEditor.h"
#include "ui_w_RouteEditor.h"

RouteEditor::RouteEditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RouteEditor)
{
    ui->setupUi(this);
}

RouteEditor::~RouteEditor()
{
    delete ui;
}
