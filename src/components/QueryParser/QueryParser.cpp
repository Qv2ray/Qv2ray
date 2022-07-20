#include "QueryParser.hpp"

using namespace Qv2ray::components::QueryParser;

const static inline QMap<Tokenizer::TokenOperator, QString> operator_maps{
    { Tokenizer::Space, " " },
    { Tokenizer::DQuote, "\"" },
    { Tokenizer::SQuote, "'" },
    { Tokenizer::SemiColon, ";" },
    { Tokenizer::Or, "|" },
    { Tokenizer::And, "&" },
    { Tokenizer::Not, "!" },
    { Tokenizer::Equal, "=" },
    { Tokenizer::NotEqual, "!=" },
    { Tokenizer::LessThan, "<" },
    { Tokenizer::GreaterThan, ">" },
    { Tokenizer::LessEqualThan, "<=" },
    { Tokenizer::GreaterEqualThan, ">=" },
    { Tokenizer::Comma, "," },
};

namespace Qv2ray::components::QueryParser::Tokenizer
{
    template<typename Pred>
    static bool ReadUntil(QString &source, QString &out, Pred pred)
    {
        int len = 0;
        for (len = 0; len < source.length(); len++)
            if (pred(source.at(len)))
                break;
        if (len == 0)
            return false;
        out = source.left(len);
        source.remove(0, len);
        return true;
    }

    template<typename Pred>
    static bool ReadWhile(QString &source, QString &out, Pred pred)
    {
        return ReadUntil(source, out, std::not_fn(pred));
    }

    bool ReadIdentifier(QString &source, QString &out)
    {
        if (source.isEmpty())
            throw tokenizer_error("null identifier");

        if (source[0].isDigit())
            throw tokenizer_error("identifier must not start with a digit");

        return ReadUntil(source, out, [](QChar c) { return std::find(operator_maps.cbegin(), operator_maps.cend(), c) != operator_maps.end(); });
    }

    bool ReadOperator(QString &source, TokenOperator &out)
    {
        if (source.isEmpty())
            return false;

        if (source[0] == ';')
        {
            source.remove(0, 1);
            out = SemiColon;
            return true;
        }

        QString opstring;

        int i = 0;
        for (i = 0; i < 2; i++)
        {
            if (source.length() - 1 < i)
                break;
            const auto opchar = source[i];
            if (opchar.isSpace())
                continue;
            if (std::find(operator_maps.cbegin(), operator_maps.cend(), opchar) != operator_maps.end())
                opstring += opchar;
            else
                break;
        }
        if (opstring.isEmpty())
            return false;
        if (std::find(operator_maps.cbegin(), operator_maps.cend(), opstring) == operator_maps.end())
        {
            i--;
            opstring.chop(1);
        }
        out = operator_maps.key(opstring);
        source.remove(0, i);
        return true;
    }

    Token Parse_Token(QString &prog)
    {
        prog = prog.trimmed();

        Token token;

        if (ReadNumber(prog, token.number))
        {
            token.type = Token_Number;
            return token;
        }

        if (ReadBoolean(prog, token.boolean))
        {
            token.type = Token_Boolean;
            return token;
        }

        if ((prog.startsWith(QChar{ '"' }) || prog.startsWith(QChar{ '\'' })) && ReadQuoted(prog, token.quoted))
        {
            token.type = Token_QuotedString;
            return token;
        }

        if (ReadIdentifier(prog, token.unquoted))
        {
            token.type = Token_UnquotedString;
            return token;
        }

        if (ReadOperator(prog, token.op))
        {
            token.type = Token_Operator;
            return token;
        }

        token.type = Token_Invalid;
        return token;
    }

    bool ReadQuoted(QString &source, QString &out)
    {
        assert(source.startsWith(QChar{ '"' }));

        bool escape = false;
        qsizetype len = 0;
        for (len = 1; len < source.size(); len++)
        {
            // Saw a '\' and we are not in escape mode, meaning that the next char will be escaped.
            if (source[len] == '\\' && !escape)
            {
                escape = true;
                continue;
            }

            if (escape && source[len - 1] == '\\')
            {
                escape = false;
                continue;
            }

            if (source[len] == '\"')
            {
                len++;
                break;
            }
        }

        out = source.left(len);
        if (len == source.size() && !out.endsWith('\"'))
            throw tokenizer_error("unterminated quoted string: " + source.toStdString());
        source.remove(0, len);
        return true;
    }

    bool ReadNumber(QString &str, long long &out)
    {
        QString outstr;
        bool result = ReadWhile(str, outstr, [](QChar c) { return c.isDigit(); });
        out = outstr.toLongLong();
        return result;
    }

    qsizetype ReadSpaces(QString &str)
    {
        QString spaces;
        const auto hasSpaces = ReadWhile(str, spaces, [](QChar c) { return c.isSpace(); });
        assert(hasSpaces == !spaces.isEmpty() || spaces.front() == spaces.back());
        return spaces.length();
    }

    bool ReadBoolean(QString &str, bool &out)
    {

        if (str.length() >= 4 && str.first(4).toLower() == u"true"_qs)
        {
            out = true;
            str.remove(0, 4);
            return true;
        }

        if (str.length() >= 5 && str.first(5).toLower() == u"false"_qs)
        {
            out = false;
            str.remove(0, 5);
            return true;
        }
        return false;
    }

    QList<Token> Tokenize(const QString &str)
    {
        QList<Token> tokens;
        for (auto &line : str.split('\n'))
            while (!line.isEmpty())
                if (const auto token = Parse_Token(line); token.type != Token_Invalid)
                    tokens << token;
                else
                    throw tokenizer_error("Cannot process a token, remaining: " + line.toStdString());
        return tokens << Token{ Token_Operator, SemiColon };
    }

} // namespace Qv2ray::components::QueryParser::Tokenizer

QList<SyntaxAnalyzer::SyntaxStatement> SyntaxAnalyzer::SyntaxAnalyze(const QList<Tokenizer::Token> &tokens)
{
    QList<SyntaxStatement> statements;

    enum
    {
        State_ExpectOp0,
        State_ExpectArg0,
        State_ExpectOp,
        State_ExpectArgs,
    } state = State_ExpectOp0;

    SyntaxStatement statement;
    state = State_ExpectOp0;
    bool needOp = false;
    for (const auto &token : tokens)
    {
        if (token.type == Tokenizer::Token_Invalid)
            throw syntax_error("Invalid Token");

        if (token.type == Tokenizer::Token_Operator && token.op == Tokenizer::SemiColon)
        {
            // Empty statement
            if (state == State_ExpectOp0)
                continue;
            state = State_ExpectOp0;
            needOp = false;
            statements << statement;
            statement = SyntaxStatement{};
            continue;
        }

        switch (state)
        {
            case State_ExpectOp0:
            {
                // Optional Operator 0
                if (token.type == Tokenizer::Token_Operator)
                {
                    statement.prefixop = token.op;
                    state = State_ExpectArg0;
                    break;
                }
                [[fallthrough]];
            }
            case State_ExpectArg0:
            {
                if (token.type == Tokenizer::Token_Boolean || token.type == Tokenizer::Token_Number || token.type == Tokenizer::Token_UnquotedString)
                    statement.arg0 = token.toVariant();
                else
                    throw syntax_error("Value expected.");
                state = State_ExpectOp;
                break;
            }
            case State_ExpectOp:
            {
                if (token.type != Tokenizer::Token_Operator)
                    throw syntax_error("Operator expected.");
                statement.op = token.op;
                state = State_ExpectArgs;
                break;
            }
            case State_ExpectArgs:
            {
                if (token.type == Tokenizer::Token_Operator)
                {
                    if (!needOp)
                        throw syntax_error("Value expected.");
                    if (token.op == Tokenizer::Comma || token.op == Tokenizer::Or || token.op == Tokenizer::And)
                        statement.va_args.append(SyntaxStatement::VAArg{ token.op }), needOp = false;
                    else
                        throw syntax_error("Unsupported operator in vaargs.");
                }
                else
                {
                    if (needOp)
                        throw syntax_error("Operator expected.");
                    if (token.type == Tokenizer::Token_QuotedString)
                    {
                        auto quoted = token.quoted;
                        quoted.remove(0, 1).chop(1);
                        statement.va_args.append(SyntaxStatement::VAArg{ quoted }), needOp = true;
                    }
                    else
                    {
                        statement.va_args.append(SyntaxStatement::VAArg{ token.toVariant() }), needOp = true;
                    }
                }
                break;
            }
        }
    }
    return statements;
}

SemanticAnalyzer::Program SemanticAnalyzer::SemanticAnalyze(const QList<SyntaxAnalyzer::SyntaxStatement> &statements)
{
    QList<Statement> result;
    result.reserve(statements.size());
    for (const auto &stmt : statements)
    {
        Statement ss;
        ss.oprand = stmt.arg0;
        ss.op = (Operator) stmt.op;

        // Has prefixop
        if (stmt.prefixop != Tokenizer::Op_Invalid)
        {
            if (stmt.op != Tokenizer::Op_Invalid)
                throw semantic_error("prefix operator and regular operators should not exist at the same time.");
            if (stmt.prefixop != Tokenizer::Not)
                throw semantic_error("prefix operator should only be '!' (negation operator)");

            ss.op = Operator::Equal;
            ss.arg = false;
            result << ss;
            continue;
        }
        else if (stmt.op == Tokenizer::Op_Invalid)
        {
            ss.op = Operator::Equal;
            ss.arg = true;
            result << ss;
            continue;
        }

        if (!(stmt.op == Tokenizer::Equal            //
              || stmt.op == Tokenizer::NotEqual      //
              || stmt.op == Tokenizer::LessThan      //
              || stmt.op == Tokenizer::GreaterThan   //
              || stmt.op == Tokenizer::LessEqualThan //
              || stmt.op == Tokenizer::GreaterEqualThan))
            throw semantic_error("unnsupported operator");

        ss.hasArgList = stmt.va_args.size() > 1;

        if (stmt.va_args.isEmpty())
            throw semantic_error("expected value");

        if (!ss.hasArgList)
        {
            ss.arg = stmt.va_args.first().val;
            result << ss;
            continue;
        }

        if (stmt.op != Tokenizer::Equal && stmt.op != Tokenizer::NotEqual)
            throw semantic_error("list arguments only supports equality operator");

        Operator vaops = Operator::Invalid;
        for (auto vaarg : stmt.va_args)
        {
            if (vaarg.op == Tokenizer::Op_Invalid)
            {
                ss.args.args << vaarg.val;
                continue;
            }

            if (vaarg.op == Tokenizer::Comma)
            {
                if (stmt.op == Tokenizer::Equal)
                    vaarg.op = Tokenizer::And;
                else if (stmt.op == Tokenizer::NotEqual)
                    vaarg.op = Tokenizer::Or;
                else
                    Q_UNREACHABLE();
            }

            // First operator, set to check cache.
            if (vaarg.op != Tokenizer::Op_Invalid && vaops == Operator::Invalid)
                vaops = (Operator) vaarg.op;
            else if (vaops != (Operator) vaarg.op)
                throw semantic_error("inconsistant list operators");
        }
        ss.args.argsop = vaops;
        result << ss;
    }
    return result;
}

bool Qv2ray::components::QueryParser::EvaluateProgram(const SemanticAnalyzer::Program &prog, const QVariantMap &variables, Qt::CaseSensitivity caseSensitive)
{
    bool result = true;
    for (const auto &statement : prog)
    {
        const auto oprand = statement.oprand.toString();
        if (!variables[oprand].isValid())
            throw evaluation_error("unknown identifier: " + oprand.toStdString());

        const auto opcode = statement.op;

        const auto hasArgList = statement.hasArgList;

        const auto _contains = [caseSensitive](const QVariantList &list, const QVariant &v) -> bool
        {
            const auto _Pred = [&](const QVariant &vv) -> bool
            {
                if (v.typeId() == QMetaType::QString)
                    return v.toString().compare(vv.toString(), caseSensitive) == 0;
                return vv == v;
            };
            return std::find_if(list.begin(), list.end(), _Pred) != list.end();
        };

        switch (opcode)
        {
            case SemanticAnalyzer::Operator::Equal:
            case SemanticAnalyzer::Operator::NotEqual:
            {
                const auto positivePredicate = opcode == SemanticAnalyzer::Operator::Equal;

                const auto variableMetaTypeId = variables[oprand].metaType().id();
                if (variableMetaTypeId == QMetaType::QStringList || variableMetaTypeId == QMetaType::QVariantList)
                {
                    QVariantList list = statement.args.args;

                    if (!hasArgList)
                        list << statement.arg;

                    const auto valList = variables[oprand].toList();

                    // If Or, default value = true;
                    bool r = statement.args.argsop == SemanticAnalyzer::Operator::And;
                    for (const auto &a : list)
                    {
                        if (statement.args.argsop == SemanticAnalyzer::Operator::And)
                            r &= _contains(valList, a);
                        else
                            r |= _contains(valList, a);
                    }

                    if (positivePredicate)
                        result &= r;
                    else
                        result &= !r;
                }
                else
                {
                    if (positivePredicate)
                        result &= statement.arg == variables[oprand];
                    else
                        result &= statement.arg != variables[oprand];
                }
                break;
            }
            case SemanticAnalyzer::Operator::LessThan:
            {
                Q_ASSERT(!hasArgList);
                result &= variables[oprand].toULongLong() < statement.arg.toULongLong();
                break;
            }
            case SemanticAnalyzer::Operator::GreaterThan:
            {
                Q_ASSERT(!hasArgList);
                result &= variables[oprand].toULongLong() > statement.arg.toULongLong();
                break;
            }
            case SemanticAnalyzer::Operator::LessEqualThan:
            {
                Q_ASSERT(!hasArgList);
                result &= variables[oprand].toULongLong() <= statement.arg.toULongLong();
                break;
            }
            case SemanticAnalyzer::Operator::GreaterEqualThan:
            {
                Q_ASSERT(!hasArgList);
                result &= variables[oprand].toULongLong() >= statement.arg.toULongLong();
                break;
            }

            default: Q_UNREACHABLE();
        }
    }
    return result;
}
