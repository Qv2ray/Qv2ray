#include "JsonHighlighter.hpp"

#include "core/settings/SettingsBackend.hpp"

namespace Qv2ray::ui
{
    vCoreConfigJsonHighlighter::vCoreConfigJsonHighlighter(QTextDocument *parent) : QSyntaxHighlighter(parent)
    {
        QTextCharFormat keywordFormat;
        keywordFormat.setForeground(GlobalConfig.uiConfig.useDarkTheme ? Qt::GlobalColor::magenta : Qt::darkMagenta);
        keywordFormat.setFontWeight(QFont::Bold);
        // It's holy a dirty hack here, we'll fully utilize the vCoreConfig models.
        auto keywordPatterns = { "inbounds",
                                 "outbounds",
                                 "routing",
                                 "settings",
                                 "transport",
                                 "request",
                                 "headers",
                                 "ip",
                                 "tag",
                                 "protocol",
                                 "listen",
                                 "rules",
                                 "port",
                                 "reverse",
                                 "dsSettings",
                                 "httpSettings",
                                 "kcpSettings",
                                 "network",
                                 "quicSettings",
                                 "security",
                                 "sockopt",
                                 "tcpSettings",
                                 "tlsSettings",
                                 "wsSettings",
                                 "streamSettings",
                                 "mux",
                                 "sendThrough",
                                 "vnext",
                                 "address",
                                 "users",
                                 "alterId",
                                 "id",
                                 "level",
                                 "path",
                                 "host",
                                 "congestion",
                                 "downlinkCapacity",
                                 "header",
                                 "type",
                                 "mtu",
                                 "readBufferSize",
                                 "tti",
                                 "uplinkCapacity",
                                 "writeBufferSize",
                                 "key",
                                 "mark",
                                 "tcpFastOpen",
                                 "tproxy",
                                 "allowInsecure",
                                 "alpn",
                                 "disableSystemRoot",
                                 "certificates",
                                 "serverName",
                                 "QV2RAY_RULE_ENABLED",
                                 "QV2RAY_RULE_TAG",
                                 "QV2RAY_RULE_USE_BALANCER" };
        int i = 0;
        for (const QString &pattern : keywordPatterns)
        {
            SetRule(QString("00_KeyWord_%1").arg(i), "\"" + pattern + "\"", keywordFormat);
            ++i;
        }
        //
        // Values
        QTextCharFormat valueFormat, classFormat;
        valueFormat.setForeground(Qt::blue);
        SetRule("03_Values", "\\btrue\\b|\\bfalse\\b|\\b[0-9]+\\b", valueFormat);
        //
        // Single Line Comments
        QTextCharFormat singleLineCommentFormat;
        singleLineCommentFormat.setForeground(Qt::darkGreen);
        SetRule("z2_SingleLineComments", "//[^\n]*", singleLineCommentFormat);
    }
    void vCoreConfigJsonHighlighter::SetRule(const QString &kind, const QString &pattern, QTextCharFormat format)
    {
        JsonHighlightingRule rule;
        rule.name = kind;
        rule.pattern = QRegularExpression(pattern);
        rule.format = format;
        highlightingRules.append(rule);
    }

    void vCoreConfigJsonHighlighter::highlightBlock(const QString &text)
    {
        for (const auto &rule : std::as_const(highlightingRules))
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
