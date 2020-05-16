#pragma once

#include <QObject>
#include <QRegularExpression>
#include <QStringList>
#include <QSyntaxHighlighter>
#include <QVector>

namespace Qv2ray::ui
{
    struct JsonHighlightingRule
    {
        QString name;
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    class vCoreConfigJsonHighlighter : public QSyntaxHighlighter
    {
        Q_OBJECT
      public:
        vCoreConfigJsonHighlighter(QTextDocument *parent = nullptr);

      private:
        QVector<JsonHighlightingRule> highlightingRules;
        void SetRule(const QString &kind, const QString &pattern, QTextCharFormat format);
        void highlightBlock(const QString &text) override;
    };
} // namespace Qv2ray::ui
using namespace Qv2ray::ui;
