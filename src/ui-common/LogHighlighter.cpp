#include "LogHighlighter.hpp"

#include "common/QvHelpers.hpp"

#define TO_EOL "(([\\s\\S]*)|([\\d\\D]*)|([\\w\\W]*))$"

namespace Qv2ray::ui
{
    SyntaxHighlighter::SyntaxHighlighter(bool darkMode, QTextDocument *parent) : QSyntaxHighlighter(parent)
    {
        HighlightingRule rule;
        keywordFormat.setForeground(darkMode ? Qt::darkMagenta : Qt::magenta);
        keywordFormat.setFontWeight(QFont::Bold);
        const QString keywordPatterns[] = { "tcp", "udp" };

        for (const QString &pattern : keywordPatterns)
        {
            rule.pattern = QRegularExpression(pattern);
            rule.format = keywordFormat;
            highlightingRules.append(rule);
        }

        if (darkMode)
        {
            ipHostFormat.setForeground(Qt::yellow);
            warningFormat.setForeground(QColor(230, 180, 0));
        }
        else
        {
            ipHostFormat.setForeground(Qt::black);
            ipHostFormat.setFontWeight(QFont::Bold);
            warningFormat.setForeground(Qt::white);
            warningFormat.setBackground(QColor(255, 128, 30));
        }

        //
        dateFormat.setFontWeight(QFont::Bold);
        dateFormat.setForeground(darkMode ? Qt::cyan : Qt::darkCyan);
        rule.pattern = QRegularExpression("\\d\\d\\d\\d/\\d\\d/\\d\\d");
        rule.format = dateFormat;
        highlightingRules.append(rule);
        //
        timeFormat.setFontWeight(QFont::Bold);
        timeFormat.setForeground(darkMode ? Qt::cyan : Qt::darkCyan);
        rule.pattern = QRegularExpression("\\d\\d:\\d\\d:\\d\\d");
        rule.format = timeFormat;
        highlightingRules.append(rule);
        //
        debugFormat.setForeground(Qt::darkGray);
        rule.pattern = QRegularExpression("\\[[Dd]ebug\\]" TO_EOL);
        rule.format = debugFormat;
        highlightingRules.append(rule);
        //
        infoFormat.setForeground(darkMode ? Qt::lightGray : Qt::darkCyan);
        rule.pattern = QRegularExpression("\\[[Ii]nfo\\]" TO_EOL);
        rule.format = infoFormat;
        highlightingRules.append(rule);
        //
        //
        {
            // IP IPv6 Host;
            rule.pattern = QRegularExpression(REGEX_IPV4_ADDR ":" REGEX_PORT_NUMBER);
            rule.pattern.setPatternOptions(QRegularExpression::PatternOption::ExtendedPatternSyntaxOption);
            rule.format = ipHostFormat;
            highlightingRules.append(rule);
            //
            rule.pattern = QRegularExpression(REGEX_IPV6_ADDR ":" REGEX_PORT_NUMBER);
            rule.pattern.setPatternOptions(QRegularExpression::PatternOption::ExtendedPatternSyntaxOption);
            rule.format = ipHostFormat;
            highlightingRules.append(rule);
            //
            rule.pattern = QRegularExpression("([a-zA-Z0-9]([a-zA-Z0-9\\-]{0,61}[a-zA-Z0-9])?\\.)+[a-zA-Z]{2,6}(/|):" REGEX_PORT_NUMBER);
            rule.pattern.setPatternOptions(QRegularExpression::PatternOption::ExtendedPatternSyntaxOption);
            rule.format = ipHostFormat;
            highlightingRules.append(rule);
        }
        //
        //
        acceptedFormat.setForeground(Qt::darkGreen);
        rule.pattern = QRegularExpression("\\saccepted\\s");
        rule.format = acceptedFormat;
        highlightingRules.append(rule);
        //
        rejectedFormat.setFontWeight(QFont::Bold);
        rejectedFormat.setBackground(Qt::red);
        rejectedFormat.setForeground(Qt::white);
        rule.pattern = QRegularExpression("\\srejected\\s" TO_EOL);
        rule.format = rejectedFormat;
        highlightingRules.append(rule);
        //
        v2rayComponentFormat.setFontWeight(QFont::Bold);
        v2rayComponentFormat.setForeground(darkMode ? Qt::darkGreen : Qt::darkYellow);
        rule.pattern = QRegularExpression(" v2ray.com(/(\\w*))*: ");
        rule.format = v2rayComponentFormat;
        highlightingRules.append(rule);
        //
        warningFormat.setFontWeight(QFont::Bold);
        rule.pattern = QRegularExpression("\\[[Ww]arning\\]" TO_EOL);
        rule.format = warningFormat;
        highlightingRules.append(rule);
        //
        failedFormat.setFontWeight(QFont::Bold);
        failedFormat.setBackground(Qt::red);
        failedFormat.setForeground(Qt::white);
        rule.pattern = QRegularExpression("failed");
        rule.format = failedFormat;
        highlightingRules.append(rule);
        //
        qvAppLogFormat.setFontWeight(QFont::Bold);
        qvAppLogFormat.setForeground(darkMode ? Qt::cyan : Qt::darkCyan);
        rule.pattern = QRegularExpression("\\[[A-Z]*\\]:");
        rule.format = qvAppLogFormat;
        highlightingRules.append(rule);
        //
        qvAppDebugLogFormat.setFontWeight(QFont::Bold);
        qvAppDebugLogFormat.setForeground(darkMode ? Qt::yellow : Qt::darkYellow);
        rule.pattern = QRegularExpression("\\[\\[DEBUG\\] - [A-Z]*\\]:");
        rule.format = qvAppDebugLogFormat;
        highlightingRules.append(rule);
    }

    void SyntaxHighlighter::highlightBlock(const QString &text)
    {
        for (const HighlightingRule &rule : qAsConst(highlightingRules))
        {
            QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);

            while (matchIterator.hasNext())
            {
                QRegularExpressionMatch match = matchIterator.next();
                setFormat(match.capturedStart(), match.capturedLength(), rule.format);
            }
        }

        setCurrentBlockState(0);
    }
} // namespace Qv2ray::ui
