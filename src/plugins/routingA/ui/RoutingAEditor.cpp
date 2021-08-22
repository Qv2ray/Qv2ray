#include "RoutingAEditor.hpp"

#include "RoutingAPlugin.hpp"
#include "common/RoutingAConverter.hpp"
#include "core/libRoutingA.hpp"

#define REGEX_IPV6_ADDR                                                                                                                                                  \
    uR"(\[\s*((([0-9A-Fa-f]{1,4}:){7}([0-9A-Fa-f]{1,4}|:))|(([0-9A-Fa-f]{1,4}:){6}(:[0-9A-Fa-f]{1,4}|((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3})|:))|(([0-9A-Fa-f]{1,4}:){5}(((:[0-9A-Fa-f]{1,4}){1,2})|:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3})|:))|(([0-9A-Fa-f]{1,4}:){4}(((:[0-9A-Fa-f]{1,4}){1,3})|((:[0-9A-Fa-f]{1,4})?:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:))|(([0-9A-Fa-f]{1,4}:){3}(((:[0-9A-Fa-f]{1,4}){1,4})|((:[0-9A-Fa-f]{1,4}){0,2}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:))|(([0-9A-Fa-f]{1,4}:){2}(((:[0-9A-Fa-f]{1,4}){1,5})|((:[0-9A-Fa-f]{1,4}){0,3}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:))|(([0-9A-Fa-f]{1,4}:){1}(((:[0-9A-Fa-f]{1,4}){1,6})|((:[0-9A-Fa-f]{1,4}){0,4}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:))|(:(((:[0-9A-Fa-f]{1,4}){1,7})|((:[0-9A-Fa-f]{1,4}){0,5}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:)))(%.+)?\s*\])"_qs
#define REGEX_IPV4_ADDR uR"((\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5]))"_qs

RoutingAHighlighter::RoutingAHighlighter(QTextDocument *parent) : QSyntaxHighlighter(parent)
{
    highlightingRules.clear();

    keywordsFormat.setForeground(QColor(31, 134, 229));
    builtinTagFormat.setForeground(QColor(141, 115, 218));
    geoIPSiteFormat.setForeground(QColor(218, 158, 106));
    commentFormat.setForeground(QColor(58, 189, 150));
    literalFormat.setForeground(QColor(106, 186, 84));
    quotedFormat.setForeground(QColor(188, 130, 72));

    {
        HighlightingRule rule;
        rule.pattern = QRegularExpression{ uR"(^(inbound|outbound|default):)"_qs };
        rule.format = keywordsFormat;
        highlightingRules << rule;
    }
    {
        HighlightingRule rule;
        rule.pattern = QRegularExpression{ uR"(\b(proxy|direct|block)\b)"_qs };
        rule.format = builtinTagFormat;
        highlightingRules << rule;
    }
    {
        HighlightingRule rule;
        rule.pattern = QRegularExpression{ uR"(\bgeo(ip|site):[\w-]*)"_qs };
        rule.format = geoIPSiteFormat;
        highlightingRules << rule;
    }
    {
        HighlightingRule rule;
        rule.pattern = QRegularExpression{ uR"(^#.*$)"_qs };
        rule.format = commentFormat;
        highlightingRules << rule;
    }
    {
        HighlightingRule rule;
        rule.pattern = QRegularExpression{ uR"(\b\d+\b)"_qs };
        rule.format = literalFormat;
        highlightingRules << rule;
    }
    {
        HighlightingRule rule;
        rule.pattern = QRegularExpression(REGEX_IPV4_ADDR);
        rule.pattern.setPatternOptions(QRegularExpression::ExtendedPatternSyntaxOption);
        rule.format = literalFormat;
        highlightingRules << rule;
    }
    {
        HighlightingRule rule;
        rule.pattern = QRegularExpression(REGEX_IPV6_ADDR);
        rule.pattern.setPatternOptions(QRegularExpression::ExtendedPatternSyntaxOption);
        rule.format = literalFormat;
        highlightingRules << rule;
    }
    {
        HighlightingRule rule;
        rule.pattern = QRegularExpression(uR"("[^"]*"|'[^']*')"_qs);
        rule.pattern.setPatternOptions(QRegularExpression::ExtendedPatternSyntaxOption);
        rule.format = quotedFormat;
        highlightingRules << rule;
    }
}

void RoutingAHighlighter::highlightBlock(const QString &text)
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

RoutingAEditor::RoutingAEditor(QWidget *parent) : Qv2rayPlugin::Gui::PluginMainWindowWidget(parent)
{
    setupUi(this);
    auto font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    logBrowser->setFont(font);
    routingAText->setFont(font);
    new RoutingAHighlighter(routingAText->document());
    new RoutingAHighlighter(logBrowser->document());
}

void RoutingAEditor::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type())
    {
        case QEvent::LanguageChange: retranslateUi(this); break;
        default: break;
    }
}

void RoutingAEditor::on_routingAText_textChanged()
{
    try
    {
        logBrowser->clear();
        inboundCLabel->setText(tr("N/A"));
        outboundCLabel->setText(tr("N/A"));
        rulesCLabel->setText(tr("N/A"));
        defaultOutboundLabel->setText(tr("N/A"));
        const auto &[defines, rules] = RoutingA::ParseRoutingA(routingAText->toPlainText());

        int inbounds = 0;
        int outbounds = 0;
        for (const auto &def : defines)
        {
            if (def.type == u"inbound"_qs)
                inbounds++;
            if (def.type == u"outbound"_qs)
                outbounds++;
            if (def.type == u"default"_qs)
                defaultOutboundLabel->setText(def.value);
        }
        inboundCLabel->setText(QString::number(inbounds));
        outboundCLabel->setText(QString::number(outbounds));
        rulesCLabel->setText(QString::number(rules.size()));

        logBrowser->setPlainText(tr("OK"));
    }
    catch (RoutingA::ParsingErrorException &e)
    {
        logBrowser->setText(tr("Exception at line ") + QString::number(e.Position) + u": \n\n"_qs + e.Message);
    }
}

void RoutingAEditor::on_showLogBtn_clicked()
{
    logBrowser->setVisible(!logBrowser->isVisible());
}

void RoutingAEditor::on_checkBtn_clicked()
{
    try
    {
        RoutingA::ParseRoutingA(routingAText->toPlainText());
        RoutingAPlugin::ShowMessageBox(tr("RoutingA Check"), tr("RoutingA Syntax Checking Passed"));
    }
    catch (RoutingA::ParsingErrorException &e)
    {
        RoutingAPlugin::ShowMessageBox(tr("Failed to Parse RoutingA"), tr("Syntax error in line ") + QString::number(e.Position));
    }
}

//#include <QMessageBox>
// int plugin_main(int argc, char *argv[])
//{
//    QApplication app{ argc, argv };
//    RoutingAPlugin plugin;
//    QObject::connect(&plugin, &RoutingAPlugin::PluginErrorMessageBox, [](auto h, auto t) { QMessageBox::warning(nullptr, h, t); });
//    RoutingAEditor e;
//    e.show();
//    return app.exec();
//}
