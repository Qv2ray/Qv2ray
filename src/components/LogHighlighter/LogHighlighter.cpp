#include "LogHighlighter.hpp"

#define REGEX_IPV6_ADDR                                                                                                                                                  \
    R"(\[\s*((([0-9A-Fa-f]{1,4}:){7}([0-9A-Fa-f]{1,4}|:))|(([0-9A-Fa-f]{1,4}:){6}(:[0-9A-Fa-f]{1,4}|((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3})|:))|(([0-9A-Fa-f]{1,4}:){5}(((:[0-9A-Fa-f]{1,4}){1,2})|:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3})|:))|(([0-9A-Fa-f]{1,4}:){4}(((:[0-9A-Fa-f]{1,4}){1,3})|((:[0-9A-Fa-f]{1,4})?:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:))|(([0-9A-Fa-f]{1,4}:){3}(((:[0-9A-Fa-f]{1,4}){1,4})|((:[0-9A-Fa-f]{1,4}){0,2}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:))|(([0-9A-Fa-f]{1,4}:){2}(((:[0-9A-Fa-f]{1,4}){1,5})|((:[0-9A-Fa-f]{1,4}){0,3}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:))|(([0-9A-Fa-f]{1,4}:){1}(((:[0-9A-Fa-f]{1,4}){1,6})|((:[0-9A-Fa-f]{1,4}){0,4}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:))|(:(((:[0-9A-Fa-f]{1,4}){1,7})|((:[0-9A-Fa-f]{1,4}){0,5}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:)))(%.+)?\s*\])"
#define REGEX_IPV4_ADDR R"((\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5]))"
#define REGEX_PORT_NUMBER R"(([0-9]|[1-9]\d{1,3}|[1-5]\d{4}|6[0-5]{2}[0-3][0-5])*)"
#define TO_EOL "(([\\s\\S]*)|([\\d\\D]*)|([\\w\\W]*))$"

namespace Qv2ray::components::LogHighlighter
{
    LogHighlighter::LogHighlighter(QTextDocument *parent) : QSyntaxHighlighter(parent)
    {
    }

    void LogHighlighter::loadRules(bool darkMode)
    {
        highlightingRules.clear();
        HighlightingRule rule;

        if (darkMode)
        {
            tcpudpFormat.setForeground(QColor(0, 200, 230));
            ipHostFormat.setForeground(Qt::yellow);
            warningFormat.setForeground(QColor(255, 160, 15));
        }
        else
        {
            ipHostFormat.setForeground(Qt::black);
            ipHostFormat.setFontWeight(QFont::Bold);
            warningFormat.setForeground(Qt::white);
            tcpudpFormat.setForeground(QColor(0, 52, 130));
            warningFormat.setBackground(QColor(255, 160, 15));
        }

        for (const auto &pattern : { "tcp", "udp" })
        {
            tcpudpFormat.setFontWeight(QFont::Bold);
            rule.pattern = QRegularExpression(pattern);
            rule.format = tcpudpFormat;
            highlightingRules.append(rule);
        }

        dateFormat.setForeground(darkMode ? Qt::cyan : Qt::darkCyan);
        rule.pattern = QRegularExpression(u"\\d\\d\\d\\d/\\d\\d/\\d\\d"_qs);
        rule.format = dateFormat;
        highlightingRules.append(rule);
        //
        timeFormat.setForeground(darkMode ? Qt::cyan : Qt::darkCyan);
        rule.pattern = QRegularExpression(u"\\d\\d:\\d\\d:\\d\\d"_qs);
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
            rule.pattern.setPatternOptions(QRegularExpression::ExtendedPatternSyntaxOption);
            rule.format = ipHostFormat;
            highlightingRules.append(rule);
            //
            rule.pattern = QRegularExpression(REGEX_IPV6_ADDR ":" REGEX_PORT_NUMBER);
            rule.pattern.setPatternOptions(QRegularExpression::ExtendedPatternSyntaxOption);
            rule.format = ipHostFormat;
            highlightingRules.append(rule);
            //
            rule.pattern = QRegularExpression("([a-zA-Z0-9]([a-zA-Z0-9\\-]{0,61}[a-zA-Z0-9])?\\.)+[a-zA-Z]{2,6}(/|):" REGEX_PORT_NUMBER);
            rule.pattern.setPatternOptions(QRegularExpression::PatternOption::ExtendedPatternSyntaxOption);
            rule.format = ipHostFormat;
            highlightingRules.append(rule);
        }

        const static QColor darkGreenColor(10, 180, 0);
        //
        //
        acceptedFormat.setForeground(darkGreenColor);
        acceptedFormat.setFontItalic(true);
        acceptedFormat.setFontWeight(QFont::Bold);
        rule.pattern = QRegularExpression(u"\\saccepted\\s"_qs);
        rule.format = acceptedFormat;
        highlightingRules.append(rule);
        //
        rejectedFormat.setFontWeight(QFont::Bold);
        rejectedFormat.setBackground(Qt::red);
        rejectedFormat.setForeground(Qt::white);
        rejectedFormat.setFontItalic(true);
        rejectedFormat.setFontWeight(QFont::Bold);
        rule.pattern = QRegularExpression("\\srejected\\s" TO_EOL);
        rule.format = rejectedFormat;
        highlightingRules.append(rule);
        //
        v2rayComponentFormat.setForeground(darkMode ? darkGreenColor : Qt::darkYellow);
        rule.pattern = QRegularExpression(uR"( (\w+\/)+\w+: )"_qs);
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
        rule.pattern = QRegularExpression(u"failed"_qs);
        rule.format = failedFormat;
        highlightingRules.append(rule);
        //
        qvAppLogFormat.setForeground(darkMode ? Qt::cyan : Qt::darkCyan);
        rule.pattern = QRegularExpression(u"\\[[A-Z]*\\]:"_qs);
        rule.format = qvAppLogFormat;
        highlightingRules.append(rule);
        //
        qvAppDebugLogFormat.setForeground(darkMode ? Qt::yellow : Qt::darkYellow);
        rule.pattern = QRegularExpression(uR"( \[\w+\] )"_qs);
        rule.format = qvAppDebugLogFormat;
        highlightingRules.append(rule);
    }

    void LogHighlighter::highlightBlock(const QString &text)
    {
        for (const HighlightingRule &rule : qAsConst(highlightingRules))
        {
            auto matchIterator = rule.pattern.globalMatch(text);

            while (matchIterator.hasNext())
            {
                QRegularExpressionMatch match = matchIterator.next();
                setFormat(match.capturedStart(), match.capturedLength(), rule.format);
            }
        }

        setCurrentBlockState(0);
    }
} // namespace Qv2ray::components::LogHighlighter
