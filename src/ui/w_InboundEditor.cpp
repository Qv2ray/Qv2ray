#include "w_InboundEditor.h"
#include "ui_w_InboundEditor.h"

InboundEditor::InboundEditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InboundEditor)
{
    ui->setupUi(this);
}

InboundEditor::~InboundEditor()
{
    delete ui;
}
