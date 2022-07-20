#pragma once

#include "MessageBus/MessageBus.hpp"
#include "QJsonModel/QJsonModel.hpp"
#include "ui_w_JsonEditor.h"

#include <QDialog>
#include <QSyntaxHighlighter>

class JSONHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT
    enum Mode
    {
        MString,
        MNumber,
        MKeyword
    };

  public:
    explicit JSONHighlighter(QTextDocument *document = nullptr) : QSyntaxHighlighter(document){};
    static QTextCharFormat getFormat(const Mode name);

  protected:
    void highlightBlock(const QString &text) override;
};

class JsonEditor
    : public QDialog
    , private Ui::JsonEditor
{
    Q_OBJECT

  public:
    explicit JsonEditor(QJsonObject rootObject = {}, QWidget *parent = nullptr);
    ~JsonEditor();
    QJsonObject OpenEditor();

  private:
    QvMessageBusSlotDecl;

  private slots:
    void on_jsonEditor_textChanged();

    void on_formatJsonBtn_clicked();

  private:
    QJsonModel model;
    QJsonObject original;
    QJsonObject final;
};
