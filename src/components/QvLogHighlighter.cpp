#include "QvLogHighlighter.hpp"

#define PORT_R "([0-9]|[1-9]\\d{1,3}|[1-5]\\d{4}|6[0-5]{2}[0-3][0-5])*"
#define TO_EOL "(([\\s\\S]*)|([\\d\\D]*)|([\\w\\W]*))$"

namespace Qv2ray
{
    namespace Components
    {
        Highlighter::Highlighter(bool darkMode, QTextDocument *parent)
            : QSyntaxHighlighter(parent)
        {
            HighlightingRule rule;
            keywordFormat.setForeground(darkMode ? Qt::darkMagenta : Qt::magenta);
            keywordFormat.setFontWeight(QFont::Bold);
            const QString keywordPatterns[] = {
                "tcp", "udp"
            };

            for (const QString &pattern : keywordPatterns) {
                rule.pattern = QRegularExpression(pattern);
                rule.format = keywordFormat;
                highlightingRules.append(rule);
            }

            if (darkMode) {
                ipHostFormat.setForeground(Qt::yellow);
                warningFormat.setForeground(QColor(230, 180, 0));
            } else {
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
            infoFormat.setForeground(darkMode ? Qt::lightGray :  Qt::darkCyan);
            rule.pattern = QRegularExpression("\\[[Ii]nfo\\]" TO_EOL);
            rule.format = infoFormat;
            highlightingRules.append(rule);
            //
            //
            {
                // IP IPv6 Host;
                rule.pattern = QRegularExpression("(\\d{1,2}|1\\d\\d|2[0-4]\\d|25[0-5])\\.(\\d{1,2}|1\\d\\d|2[0-4]\\d|25[0-5])\\.(\\d{1,2}|1\\d\\d|2[0-4]\\d|25[0-5])\\.(\\d{1,2}|1\\d\\d|2[0-4]\\d|25[0-5])\\:" PORT_R);
                rule.pattern.setPatternOptions(QRegularExpression::PatternOption::ExtendedPatternSyntaxOption);
                rule.format = ipHostFormat;
                highlightingRules.append(rule);
                //
                rule.pattern = QRegularExpression("\\[\\s*((([0-9A-Fa-f]{1,4}:){7}([0-9A-Fa-f]{1,4}|:))|(([0-9A-Fa-f]{1,4}:){6}(:[0-9A-Fa-f]{1,4}|((25[0-5]|2[0-4]\\d|1\\d\\d|[1-9]?\\d)(\\.(25[0-5]|2[0-4]\\d|1\\d\\d|[1-9]?\\d)){3})|:))|(([0-9A-Fa-f]{1,4}:){5}(((:[0-9A-Fa-f]{1,4}){1,2})|:((25[0-5]|2[0-4]\\d|1\\d\\d|[1-9]?\\d)(\\.(25[0-5]|2[0-4]\\d|1\\d\\d|[1-9]?\\d)){3})|:))|(([0-9A-Fa-f]{1,4}:){4}(((:[0-9A-Fa-f]{1,4}){1,3})|((:[0-9A-Fa-f]{1,4})?:((25[0-5]|2[0-4]\\d|1\\d\\d|[1-9]?\\d)(\\.(25[0-5]|2[0-4]\\d|1\\d\\d|[1-9]?\\d)){3}))|:))|(([0-9A-Fa-f]{1,4}:){3}(((:[0-9A-Fa-f]{1,4}){1,4})|((:[0-9A-Fa-f]{1,4}){0,2}:((25[0-5]|2[0-4]\\d|1\\d\\d|[1-9]?\\d)(\\.(25[0-5]|2[0-4]\\d|1\\d\\d|[1-9]?\\d)){3}))|:))|(([0-9A-Fa-f]{1,4}:){2}(((:[0-9A-Fa-f]{1,4}){1,5})|((:[0-9A-Fa-f]{1,4}){0,3}:((25[0-5]|2[0-4]\\d|1\\d\\d|[1-9]?\\d)(\\.(25[0-5]|2[0-4]\\d|1\\d\\d|[1-9]?\\d)){3}))|:))|(([0-9A-Fa-f]{1,4}:){1}(((:[0-9A-Fa-f]{1,4}){1,6})|((:[0-9A-Fa-f]{1,4}){0,4}:((25[0-5]|2[0-4]\\d|1\\d\\d|[1-9]?\\d)(\\.(25[0-5]|2[0-4]\\d|1\\d\\d|[1-9]?\\d)){3}))|:))|(:(((:[0-9A-Fa-f]{1,4}){1,7})|((:[0-9A-Fa-f]{1,4}){0,5}:((25[0-5]|2[0-4]\\d|1\\d\\d|[1-9]?\\d)(\\.(25[0-5]|2[0-4]\\d|1\\d\\d|[1-9]?\\d)){3}))|:)))(%.+)?\\s*\\]:" PORT_R);
                rule.pattern.setPatternOptions(QRegularExpression::PatternOption::ExtendedPatternSyntaxOption);
                rule.format = ipHostFormat;
                highlightingRules.append(rule);
                //
                rule.pattern = QRegularExpression("([a-zA-Z0-9]([a-zA-Z0-9\\-]{0,61}[a-zA-Z0-9])?\\.)+[a-zA-Z]{2,6}(/|):" PORT_R);
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
            x.setFontWeight(QFont::Bold);
            x.setForeground(darkMode ? Qt::darkGreen : Qt::darkYellow);
            rule.pattern = QRegularExpression(" v2ray.com(/(\\w*))*: ");
            rule.format = x;
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
        }

        void Highlighter::highlightBlock(const QString &text)
        {
            for (const HighlightingRule &rule : qAsConst(highlightingRules)) {
                QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);

                while (matchIterator.hasNext()) {
                    QRegularExpressionMatch match = matchIterator.next();
                    setFormat(match.capturedStart(), match.capturedLength(), rule.format);
                }
            }

            setCurrentBlockState(0);
        }
    }
}
