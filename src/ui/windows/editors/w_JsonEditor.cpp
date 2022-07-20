#include "w_JsonEditor.hpp"

#include "Qv2rayBase/Common/Utils.hpp"
#include "ui/WidgetUIBase.hpp"

QTextCharFormat JSONHighlighter::getFormat(const Mode name)
{
    QTextCharFormat format;
    switch (name)
    {
        case MKeyword: format.setForeground(QColor(0x25, 0x77, 0xdc)); break;
        case MString: format.setForeground(QColor(0xff, 0xb6, 0x4f)); break;
        case MNumber: format.setForeground(QColor(0x8f, 0xdc, 0x74)); break;
    }
    return format;
}

void JSONHighlighter::highlightBlock(const QString &text)
{
    static const QRegularExpression m_keyRegex = QRegularExpression{ uR"(("[^\r\n:]+?")\s*:)"_qs };
    static const QVector<std::pair<QRegularExpression, Mode>> m_highlightRules{
        { QRegularExpression(uR"(\bnull\b)"_qs), MKeyword },               //
        { QRegularExpression(uR"(\btrue\b)"_qs), MKeyword },               //
        { QRegularExpression(uR"(\bfalse\b)"_qs), MKeyword },              //
        { QRegularExpression(uR"(\b(0b|0x){0,1}[\d.']+\b)"_qs), MNumber }, //
        { QRegularExpression(uR"("[^\n"]*")"_qs), MString },               //
    };

    for (auto &&rule : m_highlightRules)
    {
        auto matchIterator = rule.first.globalMatch(text);
        while (matchIterator.hasNext())
        {
            const auto match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), getFormat(rule.second));
        }
    }

    // Special treatment for key regex
    auto matchIterator = m_keyRegex.globalMatch(text);

    while (matchIterator.hasNext())
    {
        const auto match = matchIterator.next();
        setFormat(match.capturedStart(1), match.capturedLength(1), getFormat(MKeyword));
    }
}

JsonEditor::JsonEditor(QJsonObject rootObject, QWidget *parent) : QDialog(parent)
{
    setupUi(this);
    QvMessageBusConnect();
    //
    original = rootObject;
    final = rootObject;
    QString jsonString = JsonToString(rootObject);

    if (VerifyJsonString(jsonString).has_value())
    {
        QvBaselib->Warn(tr("Json Contains Syntax Errors"), tr("Original Json may contain syntax errors. Json tree is disabled."));
    }
    else
    {
        jsonTree->setModel(&model);
        model.loadJson(QJsonDocument(rootObject).toJson());
    }

    jsonEditor->setText(JsonToString(rootObject));
    new JSONHighlighter(jsonEditor->document());
    jsonTree->expandAll();
    jsonTree->resizeColumnToContents(0);
}

QvMessageBusSlotImpl(JsonEditor)
{
    switch (msg)
    {

        case MessageBus::UPDATE_COLORSCHEME: break;
    }
}

QJsonObject JsonEditor::OpenEditor()
{
    int resultCode = this->exec();
    auto string = jsonEditor->toPlainText();

    while (resultCode == QDialog::Accepted && VerifyJsonString(string).has_value())
    {
        QvBaselib->Warn(tr("Json Contains Syntax Errors"), tr("You must correct these errors before continuing."));
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
    auto VerifyResult = VerifyJsonString(string);

    if (VerifyResult)
    {
        jsonValidateStatus->setText(*VerifyResult);
        RED(jsonEditor);
    }
    else
    {
        BLACK(jsonEditor);
        final = JsonFromString(string);
        model.loadJson(QJsonDocument(final).toJson());
        jsonTree->expandAll();
        jsonTree->resizeColumnToContents(0);
    }
}

void JsonEditor::on_formatJsonBtn_clicked()
{
    auto string = jsonEditor->toPlainText();
    auto VerifyResult = VerifyJsonString(string);

    if (VerifyResult)
    {
        jsonValidateStatus->setText(*VerifyResult);
        RED(jsonEditor);
        QvBaselib->Warn(tr("Syntax Errors"), tr("Please fix the JSON errors or remove the comments before continue"));
    }
    else
    {
        BLACK(jsonEditor);
        jsonEditor->setPlainText(JsonToString(JsonFromString(string)));
        model.loadJson(QJsonDocument(JsonFromString(string)).toJson());
        jsonTree->setModel(&model);
        jsonTree->expandAll();
        jsonTree->resizeColumnToContents(0);
    }
}
