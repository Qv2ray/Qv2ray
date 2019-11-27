﻿#include "w_JsonEditor.hpp"

#include "QvUtils.hpp"

JsonEditor::JsonEditor(QJsonObject rootObject, QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);
    original = rootObject;
    final = rootObject;
    QString jsonString  = JsonToString(rootObject);

    if (VerifyJsonString(&jsonString).isEmpty()) {
        LOG(MODULE_UI, "Begin loading Json Model")
        jsonTree->setModel(&model);
        model.loadJson(QJsonDocument(rootObject).toJson());
    } else {
        QvMessageBox(this, tr("Json Contains Syntax Errors"), tr("Original Json may contain syntax errors. Json tree is disabled."));
    }

    jsonEditor->setText(JsonToString(rootObject));
    jsonTree->expandAll();
    jsonTree->resizeColumnToContents(0);
}

QJsonObject JsonEditor::OpenEditor()
{
    int resultCode = this->exec();
    auto string = jsonEditor->toPlainText();

    while (resultCode == QDialog::Accepted && !VerifyJsonString(&string).isEmpty()) {
        QvMessageBox(this, tr("Json Contains Syntax Errors"), tr("You must correct these errors before continue."));
        resultCode = this->exec();
        string = jsonEditor->toPlainText();
    }

    return resultCode == QDialog::Accepted ? final : original;
}

JsonEditor::~JsonEditor()
{
}

void JsonEditor::on_jsonEditor_textChanged()
{
    auto string = jsonEditor->toPlainText();
    auto VerifyResult = VerifyJsonString(&string);
    jsonValidateStatus->setText(VerifyResult);

    if (VerifyResult.isEmpty()) {
        BLACK(jsonEditor)
        final = JsonFromString(string);
        model.loadJson(QJsonDocument(final).toJson());
        jsonTree->expandAll();
        jsonTree->resizeColumnToContents(0);
    } else {
        RED(jsonEditor)
    }
}

void JsonEditor::on_formatJsonBtn_clicked()
{
    auto string = jsonEditor->toPlainText();
    auto VerifyResult = VerifyJsonString(&string);
    jsonValidateStatus->setText(VerifyResult);

    if (VerifyResult.isEmpty()) {
        BLACK(jsonEditor)
        jsonEditor->setPlainText(JsonToString(JsonFromString(string)));
    } else {
        RED(jsonEditor)
        QvMessageBox(this, tr("Syntax Errors"), tr("Please fix the JSON errors before continue"));
    }
}
