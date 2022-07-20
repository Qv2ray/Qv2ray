#pragma once

#include <QVariant>
#include <stdexcept>

namespace Qv2ray::components::QueryParser
{
    class parser_error_base : public std::runtime_error
    {
      public:
        parser_error_base(const std::string &ss) : std::runtime_error(ss){};
    };

    class tokenizer_error : public parser_error_base
    {
      public:
        using parser_error_base::parser_error_base;
    };

    class syntax_error : public parser_error_base
    {
      public:
        using parser_error_base::parser_error_base;
    };

    class semantic_error : public parser_error_base
    {
      public:
        using parser_error_base::parser_error_base;
    };

    class evaluation_error : public std::runtime_error
    {
      public:
        using std::runtime_error::runtime_error;
    };

    namespace Tokenizer
    {
        Q_NAMESPACE
        enum TokenOperator
        {
            Op_Invalid,
            Space,
            DQuote,
            SQuote,

            SemiColon,

            Or,
            And,
            Not,

            Equal,
            NotEqual,

            LessThan,
            GreaterThan,
            LessEqualThan,
            GreaterEqualThan,

            Comma,
        };
        Q_ENUM_NS(TokenOperator)

        enum TokenType
        {
            Token_Invalid,
            Token_UnquotedString,
            Token_QuotedString,
            Token_Operator,
            Token_Number,
            Token_Boolean,
        };
        Q_ENUM_NS(TokenType)

        struct Token
        {
            TokenType type = Token_Invalid;

            TokenOperator op = Op_Invalid;
            QString quoted{};
            QString unquoted{};
            long long number = 0;
            bool boolean = false;

            QVariant toVariant() const
            {
                switch (type)
                {
                    case Token_Invalid: return QVariant{}; break;
                    case Token_UnquotedString: return unquoted; break;
                    case Token_QuotedString: return quoted; break;
                    case Token_Operator: return op; break;
                    case Token_Number: return number; break;
                    case Token_Boolean: return boolean; break;
                }

                Q_UNREACHABLE();
            }
        };

        Token Parse_Token(QString &prog);
        qsizetype ReadSpaces(QString &str);
        bool ReadNumber(QString &str, long long &out);
        bool ReadBoolean(QString &str, bool &out);
        bool ReadQuoted(QString &source, QString &out);
        bool ReadIdentifier(QString &source, QString &out);

        QList<Token> Tokenize(const QString &str);
    } // namespace Tokenizer

    namespace SyntaxAnalyzer
    {
        struct SyntaxStatement
        {
            struct VAArg
            {
                Tokenizer::TokenOperator op = Tokenizer::Op_Invalid;
                QVariant val;
                explicit VAArg(Tokenizer::TokenOperator _op) : op(_op){};
                explicit VAArg(const QVariant &_val) : val(_val){};
            };
            Tokenizer::TokenOperator prefixop = Tokenizer::Op_Invalid;
            QVariant arg0;

            Tokenizer::TokenOperator op = Tokenizer::Op_Invalid;
            QList<VAArg> va_args;
        };

        QList<SyntaxStatement> SyntaxAnalyze(const QList<Tokenizer::Token> &tokens);
    } // namespace SyntaxAnalyzer

    namespace SemanticAnalyzer
    {
        Q_NAMESPACE
        enum class Operator
        {
            Invalid = Tokenizer::TokenOperator::Op_Invalid,
            Or = Tokenizer::TokenOperator::Or,
            And = Tokenizer::TokenOperator::And,
            Not = Tokenizer::TokenOperator::Not,

            Equal = Tokenizer::TokenOperator::Equal,
            NotEqual = Tokenizer::TokenOperator::NotEqual,

            LessThan = Tokenizer::TokenOperator::LessThan,
            GreaterThan = Tokenizer::TokenOperator::GreaterThan,
            LessEqualThan = Tokenizer::TokenOperator::LessEqualThan,
            GreaterEqualThan = Tokenizer::TokenOperator::GreaterEqualThan,
        };
        Q_ENUM_NS(Operator)

        struct Statement
        {
            QVariant oprand;
            Operator op = Operator::Invalid;

            bool hasArgList = false;
            QVariant arg;
            struct
            {
                Operator argsop = Operator::Invalid;
                QList<QVariant> args;
            } args;
        };

        typedef QList<Statement> Program;
        Program SemanticAnalyze(const QList<SyntaxAnalyzer::SyntaxStatement> &statements);
    } // namespace SemanticAnalyzer

    inline SemanticAnalyzer::Program ParseProgram(const QString &source)
    {
        using namespace SemanticAnalyzer;
        using namespace SyntaxAnalyzer;
        using namespace Tokenizer;
        return SemanticAnalyze(SyntaxAnalyze(Tokenize(source)));
    }

    bool EvaluateProgram(const SemanticAnalyzer::Program &prog, const QVariantMap &variables, Qt::CaseSensitivity caseSensitive = Qt::CaseSensitive);

} // namespace Qv2ray::components::QueryParser
