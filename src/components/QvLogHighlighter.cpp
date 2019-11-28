#include "QvLogHighlighter.hpp"

#define PORT_R "([0-9]|[1-9]\\d{1,3}|[1-5]\\d{4}|6[0-5]{2}[0-3][0-5])*"
#define TO_EOL "(([\\s\\S]*)|([\\d\\D]*)|([\\w\\W]*))$"

namespace Qv2ray
{
    namespace Components
    {

        Highlighter::Highlighter(QTextDocument *parent)
            : QSyntaxHighlighter(parent)
        {
            HighlightingRule rule;
            keywordFormat.setForeground(Qt::darkMagenta);
            keywordFormat.setFontWeight(QFont::Bold);
            const QString keywordPatterns[] = {
                "tcp", "udp"
            };

            for (const QString &pattern : keywordPatterns) {
                rule.pattern = QRegularExpression(pattern);
                rule.format = keywordFormat;
                highlightingRules.append(rule);
            }

            dateFormat.setFontWeight(QFont::Bold);
            dateFormat.setForeground(Qt::cyan);
            rule.pattern = QRegularExpression("\\d\\d\\d\\d/\\d\\d/\\d\\d");
            rule.format = dateFormat;
            highlightingRules.append(rule);
            //
            timeFormat.setFontWeight(QFont::Bold);
            timeFormat.setForeground(Qt::cyan);
            rule.pattern = QRegularExpression("\\d\\d:\\d\\d:\\d\\d");
            rule.format = timeFormat;
            highlightingRules.append(rule);
            //
            debugFormat.setForeground(Qt::darkGray);
            rule.pattern = QRegularExpression("\\[[Dd]ebug\\]" TO_EOL);
            rule.format = debugFormat;
            highlightingRules.append(rule);
            //
            infoFormat.setForeground(Qt::darkCyan);
            rule.pattern = QRegularExpression("\\[[Ii]nfo\\]" TO_EOL);
            rule.format = infoFormat;
            highlightingRules.append(rule);
            //
            //
            {
                // IP IPv6 Host
                ipPortFormat.setFontWeight(QFont::Bold);
                ipPortFormat.setForeground(Qt::green);
                rule.pattern = QRegularExpression("(\\d{1,2}|1\\d\\d|2[0-4]\\d|25[0-5])\\.(\\d{1,2}|1\\d\\d|2[0-4]\\d|25[0-5])\\.(\\d{1,2}|1\\d\\d|2[0-4]\\d|25[0-5])\\.(\\d{1,2}|1\\d\\d|2[0-4]\\d|25[0-5])\\:" PORT_R);
                rule.pattern.setPatternOptions(QRegularExpression::PatternOption::ExtendedPatternSyntaxOption);
                rule.format = ipPortFormat;
                highlightingRules.append(rule);
                //
                ip6PortFormat.setFontWeight(QFont::Bold);
                ip6PortFormat.setForeground(Qt::green);
                rule.pattern = QRegularExpression("\\[\\s*((([0-9A-Fa-f]{1,4}:){7}([0-9A-Fa-f]{1,4}|:))|(([0-9A-Fa-f]{1,4}:){6}(:[0-9A-Fa-f]{1,4}|((25[0-5]|2[0-4]\\d|1\\d\\d|[1-9]?\\d)(\\.(25[0-5]|2[0-4]\\d|1\\d\\d|[1-9]?\\d)){3})|:))|(([0-9A-Fa-f]{1,4}:){5}(((:[0-9A-Fa-f]{1,4}){1,2})|:((25[0-5]|2[0-4]\\d|1\\d\\d|[1-9]?\\d)(\\.(25[0-5]|2[0-4]\\d|1\\d\\d|[1-9]?\\d)){3})|:))|(([0-9A-Fa-f]{1,4}:){4}(((:[0-9A-Fa-f]{1,4}){1,3})|((:[0-9A-Fa-f]{1,4})?:((25[0-5]|2[0-4]\\d|1\\d\\d|[1-9]?\\d)(\\.(25[0-5]|2[0-4]\\d|1\\d\\d|[1-9]?\\d)){3}))|:))|(([0-9A-Fa-f]{1,4}:){3}(((:[0-9A-Fa-f]{1,4}){1,4})|((:[0-9A-Fa-f]{1,4}){0,2}:((25[0-5]|2[0-4]\\d|1\\d\\d|[1-9]?\\d)(\\.(25[0-5]|2[0-4]\\d|1\\d\\d|[1-9]?\\d)){3}))|:))|(([0-9A-Fa-f]{1,4}:){2}(((:[0-9A-Fa-f]{1,4}){1,5})|((:[0-9A-Fa-f]{1,4}){0,3}:((25[0-5]|2[0-4]\\d|1\\d\\d|[1-9]?\\d)(\\.(25[0-5]|2[0-4]\\d|1\\d\\d|[1-9]?\\d)){3}))|:))|(([0-9A-Fa-f]{1,4}:){1}(((:[0-9A-Fa-f]{1,4}){1,6})|((:[0-9A-Fa-f]{1,4}){0,4}:((25[0-5]|2[0-4]\\d|1\\d\\d|[1-9]?\\d)(\\.(25[0-5]|2[0-4]\\d|1\\d\\d|[1-9]?\\d)){3}))|:))|(:(((:[0-9A-Fa-f]{1,4}){1,7})|((:[0-9A-Fa-f]{1,4}){0,5}:((25[0-5]|2[0-4]\\d|1\\d\\d|[1-9]?\\d)(\\.(25[0-5]|2[0-4]\\d|1\\d\\d|[1-9]?\\d)){3}))|:)))(%.+)?\\s*\\]:" PORT_R);
                rule.pattern.setPatternOptions(QRegularExpression::PatternOption::ExtendedPatternSyntaxOption);
                rule.format = ip6PortFormat;
                highlightingRules.append(rule);
                //
                hostFormat.setFontWeight(QFont::Bold);
                hostFormat.setForeground(Qt::yellow);
                rule.pattern = QRegularExpression("([a-zA-Z0-9]([a-zA-Z0-9\\-]{0,61}[a-zA-Z0-9])?\\.)+[a-zA-Z]{2,6}(/|):" PORT_R);
                rule.pattern.setPatternOptions(QRegularExpression::PatternOption::ExtendedPatternSyntaxOption);
                rule.format = hostFormat;
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
            rejectedFormat.setForeground(Qt::red);
            rule.pattern = QRegularExpression("\\srejected\\s" TO_EOL);
            rule.format = rejectedFormat;
            highlightingRules.append(rule);
            //
            x.setFontWeight(QFont::Bold);
            x.setForeground(Qt::darkGreen);
            rule.pattern = QRegularExpression(" v2ray.com(/(\\w*))*: ");
            rule.format = x;
            highlightingRules.append(rule);
            //
            warningFormat.setFontWeight(QFont::Bold);
            warningFormat.setForeground(QColor::fromRgb(230, 180, 0));
            rule.pattern = QRegularExpression("\\[[Ww]arning\\]" TO_EOL);
            rule.format = warningFormat;
            highlightingRules.append(rule);
            //
            failedFormat.setFontWeight(QFont::Bold);
            failedFormat.setForeground(Qt::red);
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
