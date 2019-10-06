#include "w_JsonEditor.h"
#include "ui_w_JsonEditor.h"

#include "QvUtils.h"

JsonEditor::JsonEditor(QJsonObject rootObject, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::JsonEditor)
{
    ui->setupUi(this);
    original = rootObject;
    result = rootObject;
    QString jsonString  = JsonToString(rootObject);

    if (VerifyJsonString(&jsonString).isEmpty()) {
        LOG(MODULE_UI, "Begin loading Json Model")
        ui->jsonTree->setModel(&model);
        model.loadJson(QJsonDocument(rootObject).toJson());
    } else {
        QvMessageBox(this, tr("Json Contains Syntax Errors"), tr("Original Json may contain syntax errors. Json tree is disabled."));
    }

    ui->jsonEditor->setText(JsonToString(rootObject));
    ui->jsonTree->expandAll();
    ui->jsonTree->resizeColumnToContents(0);
}

QJsonObject JsonEditor::OpenEditor()
{
    int resultCode = this->exec();
    auto string = ui->jsonEditor->toPlainText();

    while (resultCode == QDialog::Accepted && !VerifyJsonString(&string).isEmpty()) {
        QvMessageBox(this, tr("Json Contains Syntax Errors"), tr("You must correct these errors before continue."));
        resultCode = this->exec();
        string = ui->jsonEditor->toPlainText();
    }

    return resultCode == QDialog::Accepted ? result : original;
}

JsonEditor::~JsonEditor()
{
    delete ui;
}

void JsonEditor::on_jsonEditor_textChanged()
{
    auto string = ui->jsonEditor->toPlainText();
    auto VerifyResult = VerifyJsonString(&string);
    ui->jsonValidateStatus->setText(VerifyResult);

    if (VerifyResult.isEmpty()) {
        BLACK(jsonEditor)
        result = JsonFromString(string);
        model.loadJson(QJsonDocument(result).toJson());
        ui->jsonTree->expandAll();
        ui->jsonTree->resizeColumnToContents(0);
    } else {
        RED(jsonEditor)
    }
}

void JsonEditor::on_formatJsonBtn_clicked()
{
    auto string = ui->jsonEditor->toPlainText();
    auto VerifyResult = VerifyJsonString(&string);
    ui->jsonValidateStatus->setText(VerifyResult);

    if (VerifyResult.isEmpty()) {
        BLACK(jsonEditor)
        ui->jsonEditor->setPlainText(JsonToString(JsonFromString(string)));
    } else {
        RED(jsonEditor)
        QvMessageBox(this, tr("Json Contains Syntax Errors"), tr("Please fix it and retry later."));
    }
}
